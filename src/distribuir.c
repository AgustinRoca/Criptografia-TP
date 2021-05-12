#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "headers/distribuir.h"

#define OFFSET_POSITION 10
#define OFFSET_SIZE 4

// Funciones auxiliares
void irAStreamDePixeles(FILE * file);
size_t charArrayToUint(char array[4]);
size_t obtenerOffset(FILE * file);
char * getFullName(const char * directory, const char * filename);


void distribuir(const char * nombreImagenSecreta, int k, const char *nombreDirectorio) {
    char * fullName = getFullName(nombreDirectorio, nombreImagenSecreta);
    FILE * imagenSecreta = fopen(fullName, "r");
    if(imagenSecreta == NULL) {
        if(errno == ENOENT) {
            printf("El archivo \"%s\" no existe\n", fullName);
        } else {
            perror("fopen");
        }
        exit(EXIT_FAILURE);
    }
    irAStreamDePixeles(imagenSecreta);
    printf("Primeros 4 bytes del stream: %x %x %x %x\n", fgetc(imagenSecreta), fgetc(imagenSecreta), fgetc(imagenSecreta), fgetc(imagenSecreta));
    fclose(imagenSecreta);
    free(fullName);
}

void irAStreamDePixeles(FILE * file){
    size_t offset = obtenerOffset(file);
    fseek(file, offset, SEEK_SET);
}

size_t charArrayToUint(char array[4]){
    size_t answer = 0;
    size_t base = 1;
    for (size_t byte = 0; byte < 4; byte++, base<<=8)
    {
        answer += array[3-byte] * base;
    }
    return answer;
}

size_t obtenerOffset(FILE * file){
    fseek(file, OFFSET_POSITION, SEEK_SET); // salteo hasta la posicion del offset

    char offsetBytes[OFFSET_SIZE]; 
    for (size_t byte = 0; byte < OFFSET_SIZE; byte++)
    {
        offsetBytes[(OFFSET_SIZE - 1) - byte] = fgetc(file); // obtengo el offset que esta en formato little endian
    }
    return charArrayToUint(offsetBytes);
}

char * getFullName(const char * directory, const char * filename){
    char * ans = malloc(strlen(directory) + strlen(filename) + 2);
    strcpy(ans, directory);
    strcat(ans, "/");
    strcat(ans, filename);
    return ans;
}
