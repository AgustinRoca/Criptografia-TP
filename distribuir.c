#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "headers/distribuir.h"


void distribuir(const char * nombreImagenSecreta, int k, const char *nombreDirectorio) {
    FILE * imagenSecreta = fopen(nombreImagenSecreta, "r");
    if(imagenSecreta == NULL) {
        if(errno == ENOENT) {
            printf("El archivo \"%s\" no existe\n", nombreImagenSecreta);
        } else {
            perror("fopen");
        }
        exit(EXIT_FAILURE);
    }
}

