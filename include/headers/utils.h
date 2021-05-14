#ifndef UTILS_H
#define UTILS_H

typedef unsigned char byte_t;

int stringEndsWith(const char * str, const char * suffix);

int numberOfFilesInDirectory(const char * directoryPath);

int allFilesAreBmp(char *directoryPath);

void goToPixelStream(FILE * file, size_t * width, size_t * height);

void closeFiles(FILE ** files, size_t filesQty);

FILE ** getFilesInDirectory(const char * directoryPath);

byte_t getPixel(FILE * file, size_t pixel);

char parity(byte_t byte);

#endif //UTILS_H
