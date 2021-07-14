#!/usr/bin/env python3
# UWB binary sniffer decoder
#
# The LPS node should be in sniffer mode already, it will be switched to
# binary mode by this script.

import sys
import struct
import serial
import binascii
import yaml
import time

if len(sys.argv) < 2:
    print("usage: {} <sniffer serial port> [format]".format(sys.argv[0]))
    print("  Possible format: human (default), csv, yaml")
    sys.exit(1)

ser = serial.Serial(sys.argv[1], 115200)

outputFormat = 'human'
if len(sys.argv) > 2:
    outputFormat = sys.argv[2].strip()

# Switch to binary mode
ser.write(b'b')

while True:
    c = ser.read(1024)
    print(c, end='')

    # if c == b'I':
    #     tround1 = ser.read(8)
    #     tround1_unpack = struct.unpack('<I',binascii.unhexlify(tround1))[0]
    #     ser.read(1)

    #     tround2 = ser.read(8)
    #     tround2_unpack = struct.unpack('<I',binascii.unhexlify(tround2))[0]
    #     ser.read(1)

    #     treply1 = ser.read(8)
    #     treply1_unpack = struct.unpack('<I',binascii.unhexlify(treply1))[0]
    #     ser.read(1)

    #     treply2 = ser.read(8)
    #     treply2_unpack = struct.unpack('<I',binascii.unhexlify(treply2))[0]
    #     ser.read(1)

    #     print(tround1,tround2,treply1,treply2,tround1_unpack,tround2_unpack,treply1_unpack,treply2_unpack)
    #     tprop_ctn = ((tround1_unpack*tround2_unpack) - (treply1_unpack*treply2_unpack)) / (tround1_unpack + tround2_unpack + treply1_unpack + treply2_unpack)
        
    #     tprop = tprop_ctn/(499.2e6 * 128)
    #     distance = 299792458.0 * tprop

    #     # [tround1,tround2,treply1,treply2] = struct.unpack(data).split(':')
    #     print(distance)

# 01b6a422:01b4edc4:01b6a1c2:01b4ebde