# 4-A
taskset -c 0 ./sched 1 100 1

# 4-B
taskset -c 0 ./sched 2 100 1

# 4-C
taskset -c 0 ./sched 4 100 1

# 4-D
taskset -c 0,2 ./sched 1 100 1

# 4-E
taskset -c 0,2 ./sched 2 100 1

# 4-F
taskset -c 0,2 ./sched 4 100 1

# 4.17
time taskset -c 0 ./sched 1 10000 10000

time taskset -c 0,2 ./sched 1 10000 10000

time taskset -c 0,2 ./sched 4 10000 10000
