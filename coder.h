#include <stdint.h>

#ifndef CODER_H
#define CODER_H

#define NUM_SYMBOLS 258         /* 256 bytes, 1 CODE_EOF, 1 upper bound */
#define CODE_EOF 256

#define W 16
#define SHIFT 24
#define PRECISION_LIMIT 0x0000FFFFUL

#define INTERVAL_MAX 0xFFFFFFFFUL
#define INTERVAL_MIN 0x00000000UL


#define ENCODED 1
#define NOT_ENCODED 0

#define CORRUPTED "Archive corrupted"
#define SUCCESS 0
#define FAILURE -1

extern FILE *in, *out;
extern char *out_filename;

extern uint16_t freq[NUM_SYMBOLS];
extern uint16_t *inverse;

extern int coded_bytes;
extern int orig_bytes;

void encode(int c);
int decode();
void compress();
int decompress();

#endif
