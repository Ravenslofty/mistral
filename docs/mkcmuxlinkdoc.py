#!/usr/bin/python

import sys

if len(sys.argv) != 3:
    print("Usage\n%s srcdoc/cmux-links.txt block" % sys.argv[0])
    sys.exit(0)

def mkarray(n, e):
    res = []
    for i in range(n):
        res.append([None]*e)
    return res

linkarrays = {
    'CMUXHG': mkarray(4, 64),
    'CMUXVG': mkarray(4, 64),
    'CMUXCR': mkarray(6, 16),
    'CMUXHR': mkarray(12, 32),
    'CMUXVR': mkarray(20, 16)
    }

mw = 0
for l in open(sys.argv[1]):    
    ls = l.rstrip('\n\r').split()
    v = ls[3] + '.' + ls[4]
    if len(v) > mw:
        mw = len(v)
    linkarrays[ls[0]][int(ls[1])][int(ls[2])] = v

t = linkarrays[sys.argv[2].upper()]
nc = len(t)
ne = len(t[0])

sep = '+--------+'
for c in range(nc):
    sep += '-%s-+' % ('-' * mw)

print(sep)
l = '| %s |' % sys.argv[2].lower()
for c in range(nc):
    l += ' %*s |' % (mw, "%2d" % c)
print(l)
l = '+========+'
for c in range(nc):
    l += '=%s=+' % ('=' * mw)
print(l)
for e in range(ne):
    if t[nc-1][e] == 'OFF.0':
        continue
    l = '|   %02x   |' % e
    for c in range(nc):
        l += ' %-*s |' % (mw, t[c][e])
    print(l)
    print(sep)
