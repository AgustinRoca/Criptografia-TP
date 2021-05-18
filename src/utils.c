#define _DEFAULT_SOURCE
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/utils.h"

#define OFFSET_POSITION 10
#define OFFSET_SIZE 4
#define WIDTH_POSITION 18
#define WIDTH_SIZE 4
#define HEIGHT_POSITION 22
#define HEIGHT_SIZE 4

#define CHUNK 5

// Funciones auxiliares
unsigned int byteArrayToUint(byte_t array[4]);
unsigned int getOffset(FILE * file);
unsigned int getHeight(FILE * file);
unsigned int getWidth(FILE * file);
char * getFullName(const char * directory, const char * filename);

int stringEndsWith(const char * str, const char * suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    return
            (str_len >= suffix_len) &&
            (0 == strcmp(str + (str_len-suffix_len), suffix));
}

int numberOfFilesInDirectory(const char * directoryPath) {
    int file_count = 0;
    struct dirent * entry;

    DIR * dirp = opendir(directoryPath);
    if(dirp == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            file_count++;
        }
    }

    closedir(dirp);
    return file_count;
}

FILE ** getFilesInDirectory(const char * directoryPath, const char * mode){
    FILE ** files = NULL;
    size_t filesQty = 0;
    struct dirent *entry;

    DIR * dirp = opendir(directoryPath);
    if(dirp == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            if(filesQty % CHUNK == 0){
                files = realloc(files, (filesQty + CHUNK) * sizeof(*files));
            }
            char * path = getFullName(directoryPath, entry->d_name);
            files[filesQty++] = fopen(path, mode);
            free(path);
        }
    }
    closedir(dirp);
    files = realloc(files, filesQty * sizeof(*files));
    return files;
}

int allFilesAreBmp(char *directoryPath) {
    struct dirent * entry;

    DIR * dirp = opendir(directoryPath);
    if(dirp == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dirp)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        if(!stringEndsWith(entry->d_name, ".bmp")) {
            closedir(dirp);
            return 0; // Failure
        }
    }
    closedir(dirp);
    return 1; // Success
}

void goToPixelStream(FILE * file, size_t * width, size_t * height){
    unsigned int offset = getOffset(file);
    *width = getWidth(file);
    *height = getHeight(file);
    fseek(file, offset, SEEK_SET);
}

unsigned int byteArrayToUint(byte_t array[4]){
    unsigned int answer = 0;
    unsigned int base = 1;
    for (size_t byte = 0; byte < 4; byte++, base<<=8)
    {
        answer += array[3-byte] * base;
    }
    return answer;
}

unsigned int readLittleEndian(FILE * file, size_t bytesQty){
    byte_t bytes[bytesQty]; 
    for (size_t byte = 0; byte < bytesQty; byte++)
    {
        bytes[(bytesQty - 1) - byte] = fgetc(file); // obtengo el int que esta en formato little endian
    }
    return byteArrayToUint(bytes);
}

unsigned int getOffset(FILE * file){
    fseek(file, OFFSET_POSITION, SEEK_SET); // salteo hasta la posicion del offset
    return readLittleEndian(file, OFFSET_SIZE);
}

unsigned int getHeight(FILE * file){
    fseek(file, HEIGHT_POSITION, SEEK_SET); // salteo hasta la posicion del alto
    return readLittleEndian(file, HEIGHT_SIZE);
}

unsigned int getWidth(FILE * file){
    fseek(file, WIDTH_POSITION, SEEK_SET); // salteo hasta la posicion del ancho
    return readLittleEndian(file, WIDTH_SIZE);
}

void closeFiles(FILE ** files, size_t filesQty){
    for (size_t i = 0; i < filesQty; i++)
    {
        fclose(files[i]);
    }
}

byte_t getPixel(FILE * file, size_t pixel){
    fseek(file, pixel + getOffset(file), SEEK_SET);
    return (byte_t) fgetc(file);
}

void setPixel(FILE * file, size_t pixel, byte_t byte){
    fseek(file, pixel + getOffset(file), SEEK_SET);
    fputc(byte, file);
}

char * getFullName(const char * directory, const char * filename){
    char * ans = malloc(strlen(directory) + strlen(filename) + 2);
    strcpy(ans, directory);
    strcat(ans, "/");
    strcat(ans, filename);
    return ans;
}

char parity(byte_t byte){
    char bitsInOne = byte & 1;
    for (size_t i = 0; i < 8; i++, byte >>= 1)
    {
        bitsInOne ^= byte & 1;
    }

    return bitsInOne;
}

size_t sToBinary(const char * s){
    size_t i = 0;
    while (*s) {
        i <<= 1;
        i += *s++ - '0';
    }
    return i;
}

byte_t ** getTopLeftBlocks(FILE * file, size_t blocksQty){
    size_t width;
    size_t height;
    goToPixelStream(file, &width, &height);

    byte_t ** blocks = malloc(blocksQty * sizeof(*blocks));
    size_t blockRow = -1;
    for (size_t blockNumber = 0; blockNumber < blocksQty; blockNumber++)
    {
        if(((2*blockNumber) % width) == 0){
            blockRow++;
        }
        size_t topLeft = width * (height - 1 - 2*blockRow) + ((2*blockNumber)%width);
        size_t topRight = topLeft + 1;
        size_t bottomLeft = width * (height - 2 - 2*blockRow) + ((2*blockNumber)%width);
        size_t bottomRight = bottomLeft + 1;
        blocks[blockNumber] = malloc(4 * sizeof(**blocks));
        blocks[blockNumber][0] = getPixel(file, topLeft);
        blocks[blockNumber][1] = getPixel(file, topRight);
        blocks[blockNumber][2] = getPixel(file, bottomLeft);
        blocks[blockNumber][3] = getPixel(file, bottomRight);
    }

    return blocks;
}
