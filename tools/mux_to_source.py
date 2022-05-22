#!/usr/bin/python

import sys

if len(sys.argv) != 3 or (sys.argv[1] != 'enum' and sys.argv[1] != 'mux'):
    print("%s enum <data-dir>          -- Generate the enumeration header" % sys.argv[0])
    print("%s mux  <data-dir>          -- Generate the muxes source" % sys.argv[0])
    sys.exit(0)

def load_mux_data(mtype, tmap, is_2d = False, variantmap={}):
    w = (lambda v: [int(v.split('.')[0]), int(v.split('.')[1])]) if is_2d else (lambda v: int(v))
    muxes = []
    fname = sys.argv[2] + '/' + mtype + '-mux.txt'
    for l in open(fname):
        ls = l.rstrip('\n\r').split()
        if len(ls) == 0:
            continue
        sub = l[0] == ' '
        if sub:
            assert(len(muxes) > 0)
            if ls[0] == '*':
                bl = []
                for b in range(muxes[-1]['bcount']):
                    bl.append(w(ls[b+1]))
                muxes[-1]['bits'].append(bl)
            else:
                if len(ls) == 3 and ls[2] == '!':
                    muxes[-1]['def'] = len(muxes[-1]['val'])
                if muxes[-1]['stype'] == 'm':
                    muxes[-1]['val'].append([int(ls[0], 16), ls[1]])
                elif muxes[-1]['stype'] == 'n':
                    muxes[-1]['val'].append([int(ls[0], 16), int(ls[1])])
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
            if subtype not in tmap:
                print("Missing subtype %s on mux %s of %s" % (subtype, mux, mtype), file=sys.stderr)
                sys.exit(1)
            span = tmap[subtype]
            bits = []
            nm = { 'subtype':subtype, 'mux':mux, 'stype':stype, 'bcount':bcount, 'span':span, 'bits':bits, 'variant':(variantmap[subtype] if subtype in variantmap else -1) }
            if d != None:
                nm['def'] = d
            if stype == 'm' or stype == 'n':
                nm['val'] = []
            if len(ls) != 3:
                if len(ls) != 3 + (span-1) + span*bcount:
                    print("Incorrect number of entries on mux %s of %s, span = %d, bits = %d" % (mux, mtype, span, bcount), file=sys.stderr)
                    sys.exit(1);
                for s in range(span):
                    bl = []
                    for b in range(bcount):
                        bl.append(w(ls[3+b + (bcount+1)*s]))
                    bits.append(bl)
            muxes.append(nm)

    for nm in muxes:
        if len(nm['bits']) != nm['span']:
            print("Incorrect span count %d vs. data %d on mux %s of %s" % (len(nm['bits']), nm['span'], nm['mux'], mtype), file=sys.stderr)
            sys.exit(1)

    return muxes

def lab_expand(muxes, is_mlab):
    for nm in muxes:
        if nm['subtype'] == 'i':
            bb = nm['bits'][0]
            sp = is_mlab and len(bb) == 64
            nb = []
            for i in range(4 if sp else 5):
                base = 85 - 8*i
                bl = []
                for b in bb:
                    bl.append([b[0], base - b[1]])
                nb.append(bl)
            if sp:
                bl = []
                for b in bb[:32]:
                    bl.append([b[0], 53])
                for b in bb[32:]:
                    bl.append([b[0], 47])
                nb.append(bl)
            for i in range(4 if sp else 5):
                base = 8*i
                bl = []
                for b in bb:
                    bl.append([b[0], base + b[1]])
                nb.append(bl)
            if sp:
                bl = []
                for b in bb[:32]:
                    bl.append([b[0], 32])
                for b in bb[32:]:
                    bl.append([b[0], 38])
                nb.append(bl)
            nm['span'] = 10
            nm['bits'] = nb

def enum_add(l, muxes):
    for nm in muxes:
        if nm['mux'] not in l:
            l.append(nm['mux'])
        if nm['stype'] == 'm':
            for s in nm['val']:
                if s[1] not in l:
                    l.append(s[1])

