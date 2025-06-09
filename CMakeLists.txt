cmake_minimum_required(VERSION 3.31)
project(os_learn C)

set(CMAKE_C_STANDARD 11)

add_executable(2-1-hello 1-Linux是怎样工作的/2/2-1/hello.c)
add_executable(2-4-ppidloop 1-Linux是怎样工作的/2/2-4/ppidloop.c)
add_executable(3-2-fork 1-Linux是怎样工作的/3/3-2/fork.c)
add_executable(3-3-fork_and_exec 1-Linux是怎样工作的/3/3-3/fork_and_exec.c)
add_executable(4-2-sched 1-Linux是怎样工作的/4/4-2/sched.c)
add_executable(4-20-sched_nice 1-Linux是怎样工作的/4/4-20/sched_nice.c)
