import sys

def toHex(b):
    h = hex(b)
    h = h[:2] + '0' + h[-1] if len(h) == 3 else h
    return h.encode()


def main(args):
    if len(args) < 2:
        return
    outName = args[1].split('.')[0] + ".txt" if len(args) == 2 else args[2]
    with open(args[1], 'rb') as file:
        data = file.read() 
    with open(outName, "wb") as file:
        file.write(b'const int DEFAULT_FONT_LENGTH = ');
        file.write(str(len(data)).encode())
        file.write(b';\n\nconst Uint8 DEFAULT_FONT[] = {\n\t')
        file.write(toHex(data[0]))
        count = 0
        for byte in data[1:]:
            count += 1
            file.write(b',')
            if count == 16:
                file.write(b'\n\t')
                count = 0
            else:
                file.write(b' ')
            file.write(toHex(byte))

        file.write(b'\n};\n')


if __name__ == "__main__":
    main(sys.argv)