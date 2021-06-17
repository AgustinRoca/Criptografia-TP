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
int contains(byte_t ** bytes, byte_t byte, size_t blockNumber, size_t filesQty);

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
    FILE ** files = getFilesInDirectory(nombreDirectorio, "r+");
    size_t filesQty = numberOfFilesInDirectory(nombreDirectorio);
    printf("%zu imagenes de camuflaje abiertas\n", filesQty);

    byte_t ** usedX = malloc(filesQty * sizeof(*usedX));
    for (size_t fileNumber = 0; fileNumber < filesQty; fileNumber++)
    {
        usedX[fileNumber] = malloc(cantidadDeBloques * sizeof(**usedX));
    }
        
    for (size_t camuflageFile = 0; camuflageFile < filesQty; camuflageFile++) {
        size_t width;
        size_t height;
        goToPixelStream(files[camuflageFile], &width, &height);

        byte_t ** camuflageBlocks = getTopLeftBlocks(files[camuflageFile], cantidadDeBloques);
        size_t blockRow = -1;
        for (size_t blockNumber = 0; blockNumber < cantidadDeBloques; blockNumber++) {
            // Chequeo que no haya usado ese X todavia
            for(int iteration = 0, overflow = 0; contains(usedX, camuflageBlocks[blockNumber][0], blockNumber, camuflageFile); iteration++){ // Si lo usé, lo cambio
                if(camuflageBlocks[blockNumber][0] + 1 >= 256){
                    overflow = 1;
                    camuflageBlocks[blockNumber][0] = camuflageBlocks[blockNumber][0] - iteration; // reseteo al valor original
                }
                camuflageBlocks[blockNumber][0] = camuflageBlocks[blockNumber][0] + (overflow?-1:1); // Si me pase de 256, me voy para atras
            }
            usedX[camuflageFile][blockNumber] = camuflageBlocks[blockNumber][0]; // Guardo que X usé
            
            byte_t fx = evaluatePolynomial(bloques[blockNumber], k, camuflageBlocks[blockNumber][0]);
            byte_t modifiedBlock[4];
            modifiedBlock[0] = camuflageBlocks[blockNumber][0];
            modifiedBlock[1] = (camuflageBlocks[blockNumber][1] & sToBinary("11111000")) + ((fx & sToBinary("11100000")) >> 5);
            modifiedBlock[2] = (camuflageBlocks[blockNumber][2] & sToBinary("11111000")) + ((fx & sToBinary("00011100")) >> 2);
            modifiedBlock[3] = (camuflageBlocks[blockNumber][3] & sToBinary("11111000")) + (fx & sToBinary("00000011")) + (parity(fx) << 2);
            if(((2*blockNumber) % width) == 0){
                blockRow++;
            }
            size_t topLeft = width * (height - 1 - 2*blockRow) + ((2*blockNumber)%width);
            size_t topRight = topLeft + 1;
            size_t bottomLeft = width * (height - 2 - 2*blockRow) + ((2*blockNumber)%width);
            size_t bottomRight = bottomLeft + 1;
            
            setPixel(files[camuflageFile], topLeft, modifiedBlock[0]);
            setPixel(files[camuflageFile], topRight, modifiedBlock[1]);
            setPixel(files[camuflageFile], bottomLeft, modifiedBlock[2]);
            setPixel(files[camuflageFile], bottomRight, modifiedBlock[3]);
        }
        
        freeBlocks(camuflageBlocks, cantidadDeBloques);
    }
    printf("Imagenes modificadas\n");

    for (size_t fileNumber = 0; fileNumber < filesQty; fileNumber++)
    {
        free(usedX[fileNumber]);
    }
    free(usedX);
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
    for(*blockQty = 0; fread(newBlock, 1, blockSize, file) > 0; (*blockQty)++) {
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

        for(size_t byteNumber = 0; byteNumber < blockSize; byteNumber++){
            blocks[(*blockQty)][byteNumber] = newBlock[byteNumber];
        }
    }
    for(size_t block = *blockQty + 1; block < (*blockQty) + ((*blockQty)%CHUNK); block++){
        free(blocks[block]);
    }
    blocks = realloc(blocks, (*blockQty) * sizeof(*blocks));
    return blocks;
}

byte_t evaluatePolynomial(byte_t * polynomial, size_t maxDegree, byte_t x){
    byte_t ans = 0;
    for (size_t degree = 0; degree < maxDegree; degree++) {
        ans = sum(ans, multiply(polynomial[degree], power(x, degree)));
    }
    return ans;
}

int contains(byte_t ** bytes, byte_t byte, size_t blockNumber, size_t filesQty){
    for (size_t fileNumber = 0; fileNumber < filesQty; fileNumber++)
    {
        if(bytes[fileNumber][blockNumber] == byte){
            return 1;
        }
    }
    return 0;
}
