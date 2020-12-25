#!/usr/bin/python

import sys

if len(sys.argv) != 3:
    print("%s <data-dir> chip         -- Generate the data source for the die" % sys.argv[0])
    sys.exit(0)

print("// Generated file, edition is futile")
print("")
print("#include \"cyclonev.h\"")
print("")
print("const mistral::CycloneV::inverter_info mistral::CycloneV::%s_inverters_info[] = {" % sys.argv[2])

for l in open(sys.argv[1] + '/' + sys.argv[2] + '-inv.txt'):
    ls = l.rstrip('\r\n').split()
    nn = ls[0].split('.')
    xy = ls[1].split('.')
    print("  { rnode(%s, %d, %d, %d), %d, %d, %d }," % (nn[0], int(nn[1]), int(nn[2]), int(nn[3]), int(xy[0]), int(xy[1]), int(ls[2])))
print("};")
