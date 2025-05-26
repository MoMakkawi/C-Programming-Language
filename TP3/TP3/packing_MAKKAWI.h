#ifndef PACKING_MAKKAWI_H
#define PACKING_MAKKAWI_H

#define MAXFNAMELENGTH 256

typedef struct {
    char filename[MAXFNAMELENGTH];
    unsigned int numofbytes;
} filestruct;

void filepacking(const char* packedfilename, int numoffiles, char** arrayofnames);
void fileunpacking(const char* packedfilename);

#endif
