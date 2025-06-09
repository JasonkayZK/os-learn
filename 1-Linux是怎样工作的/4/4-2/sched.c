#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NLOOP_FOR_ESTIMATION 1000000000L
#define NSECS_PER_MSEC 1000000L
#define NSECS_PER_SEC 1000000000L

static unsigned long nloop_per_resol;
static struct timespec start;
static pid_t *pids;

static long diff_nsec(const struct timespec before,
                      const struct timespec after) {
  return after.tv_sec * NSECS_PER_SEC + after.tv_nsec -
         (before.tv_sec * NSECS_PER_SEC + before.tv_nsec);
}

static unsigned long estimate_loops_per_msec() {
  struct timespec before, after;
  clock_gettime(CLOCK_MONOTONIC, &before);

  for (unsigned long i = 0; i < NLOOP_FOR_ESTIMATION; i++) {
  }

  clock_gettime(CLOCK_MONOTONIC, &after);

  const long estimated_time =
      NLOOP_FOR_ESTIMATION * NSECS_PER_MSEC / diff_nsec(before, after);
  printf("estimated_time: %lu\n", estimated_time);

  return estimated_time;
}

static void load(void) {
  for (unsigned long i = 0; i < nloop_per_resol; i++) {
  }
}

static void child_fn(const int id, struct timespec *buf, const int nrecord) {
  int i;
  for (i = 0; i < nrecord; i++) {
    struct timespec ts;

    load();  // Costing compute simulation
    clock_gettime(CLOCK_MONOTONIC, &ts);
    buf[i] = ts;
  }
  for (i = 0; i < nrecord; i++) {
    printf("%d\t%ld\t%d\n", id, diff_nsec(start, buf[i]) / NSECS_PER_MSEC,
           (i + 1) * 100 / nrecord);
  }
  exit(EXIT_SUCCESS);
}

int main(const int argc, char *argv[]) {
  int ret = EXIT_FAILURE;

  // Step 1: Check params
  if (argc < 4) {
    fprintf(stderr, "usage: %s <nproc> <total[ms]> <resolution[ms]>\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }
  const int nproc = atoi(argv[1]);
  const int total = atoi(argv[2]);
  const int resol = atoi(argv[3]);
  if (nproc < 1) {
    fprintf(stderr, "<nproc>(%d) should be >= 1\n", nproc);
    exit(EXIT_FAILURE);
  }
  if (total < 1) {
    fprintf(stderr, "<total>(%d) should be >= 1\n", total);
    exit(EXIT_FAILURE);
  }
  if (resol < 1) {
    fprintf(stderr, "<resol>(%d) should be >= 1\n", resol);
    exit(EXIT_FAILURE);
  }
  if (total % resol) {
    fprintf(stderr, "<total>(%d) should be multiple of <resolution>(%d)\n",
            total, resol);
    exit(EXIT_FAILURE);
  }
  const int nrecord = total / resol;

  // Step 2: estimate_loops_per_msec
  struct timespec *logbuf = malloc(nrecord * sizeof(struct timespec));
  if (!logbuf) err(EXIT_FAILURE, "failed to allocate log buffer");

  puts("estimating the workload which takes just one milli-second...");
  nloop_per_resol = estimate_loops_per_msec() * resol;
  puts("end estimation");
  fflush(stdout);

  // Step 3: Create pids
  pids = malloc(nproc * sizeof(pid_t));
  if (pids == NULL) err(EXIT_FAILURE, "failed to allocate pid table");

  // Step 4: Init clock
  clock_gettime(CLOCK_MONOTONIC, &start);

  // Step 5: create nproc's process and run
  ret = EXIT_SUCCESS;
  int i, ncreated;
  for (i = 0, ncreated = 0; i < nproc; i++, ncreated++) {
    pids[i] = fork();
    if (pids[i] < 0) {  // Step 5.1: Failed to fork enough process
      // Step 5.1.1: kill all and exit!
      for (int j = 0; j < ncreated; j++) {
        kill(pids[j], SIGKILL);
      }
      ret = EXIT_FAILURE;
      break;
    }
    if (pids[i] == 0) {  // Step 5.2: Fork success!
      // 子进程
      child_fn(i, logbuf, nrecord);
      /* 不应该运行到这里 */
      abort();
    }
  }
  // 父进程
  for (i = 0; i < ncreated; i++) {
    if (wait(NULL) < 0) warn("wait() failed.");
  }

  exit(ret);
}
