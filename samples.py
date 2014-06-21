import os
import base64

# Takes key bytes and returns S
def ksa(key):
    S = range(256)

    for i in range(0,256):
        S[i] = i

    j = 0
    for i in range(0,256):
        j = (j + S[i] + key[i % len(key)]) % 256
        S[i], S[j] = S[j], S[i]

    return S

def prga(S):
    i = 0
    j = 0
    while True:
        i = (i + 1) % 256
        j = (j + S[i]) % 256
        S[i], S[j] = S[j], S[i]
        K = S[(S[i] + S[j]) % 256]
        yield K

def encrypt(text, gen):
    cipher = []
    for c in text:
        p_byte = ord(c)
        c_byte = p_byte ^ gen.next()
        cipher.append(chr(c_byte))
    cipher = ''.join(cipher)
    return cipher

def main():
    out = open("samples3.txt", "w")
    plaintext = "a" * 256
    n = pow(2,25)
    for i in range(n):
        key = [ord(c) for c in os.urandom(128)]
        S = ksa(key)
        gen = prga(S)
        cipher = encrypt(plaintext, gen)
        out.write("%s\n" % base64.b64encode(cipher))

        if not i % (n/100):
            print "%d percent" % ((i * 100) / n)

if __name__ == "__main__":
    main()