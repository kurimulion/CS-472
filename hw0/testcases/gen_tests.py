import sys
import random

if len(sys.argv) != 3:
    print("usage: $python gen_tests.py <testcase index> <num_nodes>")
    sys.exit()

n = int(sys.argv[2])

f = open("test" + sys.argv[1] + ".txt", "w")
f.write(str(n) + "\n")

for i in range(random.randint(int(n/2), n*2) + 1): # at least one line
    for j in range(random.randint(2, n)): # at least 2 entries
        f.write(str(random.randint(0, n-1)) + " ")
    f.write("\n")
f.close()