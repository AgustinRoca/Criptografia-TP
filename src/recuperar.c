#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "headers/recuperar.h"
#include "headers/utils.h"
#include "headers/galois2_8.h"



void recuperar(const char * nombreImagenSecreta, int k, const char *nombreDirectorio) {
    FILE ** files = getFilesInDirectory(nombreDirectorio, "r");
    size_t filesQty = numberOfFilesInDirectory(nombreDirectorio);
    printf("%zu imagenes de camuflaje abiertas\n", filesQty);
    
    size_t width;
    size_t height;
    goToPixelStream(files[0], &width, &height);
        
    size_t blockQty = width * height / k;
    byte_t xFxPairs[filesQty][blockQty][2];
    for (size_t camuflageFile = 0; camuflageFile < filesQty; camuflageFile++) {
        byte_t ** camuflageBlocks = getTopLeftBlocks(files[camuflageFile], blockQty);
        for (size_t blockNumber = 0; blockNumber < blockQty; blockNumber++)
        {
            byte_t x = camuflageBlocks[blockNumber][0];
            byte_t fx = (camuflageBlocks[blockNumber][1] & sToBinary("00000111")) << 5;
            fx += (camuflageBlocks[blockNumber][2] & sToBinary("00000111")) << 2;
            fx += (camuflageBlocks[blockNumber][3] & sToBinary("00000011"));
            if(parity(fx) == ((camuflageBlocks[blockNumber][3] & sToBinary("00000100")) >> 2)){
                xFxPairs[camuflageFile][blockNumber][0] = x;
                xFxPairs[camuflageFile][blockNumber][1] = fx;
            }
        }        
    }
    printf("Pares (x, F(x)) obtenidos\n");

    byte_t s[blockQty][k];
    for (size_t j = 0; j < blockQty; j++){
        // Calculo s1
        s[j][0] = 0;
        for (size_t i = 0; i < filesQty; i++) {
            byte_t y = xFxPairs[i][j][1];
            byte_t productoria = 1;
            for (size_t q = 0; q < filesQty; q++) {
                if(i != q){
                    productoria = multiply(productoria, multiply(xFxPairs[q][j][0], inverse(sum(xFxPairs[i][j][0], xFxPairs[q][j][0]))));
                }
            }
            s[j][0] = sum(s[j][0], multiply(y, productoria));
        }

        // Calculo sr
        for (int r = 1; r < k; r++)
        {
            s[j][r] = 0;
            for (int i = 0; i < k - r; i++)
            {            
                byte_t yprima = multiply(sum(xFxPairs[i][j][1], s[j][0]), inverse(xFxPairs[i][j][0]));
                byte_t productoria = 1;
                for (int q = 0; q < k - r; q++) {
                    if(i != q){
                        productoria = multiply(productoria, multiply(xFxPairs[q][j][0], inverse(sum(xFxPairs[i][j][0], xFxPairs[q][j][0]))));
                    }
                }
                s[j][r] = sum(s[j][r], multiply(yprima, productoria));
            }
        }
    }
    printf("Bytes obtenidos\n");
    
    FILE * secret = fopen(nombreImagenSecreta, "w");
    if(secret == NULL)
    {
        perror("fopen");
    }

    fseek(files[0], 0, SEEK_SET);
    int c = fgetc(files[0]);
    while (c != EOF)
    {
        fputc(c, secret);
        c = fgetc(files[0]);
    }
    goToPixelStream(secret, &width, &height);
    for (size_t j = 0; j < blockQty; j++){
        for (int r = 0; r < k; r++){
            fputc(s[j][r], secret);
        }
    }
    printf("Imagen secreta construida\n");

    fclose(secret);
    closeFiles(files, filesQty);
}