def sort_muxes(muxes):
    muxes.sort(key = lambda nm : nm['mux'])

def collate_oram_bpos(l, muxes, filter = lambda t: True):
    for nm in muxes:
        if filter(nm['subtype']):
            for s in nm['bits']:
                for b in s:
                    l.append(b[0])
                    l.append(b[1])

def collate_pram_bpos(l, muxes, filter = lambda t: True):
    for nm in muxes:
        if filter(nm['subtype']):
            for s in nm['bits']:
                for b in s:
                    l.append(b)

def collate_cram_bpos(l, muxes, filter = lambda t: True):
    for nm in muxes:
        if filter(nm['subtype']):
            for s in nm['bits']:
                for b in s:
                    l.append(b[0])
                    l.append(b[1])

def collate_sel_entries(l, muxes, filter = lambda t: True):
    for nm in muxes:
        if filter(nm['subtype']):
            if nm['stype'] == 'm':
                for s in nm['val']:
                    l.append([s[0], s[1], nm['bcount']])

def collate_num_entries(l, muxes, filter = lambda t: True):
    for nm in muxes:
        if filter(nm['subtype']):
            if nm['stype'] == 'n':
                for s in nm['val']:
                    l.append([s[0], s[1], nm['bcount']])

def dump_bpos(name, bpos):
    print("const uint16_t mistral::CycloneV::%s[] = {" % name)
    s = ''
    for i in range(len(bpos)):
        if s == '':
            s = '  '
        else:
            s += ' '
        s += '%d,' % bpos[i]
        if (i & 31) == 31:
            print(s)
            s = ''
    if s != '':
        print(s)
    print("};")

