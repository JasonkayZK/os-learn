# Create
dd if=/dev/zero of=testfile oflag=direct bs=1M count=1K

# First read
free

time cat testfile >/dev/null

free

#               total        used        free      shared  buff/cache   available
#Mem:         3762472     1583640      492524       14796     1686308     1905332
#Swap:              0           0           0
#
#real    0m3.633s
#user    0m0.000s
#sys     0m0.398s
#               total        used        free      shared  buff/cache   available
#Mem:         3762472     1579724      111380       14796     2071368     1911460
#Swap:              0           0           0

# Second read

time cat testfile >/dev/null

free

#real    0m2.802s
#user    0m0.000s
#sys     0m0.410s
#               total        used        free      shared  buff/cache   available
#Mem:         3762472     1581344      118284       14796     2062844     1913528
#Swap:              0           0           0
