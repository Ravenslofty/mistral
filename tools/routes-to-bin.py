#!/usr/bin/python

import sys
import lzma
import struct

if len(sys.argv) != 5:
    print("Usage:\n%s <data> cv-rnodetypes.ipp chip <gdata>" % sys.argv[0])
    sys.exit(0)

chip = sys.argv[3]

nmap = {}
nid = 0
for l in open(sys.argv[2]):
    if len(l) > 2 and l[:2] == 'P(':
        idx = l.find(')')
        nmap[l[2:idx]] = nid
        nid += 1

wl = { 'e50f':4958, 'gt75f':6006, 'gt150f':7605, 'gt300f':10038, 'gx25f':3856, 'sx50f':6006, 'sx120f':7605 }

w = wl[chip]

muxes = []

def node_to_id(s):
    global nmap
    ns = s.split('.')
    t = nmap[ns[0]]
    x = int(ns[1])
    y = int(ns[2])
    z = int(ns[3])
    return (t << 24) | (x << 17) | (y << 10) | z

for l in lzma.open(sys.argv[1] + '/' + chip + '-r.txt.xz', "rt"):
    ls = l.rstrip('\r\n').split()
    dest = node_to_id(ls[0])
    idx1 = ls[1].find(':')
    idx2 = ls[1].find('_')
    pat = ls[1][:idx1]
    if '.' in pat:
        patx = pat.split('.')
        assert(patx[0] == '6')
        pat = 70 + int(patx[1])
    else:
        pat = int(pat)
    fwpos = int(ls[1][idx1+1:idx2]) + w*int(ls[1][idx2+1:])
    span = {}
    for n in ls[2:]:
        idx3 = n.find(':')
        span[int(n[:idx3])] = node_to_id(n[idx3+1:])
    muxes.append((dest, pat, fwpos, span))


muxes.sort()

bmux = bytearray(len(muxes)*188)
off = 0
for i in range(0, len(muxes)):
    struct.pack_into("III", bmux, off, muxes[i][0], muxes[i][1], muxes[i][2])
    for s,v in muxes[i][3].items():
        struct.pack_into("I", bmux, off + 12 + 4*s, v)
    off += 188

cbmux = lzma.compress(bmux)

print(" %d muxes" % len(muxes))

open(sys.argv[4] + '/' + chip + "-r.bin", "wb").write(cbmux)
