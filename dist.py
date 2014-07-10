import json
import base64

def main():
    H = [[0 for j in range(256)] for i in range(256)]

    C = ciphers()

    # For each cipher
    for j in range(pow(2,25)):
        cipher = C.next()

        if not j % (pow(2,25)/100):
            print "%d percent" % ((j * 100) / pow(2,25))

        for r in range(256):
             H[r][cipher[r]] += 1

    serialized = json.dumps(H)

    f = open('our_dist.txt', 'w')

    f.write(serialized)

def ciphers():
    f = open("samples/samples.txt")

    while True:
        line = f.readline().strip()

        str_cipher = base64.b64decode(line)
        buf_cipher = [ord(c) for c in str_cipher]

        yield buf_cipher


if __name__ == '__main__':
    main()