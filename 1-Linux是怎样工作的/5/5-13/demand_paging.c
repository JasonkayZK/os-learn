//
// Use `sar -r 1` to test!
//

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE (100 * 1024 * 1024)
#define NCYCLE 10
#define PAGE_SIZE 4096

int main() {
  time_t t;

  // Step 1: Print before allocation memory map
  t = time(NULL);
  char *s = ctime(&t);
  printf("%.*s: before allocation, please press Enter key\n",
         (int)(strlen(s) - 1), s);
  getchar();

  // Step 2: Allocate memory(For vitual, not physical!)
  char *p = malloc(BUFFER_SIZE);
  if (p == NULL) {
    err(EXIT_FAILURE, "malloc() failed");
  }

  // Step 3: Print memory after allocated(For vitual, not physical!)
  t = time(NULL);
  s = ctime(&t);
  printf("%.*s: allocated %dMB, please press Enter key\n", (int)(strlen(s) - 1),
         s, BUFFER_SIZE / (1024 * 1024));
  getchar();

  // Step 4: Acquire physical memory
  for (int i = 0; i < BUFFER_SIZE; i += PAGE_SIZE) {
    p[i] = 0;
    const int cycle = i / (BUFFER_SIZE / NCYCLE);
    if (cycle != 0 && i % (BUFFER_SIZE / NCYCLE) == 0) {
      t = time(NULL);
      s = ctime(&t);
      printf("%.*s: touched %dMB\n", (int)(strlen(s) - 1), s,
             i / (1024 * 1024));
      sleep(1);
    }
  }

  // Print touched memory map
  t = time(NULL);
  s = ctime(&t);
  printf("%.*s: touched %dMB, please press Enter key\n", (int)(strlen(s) - 1),
         s, BUFFER_SIZE / (1024 * 1024));
  getchar();

  exit(EXIT_SUCCESS);
}
