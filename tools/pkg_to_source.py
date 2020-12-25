#!/usr/bin/python

import sys

if len(sys.argv) != 3:
    print("%s <data-dir> chip         -- Generate the data source for the possible packages" % sys.argv[0])
    sys.exit(0)

package_codes = {
    'f17': 256,
    'f23': 484,
    'f27': 672,
    'f31': 896,
    'f35': 1152,

    'u15': 324,
    'u19': 484,
    'u23': 672,

    'm11': 301,
    'm13': 383,
    'm15': 484,
    }

print("// Generated file, edition is futile")
print("")
print("#include \"cyclonev.h\"")

def f(ls, s):
    s += ':'
    l = len(s)
    for i in range(3, len(ls)):
        if ls[i][:l] == s:
            return ls[i][l:]
    return False

in_package = False

for l in open(sys.argv[1] + '/' + sys.argv[2] + '-pkg.txt'):
    ls = l.rstrip('\r\n').split()

    if l[0] != ' ':
        if in_package:
            print("};")
        in_package = True

        print("")
        print("const mistral::CycloneV::pin_info_t mistral::CycloneV::%s_package_%s[%d] = {" % (sys.argv[2], ls[0], package_codes[ls[0]]))
    else:
        s = '  { %2d, %2d,' % (int(ls[0]), int(ls[1]))
        if f(ls, 'gpio'):
            pos = f(ls, 'gpio')
            posx = pos.split('.')
            s += ' xy2pos(%d, %d) | (%d << 14),' % (int(posx[0]), int(posx[1]), int(posx[2]))
        else:
            s += ' 0xffff,'

        index = None
        fl = '0'
        if f(ls, 'dedicated_programming_pad'):
            fl = 'PIN_DPP'
            index = f(ls, 'dedicated_programming_pad')
        elif f(ls, 'hssi_input'):
            fl = 'PIN_HSSI'
            index = f(ls, 'hssi_input')
        elif f(ls, 'jtag'):
            fl = 'PIN_JTAG'
            index = f(ls, 'jtag')
        elif f(ls, 'gpio'):
            fl = 'PIN_GPIO'
        if 'hps' in ls:
            fl += '|PIN_HPS'
        if 'dm' in ls:
            fl += '|PIN_DM'
        if 'dqs' in ls:
            fl += '|PIN_DQS'
        if 'dqs_dis' in ls:
            fl += '|PIN_DQS_DIS'
        if 'dqsb' in ls:
            fl += '|PIN_DQSB'
        if 'dqsb_dis' in ls:
            fl += '|PIN_DQSB_DIS'
        if f(ls, 't'):
            t = f(ls, 't')
            if t == 'do-not-use':
                fl += '|PIN_DO_NOT_USE'
            elif t == 'gxp-reference-resistor':
                fl += '|PIN_GXP_RREF'
            elif t == 'nc':
                fl += '|PIN_NC'
            elif t == 'vcc':
                fl += '|PIN_VCC'
            elif t == 'vccl-sense':
                fl += '|PIN_VCCL_SENSE'
            elif t == 'vccn':
                fl += '|PIN_VCCN'
            elif t == 'vccpd':
                fl += '|PIN_VCCPD'
            elif t == 'vref':
                fl += '|PIN_VREF'
            elif t == 'vss':
                fl += '|PIN_VSS'
            elif t == 'vss-sense':
                fl += '|PIN_VSS_SENSE'
            else:
                print("Unknown type %s" % t, file=sys.stderr)
                sys.exit(1)
        s += ' ' + fl + ', "' + ls[2] + '", '
        if f(ls, 'f'):
            s += ' "' + f(ls, 'f') + '",'
        else:
            s += ' nullptr,'
        if f(ls, 'ioblock'):
            s += ' "' + f(ls, 'ioblock') + '",'
        else:
            s += ' nullptr,'
        if f(ls, 'r'):
            s += ' ' + f(ls, 'r') + ', ' + f(ls, 'c') + ', ' + f(ls, 'l') + ', ' + f(ls, 'len') + ', ' + f(ls, 'd') + ','
        else:
            s += ' 0, 0, 0, 0,'
        if index != None:
            s += ' ' + index
        else:
            s += ' -1'
        s += ' },'
        print(s)        
        
if in_package:
    print("};")
