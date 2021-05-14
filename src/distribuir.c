#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h> 
#include "headers/utils.h"
#include "headers/distribuir.h"
#include "headers/galois2_8.h"

#define CHUNK 500

// Funciones auxiliares
byte_t ** getBlocks(FILE * file, size_t blockSize, size_t * blockQty);
void freeBlocks(byte_t ** blocks, size_t blockQty);
byte_t evaluatePolynomial(byte_t * polynomial, size_t maxDegree, byte_t x);
byte_t ** getTopLeftBlocks(FILE * file, size_t blocksQty);

void distribuir(const char * nombreImagenSecreta, size_t k, const char *nombreDirectorio) {
    FILE * imagenSecreta = fopen(nombreImagenSecreta, "r");
    if(imagenSecreta == NULL) {
        if(errno == ENOENT) {
            printf("El archivo \"%s\" no existe\n", nombreImagenSecreta);
        } else {
            perror("fopen");
        }
        exit(EXIT_FAILURE);
    }

    size_t cantidadDeBloques = 0;
    byte_t ** bloques = getBlocks(imagenSecreta, k, &cantidadDeBloques);
    printf("Tengo %zu bloques de tamaño %zu\n", cantidadDeBloques, k);

    // Obtengo las imagenes de camuflaje
    FILE ** files = getFilesInDirectory(nombreDirectorio);
    size_t filesQty = numberOfFilesInDirectory(nombreDirectorio);
    printf("%zu imagenes de camuflaje abiertas\n", filesQty);

    for (size_t camuflageFile = 0; camuflageFile < filesQty; camuflageFile++) {
        byte_t ** camuflageBlocks = getTopLeftBlocks(files[camuflageFile], k);
        for (size_t blockNumber = 0; blockNumber < k; blockNumber++) {
            // TODO: chequear que no este usando un X repetido
            byte_t fx = evaluatePolynomial(bloques[blockNumber], k, camuflageBlocks[blockNumber][0]);
            byte_t modifiedBlock[4];
            modifiedBlock[0] = camuflageBlocks[blockNumber][0];
            modifiedBlock[1] = (camuflageBlocks[blockNumber][1] & 11111000) + ((fx & 11100000) >> 5);
            modifiedBlock[2] = (camuflageBlocks[blockNumber][2] & 11111000) + ((fx & 00011100) >> 2);
            modifiedBlock[3] = (camuflageBlocks[blockNumber][3] & 11111000) +  (fx & 00000011) + (parity(fx) << 2);
            // TODO: poner el modifiedBlock en la imagen
        }
        
        freeBlocks(camuflageBlocks, k);
    }
    printf("Bloques de camuflage obtenidos\n");

    closeFiles(files, filesQty);
    freeBlocks(bloques, cantidadDeBloques);
    fclose(imagenSecreta);
}



byte_t ** getBlocks(FILE * file, size_t blockSize, size_t * blockQty){
    size_t width;
    size_t height;
    goToPixelStream(file, &width, &height);
    byte_t newBlock[blockSize];
    
    byte_t ** blocks = NULL;
    for(*blockQty = 0; fread(newBlock, 1, sizeof(newBlock), file) > 0; (*blockQty)++) {
        if(((*blockQty) % CHUNK) == 0){
            blocks = realloc(blocks, ((*blockQty) + CHUNK) * sizeof(*blocks));
            if(blocks == NULL){
                perror("realloc");
            }
            for (size_t block = *blockQty; block < (*blockQty) + CHUNK; block++){
                blocks[block] = malloc(blockSize * sizeof(**blocks));
                if(blocks[block] == NULL){
                    perror("malloc");
                }
            }            
        }

        for(size_t byteNumber = 0; byteNumber < sizeof(newBlock); byteNumber++){
            blocks[(*blockQty)][byteNumber] = newBlock[byteNumber];
        }
    }
    for(size_t block = *blockQty + 1; block < (*blockQty) + ((*blockQty)%CHUNK); block++){
        free(blocks[block]);
    }
    blocks = realloc(blocks, (*blockQty) * sizeof(*blocks));
    return blocks;
}

void freeBlocks(byte_t ** blocks, size_t blocksQty){
    for(size_t block = 0; block < blocksQty; block++){
        free(blocks[block]);
    }
    free(blocks);
}

byte_t evaluatePolynomial(byte_t * polynomial, size_t maxDegree, byte_t x){
    if(x > 255)
        perror("Byte demasiado grande para el polinomio (>= 2^8)");
    byte_t ans = 0;
    int base = 1;
    for (size_t degree = 0; degree < maxDegree; degree++, base <<= 1) {
        if((polynomial[degree] & base) != 0){
            ans = sum(ans, power(x, degree));
        }
    }
    return ans;
}

byte_t ** getTopLeftBlocks(FILE * file, size_t blocksQty){
    size_t width;
    size_t height;
    goToPixelStream(file, &width, &height);

    byte_t ** blocks = malloc(blocksQty * sizeof(*blocks));
    for (size_t i = 0; i < blocksQty; i++)
    {
        blocks[i] = malloc(4 * sizeof(**blocks));
        size_t topLeft = width * (height - 1) + 2*i;
        size_t topRight = topLeft + 1;
        size_t bottomLeft = width * (height - 2) + 2*i;
        size_t bottomRight = bottomLeft + 1;

        blocks[i][0] = getPixel(file, topLeft);
        blocks[i][1] = getPixel(file, topRight);
        blocks[i][2] = getPixel(file, bottomLeft);
        blocks[i][3] = getPixel(file, bottomRight);
    }

    return blocks;
}
