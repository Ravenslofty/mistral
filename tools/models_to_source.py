#!/usr/bin/python

import sys

if len(sys.argv) != 2:
    print("%s <data-dir>          -- Generate the variant and model objects" % sys.argv[0])
    sys.exit(0)

variants = []
models = []

for l in open(sys.argv[1] + '/models.txt'):
    ls = l.rstrip('\r\n').split()
    cb = {}
    if ls[0] == 'variant':
        variants.append(cb)
    else:
        models.append(cb)
        
    cb['n'] = ls[1]
    for i in ls[2:]:
        ii = i.split(':')
        cb[ii[0]] = ii[1]

print("// Generated file, edition is futile")
print("")
print("#include \"cyclonev.h\"")
print("")

for v in variants:
    print('const mistral::CycloneV::variant_info mistral::CycloneV::v_%s = { "%s", %s, 0x%s, %s, %s, %s, %s, %s, %s, %s };' % (v['n'], v['n'], v['die'], v['idcode'], v['alut'], v['alm'], v['memory'], v['dsp'], v['fpll'], v['dll'], v['hps']))

print("")
print("const mistral::CycloneV::Model mistral::CycloneV::models[] = {")

for m in models:
    print('  { "%s", v_%s, PKG_%s, \'%s\', %s, SG_%s, %s, %s, %s, %s, %s },' % (m['n'], m['v'], m['pkg'].upper(), m['temp'], m['speed'], m['sg'].upper(), m['pcie'], m['gxb'], m['hmc'], m['io'], m['gpio']))
print('  { nullptr, v_sx120f }')
print('};')

