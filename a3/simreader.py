import sys
import re

count = 0
w=0.0
ta=0.0

# Regexp are used to find a match and collect the data needed for the graphs.
for line in sys.stdin:
    m = re.search('EXIT w=(\d+\.\d+) ta=(\d+\.\d+)', line)
    if m:
        count += 1
        w += float(m.group(1))
        ta += float(m.group(2))

# Simulations with the same quantum number are grouped on the
# same row and seperated by a single space. Once the last 
# simulation of the group with dispatch 25 is encountered it is
# appeneded with a newline character.
postfix = " "
if sys.argv[1]=="25":
    postfix = "\n"

print(count)
print(w)
f = open("./stat/w.txt","a+")
f.write("%f%s"%(w/count,postfix))
f.close()

print(ta)
f = open("./stat/ta.txt","a+")
f.write("%f%s"%(ta/count,postfix))
f.close()