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

static pid_t *pids;

static long diff_nsec(const struct timespec before,
                      const struct timespec after) {
  return after.tv_sec * NSECS_PER_SEC + after.tv_nsec -
         (before.tv_sec * NSECS_PER_SEC + before.tv_nsec);
}

static unsigned long loops_per_msec() {
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

static void load(unsigned long nloop) {
  for (unsigned long i = 0; i < nloop; i++) {
  }
}

static void child_fn(const int id, struct timespec *buf, const int nrecord,
                     unsigned long nloop_per_resol, struct timespec start) {
  int i;
  for (i = 0; i < nrecord; i++) {
    struct timespec ts;

    load(nloop_per_resol);  // Costing compute simulation
    clock_gettime(CLOCK_MONOTONIC, &ts);
    buf[i] = ts;
  }
  for (i = 0; i < nrecord; i++) {
    printf("%d\t%ld\t%d\n", id, diff_nsec(start, buf[i]) / NSECS_PER_MSEC,
           (i + 1) * 100 / nrecord);
  }
  exit(EXIT_SUCCESS);
}

static void parent_fn(int nproc) {
  for (int i = 0; i < nproc; i++) {
    wait(NULL);
  }
}

int main(void) {
  int ret = EXIT_FAILURE;

  // Step 1: Check params
  if (argc < 3) {
    fprintf(stderr, "usage: %s <total[ms]> <resolution[ms]>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  const int nproc = 2;
  const int total = atoi(argv[1]);
  const int resol = atoi(argv[2]);
  if (total < 1) {
    fprintf(stderr, "<total>(%d) should be >= 1\n", total);
    exit(EXIT_FAILURE);
  }
  if (resol < 1) {
    fprintf(stderr, "<resol>(%d) should be >= １\n", resol);
    exit(EXIT_FAILURE);
  }
  if (total % resol) {
    fprintf(stderr, "<total>(%d) should be multiple of <resolution>(%d)\n",
            total, resol);
    exit(EXIT_FAILURE);
  }
  int nrecord = total / resol;

  // Step 2: estimate_loops_per_msec
  struct timespec *logbuf = malloc(nrecord * sizeof(struct timespec));
  if (!logbuf) err(EXIT_FAILURE, "failed to allocate log buffer");
  unsigned long nloop_per_resol = loops_per_msec() * resol;

  // Step 3: Create pids
  pids = malloc(nproc * sizeof(pid_t));
  if (pids == NULL) {
    warn("malloc(pids) failed");
    goto free_logbuf;
  }

  // Step 4: Init clock
  struct timespec start;
  clock_gettime(CLOCK_MONOTONIC, &start);

  // Step 5: create nproc's process and run
  ret = EXIT_SUCCESS;
  int i, ncreated;
  for (i = 0, ncreated = 0; i < nproc; i++, ncreated++) {
    pids[i] = fork();
    if (pids[i] < 0) {  // Step 5.1: Failed to fork enough process
      goto wait_children;
    }
    if (pids[i] == 0) {  // Step 5.2: Fork success!
      if (i == 1) nice(5);
      // 子进程
      child_fn(i, logbuf, nrecord, nloop_per_resol, start);
      /* 不应该运行到这里 */
    }
  }
  ret = EXIT_SUCCESS;

  // 父进程
wait_children:
  if (ret == EXIT_FAILURE)
    for (i = 0; i < ncreated; i++)
      if (kill(pids[i], SIGINT) < 0) warn("kill(%d) failed", pids[i]);

  for (i = 0; i < ncreated; i++)
    if (wait(NULL) < 0) warn("wait() failed.");

free_pids:
  free(pids);

free_logbuf:
  free(logbuf);

  exit(ret);
}
