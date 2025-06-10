#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1000
#define ALLOC_SIZE (100 * 1024 * 1024)

static char command[BUFFER_SIZE];
static char overwrite_data[] = "HELLO";

int main(void) {
  const pid_t pid = getpid();
  snprintf(command, BUFFER_SIZE, "cat /proc/%d/maps", pid);

  // Step 1: Print origin memory map
  puts("*** memory map before mapping file ***");
  fflush(stdout);
  system(command);

  // Step 2: Open file
  const int fd = open("testfile", O_RDWR);
  if (fd == -1) {
    err(EXIT_FAILURE, "open() failed");
  }

  // Step 3: Read file content
  char *file_contents =
      mmap(NULL, ALLOC_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (file_contents == (void *)-1) {
    warn("mmap() failed");
    goto close_file;
  }
  puts("");
  printf("*** succeeded to map file: address = %p; size = 0x%x ***\n",
         file_contents, ALLOC_SIZE);

  // Step 4: Print new memory map
  puts("");
  puts("*** memory map after mapping file ***");
  fflush(stdout);
  system(command);
  puts("");
  printf("*** file contents before overwrite mapped region: %s", file_contents);

  // Step 5: Write file content
  memcpy(file_contents, overwrite_data, strlen(overwrite_data));
  puts("");
  printf("*** overwritten mapped region with: %s\n", file_contents);

  // Step 6: Print map
  // munmap是一个系统调用函数，其作用是撤销之前通过mmap建立的内存映射。
  // munmap函数的主要功能是：
  // - 切断进程虚拟地址空间与物理内存或者文件之间的映射联系。
  // - 把映射所占用的虚拟地址空间释放出来。
  // -
  // 要是映射的内容被修改过，并且设置了MAP_SHARED标志，系统会把这些修改写回到对应的文件中。
  if (munmap(file_contents, ALLOC_SIZE) == -1) {
    warn("munmap() failed");
  }

close_file:
  if (close(fd) == -1) warn("close() failed");
  exit(EXIT_SUCCESS);
}