def dump_sel_entries(entries):
    print("const mistral::CycloneV::bmux_sel_entry mistral::CycloneV::bmux_sel_entries[] = {")
    for e in entries:
        print("  { 0x%0*x, %s }," % ((e[2]+3) // 4, e[0], e[1].upper()))
    print("};")

def dump_num_entries(entries):
    print("const mistral::CycloneV::bmux_num_entry mistral::CycloneV::bmux_num_entries[] = {")
    for e in entries:
        print("  { 0x%0*x, %d }," % ((e[2]+3) // 4, e[0], e[1]))
    print("};")

def dump_muxes(name, muxes, bci, counters, filter = lambda t: True):
    print("const mistral::CycloneV::bmux mistral::CycloneV::bm_%s[] = {" % name)
    for m in muxes:
        if filter(m['subtype']):
            if m['stype'] == 'm':
                print("  { %s, %d, %d, MT_MUX,  %d, %d, %d, %d, %d }," % (m['mux'].upper(), m['span'], m['bcount'], len(m['val']), -2 if 'def' not in m else m['def'], counters[bci], counters[3], m['variant']))
                counters[bci] += m['bcount'] * m['span']
                counters[3] += len(m['val'])
            elif m['stype'] == 'n':
                print("  { %s, %d, %d, MT_NUM,  %d, %d, %d, %d, %d }," % (m['mux'].upper(), m['span'], m['bcount'], len(m['val']), -2 if 'def' not in m else m['def'], counters[bci], counters[4], m['variant']))
                counters[bci] += m['bcount'] * m['span']
                counters[4] += len(m['val'])
            elif m['stype'] == 'b':
                print("  { %s, %d, %d, MT_BOOL, 0, %d, %d, 0, %d }," % (m['mux'].upper(), m['span'], m['bcount'], -2 if 'def' not in m else m['def'], counters[bci], m['variant']))
                counters[bci] += m['bcount'] * m['span']
            else:
                print("  { %s, %d, %d, MT_RAM,  0, %d, %d, 0, %d }," % (m['mux'].upper(), m['span'], m['bcount'], -2 if 'def' not in m else m['def'], counters[bci], m['variant']))
                counters[bci] += m['bcount'] * m['span']
    print("  { BMNONE }")
    print("};")

opt        = load_mux_data("opt",        { 'g': 1 }, True)
gpio       = load_mux_data("gpio",       { 'i': 4, 'x':4, 'X':4, 'g': 1, 'l': 2, 's': 1 }, False, { 'x':1, 'X': 0, 's': 1 })
dqs16      = load_mux_data("dqs16",      { 'g': 1, 'i': 16 })
fpll       = load_mux_data("fpll",       { 'g': 1, 'e': 2, 'c': 9, 'l': 2 })
lab        = load_mux_data("lab",        { 'g': 1, 'i': 1 }, True )
mlab       = load_mux_data("mlab",       { 'g': 1, 'i': 1 }, True )
m10k       = load_mux_data("m10k",       { 'g': 1, 'm':256 }, True)
dsp        = load_mux_data("dsp",        { 'g': 1, 'c': 8, 'd': 12 }, True)
hps_clocks = load_mux_data("hps_clocks", { 'i': 9 }, True)
cmuxc      = load_mux_data("cmuxc",      { 'r': 6, 'R': 1, 'Y': 4 })
cmuxv      = load_mux_data("cmuxv",      { 'g': 4, 'r': 20, 'G': 1, 'R': 1 })
cmuxh      = load_mux_data("cmuxh",      { 'g': 4, 'r': 12, 'G': 1, 'R': 1, 'X': 4, 'Y': 8 })
dll        = load_mux_data("dll",        { 'g': 1 })
hssi       = load_mux_data("hssi",       { 'g': 1, 'i': 3 })
cbuf       = load_mux_data("cbuf",       { 'g': 1, 'i': 2 })
lvl        = load_mux_data("lvl",        { 'g': 1, 'i': 4 })
pma3       = load_mux_data("pma3",       { 'g': 1, 'i': 3, 'q': 4, 'k': 6, 'd': 2, 't': 1, 'b': 1 }, False, { 't': 1, 'b': 0})
pma3c      = load_mux_data("pma3-c",     { 'i': 3 })
serpar     = load_mux_data("serpar",     { 'g': 1 })
term       = load_mux_data("term",       { 'g': 1 })
hip        = load_mux_data("hip",        { 'g': 1, 'i': 8, 'm': 6 })
hmc        = load_mux_data("hmc",        { 'g': 1, 'i': 128, 'a': 40, 'p': 6, 'f': 4, 'o': 8 })

lab_expand(lab, False)
lab_expand(mlab, True)

if sys.argv[1] == 'enum':
    enum_list = []
    enum_add(enum_list, opt)

    enum_add(enum_list, gpio)
    enum_add(enum_list, dqs16)
    enum_add(enum_list, fpll)
    enum_add(enum_list, cmuxc)
    enum_add(enum_list, cmuxv)
    enum_add(enum_list, cmuxh)
    enum_add(enum_list, dll)
    enum_add(enum_list, hssi)
    enum_add(enum_list, cbuf)
    enum_add(enum_list, lvl)
    enum_add(enum_list, pma3)
    enum_add(enum_list, pma3c)
    enum_add(enum_list, serpar)
    enum_add(enum_list, term)
    enum_add(enum_list, hip)
    enum_add(enum_list, hmc)

    enum_add(enum_list, lab)
    enum_add(enum_list, mlab)
    enum_add(enum_list, m10k)
    enum_add(enum_list, dsp)
    enum_add(enum_list, hps_clocks)

    enum_list.sort()

    print("P(BMNONE),")
    for e in enum_list:
        print("P(%s)," % e.upper())

if sys.argv[1] == 'mux':
    sort_muxes(opt)

    sort_muxes(gpio)
    sort_muxes(dqs16)
    sort_muxes(fpll)
    sort_muxes(cmuxc)
    sort_muxes(cmuxv)
    sort_muxes(cmuxh)
    sort_muxes(dll)
    sort_muxes(hssi)
    sort_muxes(cbuf)
    sort_muxes(lvl)
    sort_muxes(pma3)
    sort_muxes(pma3c)
    sort_muxes(serpar)
    sort_muxes(term)
    sort_muxes(hip)
    sort_muxes(hmc)

    sort_muxes(lab)
    sort_muxes(mlab)
    sort_muxes(m10k)
    sort_muxes(dsp)
    sort_muxes(hps_clocks)

    bmux_oram_bpos = []
    collate_oram_bpos(bmux_oram_bpos, opt)

    bmux_pram_bpos = []
    collate_pram_bpos(bmux_pram_bpos, gpio)
    collate_pram_bpos(bmux_pram_bpos, dqs16)
    collate_pram_bpos(bmux_pram_bpos, fpll)
    collate_pram_bpos(bmux_pram_bpos, cmuxc)
    collate_pram_bpos(bmux_pram_bpos, cmuxv, lambda t: t == 'r' or t == 'R')
    collate_pram_bpos(bmux_pram_bpos, cmuxv, lambda t: t == 'g' or t == 'G')
    collate_pram_bpos(bmux_pram_bpos, cmuxh, lambda t: t == 'r' or t == 'R')
    collate_pram_bpos(bmux_pram_bpos, cmuxh, lambda t: t == 'g' or t == 'G')
    collate_pram_bpos(bmux_pram_bpos, dll)
    collate_pram_bpos(bmux_pram_bpos, hssi)
    collate_pram_bpos(bmux_pram_bpos, cbuf)
    collate_pram_bpos(bmux_pram_bpos, lvl)
    collate_pram_bpos(bmux_pram_bpos, pma3)
    collate_pram_bpos(bmux_pram_bpos, pma3c)
    collate_pram_bpos(bmux_pram_bpos, serpar)
    collate_pram_bpos(bmux_pram_bpos, term)
    collate_pram_bpos(bmux_pram_bpos, hip)
    collate_pram_bpos(bmux_pram_bpos, hmc)

    bmux_cram_bpos = []
    collate_cram_bpos(bmux_cram_bpos, lab)
    collate_cram_bpos(bmux_cram_bpos, mlab)
    collate_cram_bpos(bmux_cram_bpos, m10k)
    collate_cram_bpos(bmux_cram_bpos, dsp)
    collate_cram_bpos(bmux_cram_bpos, hps_clocks)
    
    bmux_sel_entries = []
    collate_sel_entries(bmux_sel_entries, opt)

    collate_sel_entries(bmux_sel_entries, gpio)
    collate_sel_entries(bmux_sel_entries, dqs16)
    collate_sel_entries(bmux_sel_entries, fpll)
    collate_sel_entries(bmux_sel_entries, cmuxc)
    collate_sel_entries(bmux_sel_entries, cmuxv, lambda t: t == 'r' or t == 'R')
    collate_sel_entries(bmux_sel_entries, cmuxv, lambda t: t == 'g' or t == 'G')
    collate_sel_entries(bmux_sel_entries, cmuxh, lambda t: t == 'r' or t == 'R')
    collate_sel_entries(bmux_sel_entries, cmuxh, lambda t: t == 'g' or t == 'G')
    collate_sel_entries(bmux_sel_entries, dll)
    collate_sel_entries(bmux_sel_entries, hssi)
    collate_sel_entries(bmux_sel_entries, cbuf)
    collate_sel_entries(bmux_sel_entries, lvl)
    collate_sel_entries(bmux_sel_entries, pma3)
    collate_sel_entries(bmux_sel_entries, pma3c)
    collate_sel_entries(bmux_sel_entries, serpar)
    collate_sel_entries(bmux_sel_entries, term)
    collate_sel_entries(bmux_sel_entries, hip)
    collate_sel_entries(bmux_sel_entries, hmc)

    collate_sel_entries(bmux_sel_entries, lab)
    collate_sel_entries(bmux_sel_entries, mlab)
    collate_sel_entries(bmux_sel_entries, m10k)
    collate_sel_entries(bmux_sel_entries, dsp)
    collate_sel_entries(bmux_sel_entries, hps_clocks)

    bmux_num_entries = []
    collate_num_entries(bmux_num_entries, opt)

    collate_num_entries(bmux_num_entries, gpio)
    collate_num_entries(bmux_num_entries, dqs16)
    collate_num_entries(bmux_num_entries, fpll)
    collate_num_entries(bmux_num_entries, cmuxc)
    collate_num_entries(bmux_num_entries, cmuxv, lambda t: t == 'r' or t == 'R')
    collate_num_entries(bmux_num_entries, cmuxv, lambda t: t == 'g' or t == 'G')
    collate_num_entries(bmux_num_entries, cmuxh, lambda t: t == 'r' or t == 'R')
    collate_num_entries(bmux_num_entries, cmuxh, lambda t: t == 'g' or t == 'G')
    collate_num_entries(bmux_num_entries, dll)
    collate_num_entries(bmux_num_entries, hssi)
    collate_num_entries(bmux_num_entries, cbuf)
    collate_num_entries(bmux_num_entries, lvl)
    collate_num_entries(bmux_num_entries, pma3)
    collate_num_entries(bmux_num_entries, pma3c)
    collate_num_entries(bmux_num_entries, serpar)
    collate_num_entries(bmux_num_entries, term)
    collate_num_entries(bmux_num_entries, hip)
    collate_num_entries(bmux_num_entries, hmc)

    collate_num_entries(bmux_num_entries, lab)
    collate_num_entries(bmux_num_entries, mlab)
    collate_num_entries(bmux_num_entries, m10k)
    collate_num_entries(bmux_num_entries, dsp)
    collate_num_entries(bmux_num_entries, hps_clocks)

    counters = [0, 0, 0, 0, 0]

    print("// Generated file, edition is futile")
    print("")
    print("#include \"cyclonev.h\"")
    print("")
    dump_bpos("bmux_oram_bpos", bmux_oram_bpos)
    print("")
    dump_bpos("bmux_pram_bpos", bmux_pram_bpos)
    print("")
    dump_bpos("bmux_cram_bpos", bmux_cram_bpos)
    print("")
    dump_sel_entries(bmux_sel_entries)
    print("")
    dump_num_entries(bmux_num_entries)

    print("")
    dump_muxes("opt",    opt,    0, counters)

    print("")
    dump_muxes("gpio",   gpio,   1, counters)
    print("")
    dump_muxes("dqs16",  dqs16,  1, counters)
    print("") 
    dump_muxes("fpll",   fpll,   1, counters)
    print("")
    dump_muxes("cmuxcr", cmuxc,  1, counters)
    print("")
    dump_muxes("cmuxvr", cmuxv,  1, counters, lambda t: t == 'r' or t == 'R')
    print("")
    dump_muxes("cmuxvg", cmuxv,  1, counters, lambda t: t == 'g' or t == 'G')
    print("")
    dump_muxes("cmuxhr", cmuxh,  1, counters, lambda t: t == 'r' or t == 'R')
    print("")
    dump_muxes("cmuxhg", cmuxh,  1, counters, lambda t: t == 'g' or t == 'G')
    print("")
    dump_muxes("dll",    dll,    1, counters)
    print("")
    dump_muxes("hssi",   hssi,   1, counters)
    print("")
    dump_muxes("cbuf",   cbuf,   1, counters)
    print("")
    dump_muxes("lvl",    lvl,    1, counters)
    print("")
    dump_muxes("pma3",   pma3,   1, counters)
    print("")
    dump_muxes("pma3c",  pma3c,  1, counters)
    print("")
    dump_muxes("serpar", serpar, 1, counters)
    print("")
    dump_muxes("term",   term,   1, counters)
    print("")
    dump_muxes("hip",    hip,    1, counters)
    print("")
    dump_muxes("hmc",    hmc,    1, counters)

    print("")
    dump_muxes("lab",    lab,    2, counters)
    print("")
    dump_muxes("mlab",   mlab,   2, counters)
    print("")
    dump_muxes("m10k",   m10k,   2, counters)
    print("")
    dump_muxes("dsp",    dsp,    2, counters)
    print("")
    dump_muxes("hps_clocks", hps_clocks, 2, counters)

