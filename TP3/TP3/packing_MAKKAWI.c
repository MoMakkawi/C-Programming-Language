#include "packing_MAKKAWI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

#define MAXFNAMELENGTH 256

struct filestruct {
    char filename[MAXFNAMELENGTH];
    unsigned int numofbytes;
};


typedef struct filestruct filtestructtype;

void filepacking(const char* packedfilename, int numoffiles, char** arrayofnames) {
    FILE* out = fopen(packedfilename, "wb");
    if (!out) {
        perror("Error opening packed file for writing");
        return;
    }

    for (int i = 0; i < numoffiles; i++) {
        FILE* in = fopen(arrayofnames[i], "rb");
        if (!in) {
            perror("Error opening input file");
            continue;
        }

        fseek(in, 0, SEEK_END);
        unsigned int size = ftell(in);
        fseek(in, 0, SEEK_SET);

        filtestructtype f;
        strncpy_s(f.filename, MAXFNAMELENGTH, arrayofnames[i], _TRUNCATE);
        f.numofbytes = size;

        fwrite(&f, sizeof(filtestructtype), 1, out);

        char* buffer = malloc(size);
        fread(buffer, 1, size, in);
        fwrite(buffer, 1, size, out);

        free(buffer);
        fclose(in);
    }

    fclose(out);
}

void fileunpacking(const char* packedfilename) {
    FILE* in = fopen(packedfilename, "rb");
    if (!in) {
        perror("Error opening packed file for reading");
        return;
    }

    _mkdir("unpackedfiles");

    while (!feof(in)) {
        filtestructtype f;
        if (fread(&f, sizeof(filtestructtype), 1, in) != 1) break;

        char path[MAXFNAMELENGTH + 20];
        snprintf(path, sizeof(path), "unpacked_files/%s", f.filename);

        FILE* out = fopen(path, "wb");
        if (!out) {
            perror("Error creating output file");
            break;
        }

        char* buffer = malloc(f.numofbytes);
        fread(buffer, 1, f.numofbytes, in);
        fwrite(buffer, 1, f.numofbytes, out);

        free(buffer);
        fclose(out);
    }

    fclose(in);
}
