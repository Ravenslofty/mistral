#!/usr/bin/python

import sys

if len(sys.argv) != 4:
    print("Usage\n%s block-mux.txt[,block-mux.txt] block-cfg.txt block-doc.txt" % sys.argv[0])
    sys.exit(0)


lf = sys.argv[1].split(',')
muxes = []
for file in lf:
    for l in open(file):
        ls = l.rstrip('\n\r').split()
        if len(ls) == 0:
            continue
        sub = l[0] == ' '
        if sub:
            assert(len(muxes) > 0)
            if ls[0] != '*':
                if len(ls) == 3 and ls[2] == '!':
                    muxes[-1]['def'] = len(muxes[-1]['val'])
                if muxes[-1]['stype'] == 'm':
                    muxes[-1]['val'].append(ls[1])
                elif muxes[-1]['stype'] == 'n':
                    muxes[-1]['val'].append(int(ls[1]))
                else:
                    print("Mux %s of %s is not of stype m or n (%s)" % (muxes[-1]['mux'], mtype, muxes[-1]['stype']), file=sys.stderr)
                    sys.exit(1)
                
        else:
            subtype = ls[0]
            mux = ls[1]
            ss = ls[2].split(':')
            bcount = 1 if len(ss) < 2 else int(ss[1])
            stype = ss[0]
            d = None
            if len(stype) > 1:
                d = stype[1:]
                stype = stype[0]
            if d != None and (stype == 'r' or stype == 'b'):
                if d == '-':
                    d = 0
                elif d == '+':
                    d = -1 if stype == 'r' else 1
                elif stype == 'r':
                    d = int(d, 16)
                else:
                    print("Incorrect default suffix %s on mux %s of %s" % (d, mux, mtype), file=sys.stderr)
                    sys.exit(1)
            assert(stype in 'bmrn')
            nm = { 'subtype':subtype, 'mux':mux, 'stype':stype, 'bcount':bcount }
            if d != None:
                nm['def'] = d
            if stype == 'm' or stype == 'n':
                nm['val'] = []
            muxes.append(nm)

cfg_drop = []
cfg_map = {}
id = 0
for l in open(sys.argv[2]):
    ls = l.rstrip('\n\r').split()
    if len(ls) == 0:
        continue
    if ls[1] == '-':
        cfg_drop.append(ls[0])
    else:
        cfg_map[ls[0]] = [id, int(ls[1])]
        id += 1

docs = {}
for l in open(sys.argv[3]):
    ls = l.rstrip('\n\r').split(maxsplit=1)
    if len(ls) == 0:
        continue
    docs[ls[0]] = ls[1] if len(ls) >= 2 else ""

types = {
    'm': "Mux",
    'n': "Num",
    'r': "Ram",
    'b': "Bool"
    }

has_inst = False

entries = []
head = ["", "Name", "Instance", "Type", "Values", "Default", "Documentation" ]
for m in muxes:
    st = m['subtype']
    if st in cfg_drop:
        continue
    if st not in cfg_map:
        print("Missing subtype %s" % st, file = sys.stderr)
        continue
    irange = "0-%d" % (cfg_map[st][1]-1) if (st in cfg_map) and (cfg_map[st][1] != 1) else ""
    if irange != '':
        has_inst = True
    mt = m['stype']
    tp = types[mt]
    mm = m['mux']
    doc = None
    if mm not in docs:
        print("Missing mux %s" % mm, file = sys.stderr)
        continue
    doc = "TODO" if docs[mm] == "" else docs[mm]
    if doc == '-':
        continue
    vals = None
    df = ""
    if mt == 'm':
        vals = m['val']
        if 'def' in m:
            df = m['val'][m['def']]
    elif mt == 'n':
        vals = []
        s = None
        e = None
        for vv in sorted(m['val']):
            if e == None or vv != e+1:
                if s != None:
                    if s != e:
                        vals.append("%d-%d" % (s, e))
                    else:
                        vals.append("%d" % s)
                s = vv
                e = vv
            else:
                e = vv
        if s != e:
            vals.append("%d-%d" % (s, e))
        else:
            vals.append("%d" % s)
        if 'def' in m:
            df = "%d" % m['val'][m['def']]
    elif mt == 'b':
        vals = 't/f'
        if 'def' in m:
            df = 't' if m['def'] == 1 else 'f'
    else: # r
        bc = m['bcount']
        nd = (bc+3) // 4
        if bc <= 16:
            vals = '%0*x-%0*x' % (nd, 0, nd, (1 << bc)-1)
        else:
            vals = '%d bits' % bc
        if 'def' in m:
            df = '0' if m['def'] == 0 else '%0*x' % (nd, (1 << bc)-1 if m['def'] == -1 else m['def'])
                
    entries.append([st, mm.upper(), irange, tp, vals, df, doc])

entries.sort(key = lambda e: [cfg_map[e[0]], e[1]])

csize = []
for e in head:
    csize.append(len(e))
for e in entries:
    if type(e[4]) != str:
        for i in range(len(head)):
            if i != 4:
                csize[i] = max(csize[i], len(e[i]))
        for j in e[4]:
            csize[4] = max(csize[4], len(j))
    else:
        for i in range(len(head)):
            csize[i] = max(csize[i], len(e[i]))

csize[4] += 2

sep = '+'
seph = '+'
for i in range(1, len(head)):
    if i != 2 or has_inst:
        sep += '-' * (csize[i] + 2)
        seph += '=' * (csize[i] + 2)
        sep += '+'
        seph += '+'

print(sep)
s = '|'
for i in range(1, len(head)):
    if i != 2 or has_inst:
        s += ' %*s |' % (csize[i], head[i])
print(s)
print(seph)
for e in entries:
    if type(e[4]) != str:
        for j in range(len(e[4])):
            s = '|'
            for i in range(1, len(head)):
                if i == 4:
                    s += ' - %-*s |' % (csize[i]-2, e[i][j])
                elif i != 2 or has_inst:
                    s += ' %*s |' % (csize[i], e[i] if j == 0 else "")
            print(s)
    else:
        s = '|'
        for i in range(1, len(head)):
            if i != 2 or has_inst:
                s += ' %*s |' % (csize[i], e[i])
        print(s)
    print(sep)

