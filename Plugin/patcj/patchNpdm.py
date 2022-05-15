import struct, binascii

HEADER_SIZE = 0x70
PERMISSIONS_DATA = """
30 03 00 00 9C 00 00 00 80 00 00 00 AC 02 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
41 43 49 44 AC 01 00 00 00 00 00 00 01 00 00 00
00 00 01 00 00 00 00 01 FF FF FF FF FF FF FF 01
40 02 00 00 2C 00 00 00 70 02 00 00 02 00 00 00
80 02 00 00 2C 00 00 00 00 00 00 00 00 00 00 00
01 00 00 00 FF FF FF FF FF FF FF FF 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 2A 00 00 00 00 00 00 00 00 00 00 00 00 00 00
B7 73 00 03 EF FF FF 1F EF FF FF 3F EF 07 E6 47
EF FF FF 7F EF FF F7 9F EF 1F 00 A0 FF 5F 00 00
FF BF 48 00 FF 7F 00 02 FF FF 02 00 00 00 00 00
41 43 49 30 00 00 00 00 00 00 00 00 00 00 00 00
00 E0 16 00 80 6A 00 01 00 00 00 00 00 00 00 00
40 00 00 00 1C 00 00 00 60 00 00 00 02 00 00 00
70 00 00 00 2C 00 00 00 00 00 00 00 00 00 00 00
01 00 00 00 FF FF FF FF FF FF FF FF 1C 00 00 00
00 00 00 00 1C 00 00 00 00 00 00 00 00 00 00 00
00 2A 00 00 00 00 00 00 00 00 00 00 00 00 00 00
B7 73 00 03 EF FF FF 1F EF FF FF 3F EF 07 E6 47
EF FF FF 7F EF FF F7 9F EF 1F 00 A0 FF 5F 00 00
FF BF 48 00 FF 7F 00 02 FF FF 02 00
"""

def unhexlify(string):
    return binascii.unhexlify(string.encode('latin-1'))

def hexlify(byte_string):
    return binascii.hexlify(byte_string).decode('latin-1')

def read_u32(f):
    return struct.unpack('<L', f.read(4))[0]

def write_u32(f, u32):
    return f.write(struct.pack('<L', u32))

def read_tid(f):
    return f.read(8)

def from_hex(hex_str):
    return unhexlify(hex_str.replace(' ', '').replace('\n', ''))

def patch_npdm(in_file, out_file):
    copy = lambda x: out_file.write(in_file.read(x))
    # copy header
    copy(HEADER_SIZE)
    aci0_offset = read_u32(in_file)
    in_file.seek(aci0_offset + 0x10)
    tid = read_tid(in_file)
    print("Title ID = " + hexlify(tid[::-1]))
    data = from_hex(PERMISSIONS_DATA)
    data = data[:0x2D0] + tid + data[0x2D8:]
    out_file.write(data)

def main(args):
    with open(args.npdm_in, 'rb') as npdm_in:
        with open(args.npdm_out, 'wb') as npdm_out:
            patch_npdm(npdm_in, npdm_out)

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('npdm_in', help='NPDM to patch')
    parser.add_argument('npdm_out', help='File to output NPDM to')
    args = parser.parse_args()

    main(args)
