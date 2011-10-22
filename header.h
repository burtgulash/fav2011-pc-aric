#include <stdint.h>

#ifndef HEADER_H
#define HEADER_H

#define FREQ_MAX 0xFFFF
#define FREQ_MIN 0x0000

extern FILE *in, *out;

void write_short(uint16_t n, FILE * out);
uint16_t read_short(FILE * in);
void write_header();
void read_header();

#endif
