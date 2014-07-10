#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

void ksa(uint8_t key[], uint8_t S[]);
void random_key(uint8_t key[]);
void prga(uint8_t S[], uint8_t K[]);

void rc4_encrypt(uint8_t plaintext[], uint8_t S[], uint8_t ciphertext[]);

const int KEY_LENGTH = 16;
const uint64_t NUM_SAMPLES = 2048;

uint8_t range[256];

int main(int argc, char const *argv[])
{
    uint8_t key[KEY_LENGTH];   // Our single key
    uint8_t plaintext[256] = {69};
    uint8_t ciphertext[256];
    uint8_t S[256];
    uint64_t N[256][256] = { { 0 } };
    uint8_t byte;
    FILE *fp;

    fp = fopen("samples/single_key1.txt", "w");

    for (int i = 0; i < 256; i++) {
        range[i] = i;
    }

    for (uint64_t i = 0; i < NUM_SAMPLES; i++) {

        random_key(key);
        ksa(key, S);

        rc4_encrypt(plaintext, S, ciphertext);

        for (int r = 0; r < 256; r++) {
            byte = ciphertext[r];
            N[r][byte]++;
        }

        if (i % (NUM_SAMPLES/100) == 0) {
            printf("%llu%%\n", (i * 100) / NUM_SAMPLES);
        }
    }

    printf("100%% (hopefully)\n");

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            fprintf(fp, "%d %d %llu\n", i, j, N[i][j]);
        }
    }


    fclose(fp);

    return 0;
}

void ksa(uint8_t key[], uint8_t S[])
{
    int j = 0;
    uint8_t tmp;
    
    memcpy(S, range, sizeof(uint8_t) * 256);

    for (int i = 0; i < 256; i++) {
        j += S[i];
        j += key[i % KEY_LENGTH];
        j %= 256;

        // Swap S[i] and S[j]
        tmp = S[i];
        S[i] = S[j];
        S[j] = tmp;
    }
}

void random_key(uint8_t key[])
{
    int urandom = open("/dev/urandom", O_RDONLY);
    read(urandom, key, KEY_LENGTH);
}

void prga(uint8_t S[], uint8_t K[])
{
    int i = 0;
    int j = 0;

    int tmp;

    for (int n = 0; n < 256; n++) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;

        tmp = S[i];
        S[i] = S[j];
        S[j] = tmp;

        K[n] = S[(S[i] + S[j]) % 256];
    }
}

void rc4_encrypt(uint8_t plaintext[], uint8_t S[], uint8_t ciphertext[])
{
    uint8_t K[256];

    prga(S, K);

    for (int i = 0; i < 256; i++) {
        ciphertext[i] = plaintext[i] ^ K[i];
    }
}