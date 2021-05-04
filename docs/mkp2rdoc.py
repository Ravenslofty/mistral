#!/usr/bin/python

import sys

if len(sys.argv) != 4:
    print("Usage\n%s block sx120f-p2r.txt p2r-doc.txt" % sys.argv[0])
    sys.exit(0)

def pnodes(n):
    nn = n.split(":")
    p1 = nn[0].split(".")
    p2 = nn[1].split(".")
    return { 'b':p1[0], 'x':int(p1[1]), 'y':int(p1[2]), 'i':(-1 if len(p1) < 4 else int(p1[3])), 'p':p2[0], 'ip':(-1 if len(p2) < 2 else int(p2[1])) }

def rnodes(n):
    nn = n.split('.')
    return { 't':nn[0], 'x':int(nn[1]), 'y':int(nn[2]), 'z':int(nn[3]) }


block = sys.argv[1].upper()

port = {}
for l in open(sys.argv[2]):
    ls = l.rstrip('\n\r').split()
    p = pnodes(ls[0])
    if p['b'] != block:
        continue
    r = rnodes(ls[1])
    a = None
    if p['p'] not in port:
        a = {}
        port[p['p']] = a
    else:
        a = port[p['p']]
    if r['t'] not in a:
        a[r['t']] = [[p['i']], [p['ip']]]
    else:
        if p['i'] not in a[r['t']][0]:
            a[r['t']][0].append(p['i'])
        if p['ip'] not in a[r['t']][1]:
            a[r['t']][1].append(p['ip'])

docs = {}
for l in open(sys.argv[3]):
    ls = l.rstrip('\n\r').split(maxsplit=1)
    if len(ls) == 0:
        continue
    docs[ls[0].upper()] = ls[1] if len(ls) >= 2 else ""

def mkinst(gl):
    vals = ''
    if len(gl) != 1 or gl[0] != -1:
        s = None
        e = None
        for vv in sorted(gl):
            assert(vv != -1)
            if e == None or vv != e+1:
                if s != None:
                    if vals != '':
                        vals += ', '
                    if s != e:
                        vals += "%d-%d" % (s, e)
                    else:
                        vals += "%d" % s
                s = vv
                e = vv
            else:
                e = vv
        if vals != '':
            vals += ', '
        if s != e:
            vals += "%d-%d" % (s, e)
        else:
            vals += "%d" % s
    return vals

entries = []
head = ["Port Name", "Instance", "Port bits", "Route node type", "Documentation" ]
for p,g in port.items():
    for gg,gl in g.items():

        binst = mkinst(gl[0])
        pinst = mkinst(gl[1])

        key = block + '.' + p
        doc = 'TODO' if key not in docs else docs[key]
        entries.append([p, binst, pinst, gg, doc])

entries.sort(key = lambda e: [e[0], e[3]])

csize = []
for e in head:
    csize.append(len(e))
for e in entries:
    for i in range(len(head)):
        csize[i] = max(csize[i], len(e[i]))

sep = '+'
seph = '+'
for i in range(len(head)):
    sep += '-' * (csize[i] + 2)
    seph += '=' * (csize[i] + 2)
    sep += '+'
    seph += '+'

print(sep)
s = '|'
for i in range(len(head)):
    s += ' %*s |' % (csize[i], head[i])
print(s)
print(seph)
for e in entries:
    s = '|'
    for i in range(len(head)):
        s += ' %*s |' % (csize[i], e[i])
    print(s)
    print(sep)

