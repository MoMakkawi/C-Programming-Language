#ifndef PACKING_WANG_H
#define PACKING_WANG_H

#define MAXFNAMELENGTH 256

typedef struct {
    char filename[MAXFNAMELENGTH];
    unsigned int numofbytes;
} filtestructtype;

void filepacking(const char *packedfilename, int numoffiles, char **arrayofnames);
void fileunpacking(const char *packedfilename);

#endif
