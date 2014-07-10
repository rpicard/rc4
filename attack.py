import base64
import json

from math import log
from collections import Counter

def main():
    C = ciphers()

    p = [[None for j in range(256)] for i in range(256)]

    for line in open('dist_2_45.txt'):

        line = line.strip()

        if line == '' or line[0] == '%':
            continue

        (position, byte_value, count) = line.split()

        position = int(position)
        byte_value = int(byte_value)
        count = int(count)

        # Should count be / 2^45 to form a probability?
        p[position][byte_value] = count / float(pow(2,45))

    f = open('our_dist.txt')
    H = json.loads(f.read())

    plaintext = []
    # attacking the first 256 bytes
    for r in range(256):
        byte = attack_byte(H[r], r, p)
        plaintext.append(byte)

    num_right = (Counter(plaintext))[ord('a')]

    percent = (num_right * 100) / 256

    print ''.join([chr(b) for b in plaintext])
    print "Got %d percent right" % percent


def ciphers():
    f = open("samples/samples.txt")

    while True:
        line = f.readline().strip()

        str_cipher = base64.b64decode(line)
        buf_cipher = [ord(c) for c in str_cipher]

        yield buf_cipher


def attack_byte(N, r, p):

    # for each potential plaintext byte
    prob = [0 for i in range(256)]
    for u in range(256):
        prob[u] = sum([N[k ^ u] * log(p[r][k]) for k in range(256)])

    return prob.index(max(prob))


if __name__ == '__main__':
    main()