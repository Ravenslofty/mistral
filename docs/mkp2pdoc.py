#!/usr/bin/python

import sys

if len(sys.argv) != 4:
    print("Usage\n%s block %s-p2p.txt p2p-doc.txt" % sys.argv[0])
    sys.exit(0)

def pnodes(n):
    nn = n.split(":")
    p1 = nn[0].split(".")
    p2 = nn[1].split(".")
    return { 'b':p1[0], 'x':int(p1[1]), 'y':int(p1[2]), 'i':(-1 if len(p1) < 4 else int(p1[3])), 'p':p2[0], 'ip':(-1 if len(p2) < 2 else int(p2[1])) }


block = sys.argv[1].upper()

port = {}
for die in ['e50f', 'gx25f', 'gt75f', 'gt150f', 'gt300f', 'sx50f', 'sx120f']:
    for l in open(sys.argv[2] % die):
        ls = l.rstrip('\n\r').split()
        s = pnodes(ls[0])
        d = pnodes(ls[1])
        if s['b'] != block and d['b'] != block:
            continue
        if s['b'] == block:
            n1 = s
            n2 = d
            s2d = True
        else:
            n1 = d
            n2 = s
            s2d = False
    
        a = None
        if n1['p'] not in port:
            a = {}
            port[n1['p']] = a
        else:
            a = port[n1['p']]
        dkey = n2['b'] + ':' + n2['p'] if n2['p'] != 'PNONE' else n2['b']
        if dkey not in a:
            a[dkey] = [[n1['i']], [n1['ip']], s2d]
        else:
            if n1['i'] not in a[dkey][0]:
                a[dkey][0].append(n1['i'])
            if n1['ip'] not in a[dkey][1]:
                a[dkey][1].append(n1['ip'])

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
head = ["Port Name", "Instance", "Port bits", "Dir", "Remote port", "Documentation" ]
for p,g in port.items():
    for gg,gl in g.items():

        binst = mkinst(gl[0])
        pinst = mkinst(gl[1])
        dd = '>' if gl[2] else '<'
        key = block + '.' + p
        doc = 'TODO' if key not in docs else docs[key]
        entries.append(["" if p == 'PNONE' else p, binst, pinst, dd, gg, doc])

entries.sort(key = lambda e: [e[0], e[3], e[4]])

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

