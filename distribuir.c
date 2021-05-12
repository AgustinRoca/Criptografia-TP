#include <stdio.h>
#include <stdlib.h>
#include "headers/distribuir.h"


void distribuir(const char * nombreImagenSecreta, int k, const char *nombreDirectorio) {
    FILE * imagenSecreta = fopen(nombreImagenSecreta, "r");
    if(imagenSecreta == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }




}

