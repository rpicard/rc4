#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

void ksa(uint8_t key[], uint8_t S[]);
void random_key(uint8_t key[]);
void prga(uint8_t S[], uint8_t K[], int num_bytes);
void rc4_encrypt(uint8_t plaintext[], uint8_t S[], uint8_t ciphertext[]);
void test_rc4(void);

const int KEY_LENGTH = 16;
const uint64_t NUM_SAMPLES = 1024;

uint8_t range[256]; // (13 * 2^30) / 32 
int debug;
int urandom;

int main(int argc, char const *argv[])
{
    uint8_t key[KEY_LENGTH];            // Our single key
    uint8_t plaintext[256] = {69};      // The plaintext we want to recover
    uint8_t ciphertext[256];            // A buffer for our ciphertext
    uint8_t S[256];                     // The RC4 state
    uint64_t N[256][256] = { { 0 } };   // Distribution of bytes at each position
    uint8_t byte;                       // A placeholder for a byte
    FILE *fp;                           // The file we write our results to

    fp = fopen(argv[1], "w");
    urandom = open("/dev/urandom", O_RDONLY);

    // So we don't have to reset S on every iteration
    for (int i = 0; i < 256; i++) {
        range[i] = i;
    }

    // Generate the samples
    for (uint64_t i = 0; i < NUM_SAMPLES; i++) {

        if (i > 250 && i < 254) {
            debug = 0;
        }
        else {
            debug = 0;
        }

        random_key(key);

        if (debug) {
            printf("Key: ");
            for (int n = 0; n < 256; n++) {
                printf("%x", key[n]);
            }
            printf("\n\n");
        }
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
            //printf("%d %d %llu\n", i, j, N[i][j]);
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

    /*if (debug) {
        printf("State: ");
        for (int n = 0; n < 256; n++) {
            printf("%x", S[n]);
        }
        printf("\n\n");
    }*/
    
    memcpy(S, range, sizeof(uint8_t) * 256);

    /*if (debug) {
        printf("State (range): ");
        for (int n = 0; n < 256; n++) {
            printf("%x", S[n]);
        }
        printf("\n\n");
    }*/

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
    read(urandom, key, KEY_LENGTH);
}

void prga(uint8_t S[], uint8_t K[], int num_bytes)
{
    int i = 0;
    int j = 0;

    int tmp;

    for (int n = 0; n < num_bytes; n++) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;

        tmp = S[i];
        S[i] = S[j];
        S[j] = tmp;

        K[n] = S[(S[i] + S[j]) % 256];
        //if (debug) {
        //    printf("%x", K[n]);
        //}
    }
    //if (debug) {
    //    printf("\n");
    //}
}

void rc4_encrypt(uint8_t plaintext[], uint8_t S[], uint8_t ciphertext[])
{
    uint8_t K[256];

    prga(S, K, 256);

    for (int i = 0; i < 256; i++) {
        ciphertext[i] = plaintext[i] ^ K[i];
    }
}

void test_rc4(void)
{
    uint8_t key[KEY_LENGTH] = {'W', 'i', 'k', 'i'};
    uint8_t S[256];
    uint8_t plaintext[5] = {'p', 'e', 'd', 'i', 'a'};
    uint8_t ciphertext[5];
    uint8_t K[5] = {1};

    for (int i = 0; i < 256; i++) {
        range[i] = i;
    }

    ksa(key, S);

    prga(S, K, 5);

    for (int i = 0; i < 5; i++) {
        ciphertext[i] = plaintext[i] ^ K[i];
    }
    
    printf("State: ");
    for (int i = 0; i < 256; i++) {
        printf("%x", S[i]);
    }
    printf("\n");

    printf("Keystream: ");
    for (int i = 0; i < 5; i++) {
        printf("%x", K[i]);
    }
    printf("\n");

    printf("Ciphertext: ");
    for (int i = 0; i < 5; i++) {
        printf("%x", ciphertext[i]);
    }
    printf("\n");

}