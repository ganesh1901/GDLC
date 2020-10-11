
from matplotlib.pyplot import figure, show

from numpy import arange, sin, pi

import struct
import os
import time
import sys
import binascii

if (len(sys.argv) != 3):
    print 'usage == python 1)filein name 2)Output file '
    exit(0)

filename = sys.argv[1]

if os.path.islink(filename):
    real_path = os.readlink(filename)
    time1 = os.path.getctime(real_path)
else:
    time1 = os.path.getctime(filename)

timestamp = time.strftime('%d-%m-%Y %H:%M:%S', time.localtime(time1))

f = open(filename, "rb")
fmt = "74B"
packet_size = struct.calcsize(fmt)

print 'packet_size', packet_size

line_count = os.stat(filename).st_size / packet_size
print 'line count ', line_count

outfilename = sys.argv[2]
f1 = open(outfilename, "w+")


for y in range(line_count):
    bytes = f.read(packet_size)
    if bytes:
        data = struct.unpack(fmt, bytes)
        li = []
        for j in range(len(data)):
            if j in [0, len(data)-1]:
                continue
            a = data[j]
            if(a < 0x40):
                a = a - 0x30
            else:
                a = a - 0x41 + 0xa
            li.append(a)

        print li
        #converting 72 bytes to 36 bytes
        ti = []
        for k in range(len(li) / 2):
            ti.append(li[k * 2 + 1] | li[k * 2] << 4)
            #f1.write("%02x " % ti[k])
        #f1.write('\n')

        #byte swaping
        for l in range(0, len(ti) - (len(ti) % 2), 2):
            ti[l], ti[l + 1] = ti[l + 1], ti[l]

        for kk in range(len(ti)):
            f1.write("%02x " % ti[kk])
        f1.write('\n')

f.close()
f1.close()
