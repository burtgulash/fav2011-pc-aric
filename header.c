#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "header.h"
#include "coder.h"

uint16_t freq[NUM_SYMBOLS];
uint16_t *inverse;

/* output short int in little endian */
void write_short(uint16_t n, FILE * out)
{
    putc(n, out);
    putc(n >> 8, out);
}

/* read one short coded in little endian from stream */
uint16_t read_short(FILE * in)
{
    return getc(in) | getc(in) << 8;
}

void write_header()
{
    int i, c;
    uint32_t len = 0;
    uint32_t tmp[NUM_SYMBOLS];
    int used[NUM_SYMBOLS];

    memset(tmp, 0, sizeof(uint32_t) * NUM_SYMBOLS);
    memset(used, 0, sizeof(uint32_t) * NUM_SYMBOLS);

    while ((c = getc(in)) != EOF) {
        used[c + 1] = 1;
        tmp[c + 1]++;
        len++;
    }
    used[CODE_EOF] = 1;

    /* downscale probabilities by 2**(W - 1) / len */
    for (i = 1; i < NUM_SYMBOLS; i++)
        tmp[i] = ((uint64_t) tmp[i] << (W - 1)) / len;

    for (i = 1; i < NUM_SYMBOLS; i++) {
        tmp[i] += used[i];      /* ensure that each symbol has range */
        tmp[i] += tmp[i - 1];   /* cumulate probabilities */
        assert(tmp[i] >= tmp[i - 1]);
    }

    len = tmp[NUM_SYMBOLS - 1] + 1;
    /* upscale probabilities to full range 2**W */
    for (i = 1; i < NUM_SYMBOLS; i++)
        tmp[i] = (tmp[i] << W) / len;

    /* copy ints to shorts */
    for (i = 1; i < NUM_SYMBOLS; i++) {
        freq[i] = tmp[i];
        assert(freq[i] >= freq[i - 1]);
    }

    freq[0] = FREQ_MIN;
    freq[NUM_SYMBOLS - 1] = FREQ_MAX;

    for (i = 1; i < NUM_SYMBOLS - 1; i++)
        write_short(freq[i], out);

    coded_bytes += sizeof(uint16_t) * (NUM_SYMBOLS - 2);
    rewind(in);                 /* rewind infile for encoding */
}

void read_header()
{
    int i, j;

    j = 0;
    for (i = 1; i < NUM_SYMBOLS - 1; i++) {
        freq[i] = read_short(in);
        for (; j < freq[i]; j++)
            inverse[j] = i - 1;
    }
    for (; j <= FREQ_MAX; j++)
        inverse[j] = CODE_EOF;

    orig_bytes += (NUM_SYMBOLS - 2) * sizeof(uint16_t);

    freq[0] = 0;
    freq[NUM_SYMBOLS - 1] = (1 << W) - 1;
}
