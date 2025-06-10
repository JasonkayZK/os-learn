#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define BUFFER_SIZE 1000
#define ALLOC_SIZE (100 * 1024 * 1024)

static char command[BUFFER_SIZE];

int main(void) {
  const pid_t pid = getpid();
  snprintf(command, BUFFER_SIZE, "cat /proc/%d/maps", pid);
  // Step 1: print current memory
  /*
    这段代码运用了 snprintf 函数来生成一条用于查看进程内存映射的 Linux
    命令。下面为你详细解读：

    函数功能：snprintf 会把格式化后的字符串写入指定的字符数组。
    参数剖析：
    command 是用来存储生成字符串的字符数组。
    BUFFER_SIZE 代表该数组的大小，其作用是防止出现缓冲区溢出的情况。
    "cat /proc/%d/maps" 是格式化字符串，其中的 %d 是一个占位符。
    pid 是要查看的进程 ID，它会替换掉格式化字符串里的 %d。
    实际效果：要是 pid 的值为 1234，那么最终生成的字符串就是 cat
    /proc/1234/maps。

    在 Linux 系统里，/proc/[pid]/maps 文件记录着进程的内存映射信息，
    像各个内存区域的起始地址、权限以及对应的文件等内容都包含在内。
    借助这个命令，我们能够查看特定进程的内存使用状况。
  */
  puts("*** memory map before memory allocation ***");
  fflush(stdout);
  system(command);

  // Step 2: Relocate memory
  void *new_memory = mmap(NULL, ALLOC_SIZE, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (new_memory == (void *)-1) {
    err(EXIT_FAILURE, "mmap() failed");
  }
  puts("");
  printf("*** succeeded to allocate memory: address = %p; size = 0x%x ***\n",
         new_memory, ALLOC_SIZE);
  puts("");

  // Step 3: Print new memory
  puts("*** memory map after memory allocation ***");
  fflush(stdout);
  system(command);

  if (munmap(new_memory, ALLOC_SIZE) == -1) {
    err(EXIT_FAILURE, "munmap() failed");
  }

  exit(EXIT_SUCCESS);
}
