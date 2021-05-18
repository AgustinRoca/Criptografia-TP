#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>

typedef unsigned char byte_t;

int stringEndsWith(const char * str, const char * suffix);

int numberOfFilesInDirectory(const char * directoryPath);

int allFilesAreBmp(char *directoryPath);

void goToPixelStream(FILE * file, size_t * width, size_t * height);

void closeFiles(FILE ** files, size_t filesQty);

FILE ** getFilesInDirectory(const char * directoryPath, const char * mode);

byte_t getPixel(FILE * file, size_t pixel);

void setPixel(FILE * file, size_t pixel, byte_t byte);

char parity(byte_t byte);

size_t sToBinary(const char * s);

#endif //UTILS_H
