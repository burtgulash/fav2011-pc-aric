#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "coder.h"

#define USAGE "usage: aric -c FILE COMPRESSED\n       aric -d COMPRESSED FILE"
#define MEGA 1000000


FILE *in, *out;
char *out_filename;

/* keep track of written and read bytes */
int coded_bytes = 0;
int orig_bytes = 0;

int open_files(const char *read, const char *write)
{
    in = fopen(read, "rb");
    out = fopen(write, "wb");
    if (!in || !out) {
        perror("could not open file");
        return 0;
    }
    return 1;
}

void close_files()
{
    if (in)
        fclose(in);
    if (out)
        fclose(out);
}

int main(int argc, char **argv)
{
    if (argc != 4 || strlen(argv[1]) != 2 || argv[1][0] != '-') {
        printf("%s", USAGE);
        return EXIT_FAILURE;
    }

    if (!open_files(argv[2], argv[3]))
        return EXIT_FAILURE;

    char mode_flag = argv[1][1];
    clock_t start_time = clock();
    double megabytes;
    double seconds;

    switch (mode_flag) {
    case 'c':
        out_filename = argv[3];
        compress();
        megabytes = orig_bytes / MEGA;
        break;

    case 'd':
        if (decompress() == FAILURE)
            return EXIT_FAILURE;

        megabytes = coded_bytes / MEGA;
        break;

    default:
        fprintf(stderr, "%s\n", USAGE);
        return EXIT_FAILURE;
    }

    seconds = (double) (clock() - start_time) / CLOCKS_PER_SEC;
    megabytes /= seconds;

    fprintf(stderr, "%d bytes to %d bytes", orig_bytes, coded_bytes);
    /* NaN test && inf test */
    if (megabytes == megabytes && megabytes - megabytes == 0)
        fprintf(stderr, ", %6.1f MB/s", megabytes);
    fprintf(stderr, "\n");


    close_files();
    return EXIT_SUCCESS;
}
