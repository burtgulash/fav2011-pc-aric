#include <stdio.h>
#include <stdlib.h>
#include <string.h>             /* for memset */
#include <stdint.h>
#include <assert.h>
#include "coder.h"
#include "header.h"


uint32_t high = INTERVAL_MAX;
uint32_t low = INTERVAL_MIN;

uint32_t code;

int count = 0;
void flush_word()
{
    int i;
    putc(high >> SHIFT, out);
    for (i = 0; i < sizeof(code) - 1; i++)
        putc(0, out);
    coded_bytes += sizeof(code);

    high = INTERVAL_MAX;
    low = INTERVAL_MIN;
}

void read_word()
{
    code = 0;

    int i;
    for (i = 0; i < sizeof(code); i++)
        code = code << 8 | getc(in);
    orig_bytes += sizeof(code);

    high = INTERVAL_MAX;
    low = INTERVAL_MIN;
}

void copy(FILE * in, FILE * out)
{
    int c;
    while ((c = getc(in)) != EOF) {
        orig_bytes++;
        coded_bytes++;
        putc(c, out);
    }
}

/*  Try to compress file. If the coded file is larger in size than source,
 *  just copy over the source file.
 */
void compress()
{
    int c;
    putc(ENCODED, out);         /* set flag to notify encoding */
    coded_bytes++;

    write_header();

    /* encoding loop */
    while ((c = getc(in)) != EOF) {
        orig_bytes++;
        encode(c);
    }
    encode(CODE_EOF);

    /* write last 4 bytes */
    flush_word();

    /* if encoded file larger than source, don't encode */
    if (coded_bytes > orig_bytes) {
        freopen(out_filename, "wb", out);
        if (!out) {
            perror("could not open file");
            return;
        }

        rewind(in);
        /* indicate in header that we are just dumping original file */
        putc(NOT_ENCODED, out);
        orig_bytes = 0;
        coded_bytes = 1;        /* flag is 1 byte */

        /* copy over */
        copy(in, out);
    }
}

void encode(int symbol)
{
    uint64_t range = high - low;
    if (range <= PRECISION_LIMIT) {
        flush_word();
        range = high - low;
    }

    /* update interval boundaries */
    high = low + (range * freq[symbol + 1] >> 16) - 1;
    low  = low + (range * freq[symbol] >> 16) + 1;

    assert(low <= high);

    /* output identical leading bytes */
    while ((low ^ high) <= 0x00FFFFFFUL) {
        putc(low >> SHIFT, out);
        coded_bytes++;

        low <<= 8;
        high = high << 8 | 0xFF;
    }
}

int decompress()
{
    int c;
    c = getc(in);
    orig_bytes++;
    if (c == NOT_ENCODED) {
        copy(in, out);
        return SUCCESS;
    }

    inverse = (uint16_t *) malloc(sizeof(uint16_t) * 0x10000);
    assert(inverse);

    read_header();
    read_word();                /* initialize buffer */

    while ((c = decode()) != CODE_EOF)
        if (c == FAILURE) {
            free(inverse);
            fprintf(stderr, CORRUPTED);
            return FAILURE;
        }

    free(inverse);
    return SUCCESS;
}



int decode()
{
    uint64_t range = high - low;
    if (range <= PRECISION_LIMIT) {
        read_word();
        range = high - low;
    }

    /* range must be larger than 2**W for this step */
    int symbol_freq = (((uint64_t) (code - low)) << W) / range;
    int symbol = inverse[symbol_freq];

    /* END of archive reached, decoding was successful */
    if (symbol == CODE_EOF) {
        orig_bytes++;
        return CODE_EOF;
    }

    putc(symbol, out);
    coded_bytes++;

    /* update interval boundaries */
    high = low + (range * freq[symbol + 1] >> 16) - 1;
    low  = low + (range * freq[symbol] >> 16) + 1;

    if (low > code || code > high)
        return FAILURE;

    /* output identical leading bytes */
    int c;
    while ((low ^ high) <= 0x00FFFFFFUL) {
        assert((code ^ low) <= 0x00FFFFFFUL);

        if ((c = getc(in)) == EOF)
            return FAILURE;
        code = code << 8 | c;
        orig_bytes++;

        high = high << 8 | 0xFF;
        low <<= 8;
    }
    return SUCCESS;
}
