#!/usr/bin/python

import sys

if len(sys.argv) != 3:
    print("%s <data-dir> driver-enum -- Generate the enum list for the drivers" % sys.argv[0])
    print("%s <data-dir> shape-enum  -- Generate the enum list for the shapes" % sys.argv[0])
    print("%s <data-dir> driver-inv  -- Generate the inversion-or-not array for the drivers" % sys.argv[0])
    sys.exit(0)

driverlist = []
driverinv = []
shapelist = []

for l in open(sys.argv[1] + '/driver-types.txt'):
    ls = l.rstrip('\r\n').split()
    driverlist.append(ls[0])
    driverinv.append(ls[1] == 'i')
    if ls[2] not in shapelist:
        shapelist.append(ls[2])

shapelist.sort()

if sys.argv[2] == 'driver-enum':
    for e in driverlist:
        print("P(%s)," % e)
elif sys.argv[2] == 'shape-enum':
    for e in shapelist:
        print("P(%s)," % e)
elif sys.argv[2] == 'driver-inv':
    for e in driverinv:
        print("%s," % ('true' if e else 'false'))
