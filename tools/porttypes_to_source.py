#!/usr/bin/python

import sys

ports = {}

lab_ports = [ 'A', 'B', 'C', 'D', 'E0', 'E1', 'F0', 'F1', 'FFT0', 'FFT1', 'FFT1L', 'FFB0', 'FFB1', 'FFB1L', 'DATAIN', 'CLKIN', 'ACLR' ]
dsp_ports = ['DATAIN', 'RESULT', 'ACLR', 'NEGATE', 'LOADCONST', 'ACCUMULATE', 'SUB', 'ENABLE', 'UNK_IN', 'CLKIN' ]
m10k_ports = [ 'BYTEENABLEA', 'BYTEENABLEB', 'ENABLE', 'ACLR', 'RDEN', 'WREN', 'ADDRSTALLA', 'ADDRSTALLB', 'ADDRA', 'ADDRB', 'DATAAIN', 'DATABIN', 'DATAAOUT', 'DATABOUT', 'CLKIN' ]

for pn in lab_ports + dsp_ports + m10k_ports:
    ports[pn] = True

for model in ["e50f", "gt150f", "gt300f", "gt75f", "gx25f", "sx120f", "sx50f"]:
    for l in open(sys.argv[1] + '/' + model + '-p2r.txt'):
        if l[0] == ' ':
            pn = l.rstrip('\r\n').split()[0]
            ports[pn.upper()] = True

    for l in open(sys.argv[1] + '/' + model + '-p2p.txt'):
        ls = l.rstrip('\r\n').split()
        for pnode in ls:
            ls2 = pnode.split(':')
            if len(ls2) < 2:
                continue
            pn = ls2[1].split('.')[0]
            ports[pn] = True

pl = list(ports.keys())
pl.sort()

print("P(PNONE),")
print()

for pn in pl:
    print("P(%s)," % pn)
    
