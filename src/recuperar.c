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
    byte_t x[filesQty][blockQty];
    byte_t fx[filesQty][blockQty];
    for (size_t camuflageFile = 0; camuflageFile < filesQty; camuflageFile++) {
        size_t discarded = 0;
        byte_t ** camuflageBlocks = getTopLeftBlocks(files[camuflageFile], blockQty);
        for (size_t blockNumber = 0; blockNumber < blockQty; blockNumber++)
        {
            byte_t xInBlock = camuflageBlocks[blockNumber][0];
            byte_t fxInBlock = 0;
            fxInBlock ^= (camuflageBlocks[blockNumber][1] << 5) & sToBinary("11100000");
            fxInBlock ^= (camuflageBlocks[blockNumber][2] << 2) & sToBinary("00011100");
            fxInBlock ^= (camuflageBlocks[blockNumber][3] & sToBinary("00000011"));
            if(parity(fxInBlock) == ((camuflageBlocks[blockNumber][3] >> 2) & sToBinary("00000001"))){
                x[camuflageFile][blockNumber] = xInBlock;
                fx[camuflageFile][blockNumber] = fxInBlock;
            } else {
                printf("El byte armado es: %zu, su paridad leida es: %d, deberia ser: %d\n", fxInBlock, (camuflageBlocks[blockNumber][3] >> 2) & sToBinary("00000001"), parity(fxInBlock));
                discarded++;
            }
        }
        printf("Discarded: %zu of %zu blocks\n", discarded, blockQty);
        free(camuflageBlocks);   
    }
    printf("Pares (x, F(x)) obtenidos\n");

    byte_t s[blockQty][k];
    for (size_t j = 0; j < blockQty; j++){
        // Calculo s1
        s[j][0] = 0;
        for (int i = 0; i < k; i++) {
            byte_t y = fx[i][j];
            byte_t productoria = 1;
            if(y != 0){
                for (int q = 0; q < k; q++) {
                    if(i != q){
                        productoria = multiply(productoria, multiply(x[q][j], inverse(sum(x[i][j], x[q][j]))));
                    }
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
                byte_t yprima = fx[i][j];
                for(int n = 0; n<r; n++){
                    yprima = sum(yprima, s[j][n]);
                    yprima = multiply(yprima, inverse(x[i][j]));
                }
                byte_t productoria = 1;
                if(yprima != 0){
                    for (int q = 0; q < k - r; q++) {
                        if(i != q){
                            productoria = multiply(productoria, multiply(x[q][j], inverse(sum(x[i][j], x[q][j]))));
                        }
                    }
                }
                s[j][r] = sum(s[j][r], multiply(yprima, productoria));
            }
        }
    }
    printf("Bytes obtenidos\n");
    
    FILE * secret = fopen_mkdir(nombreImagenSecreta, "w+");
    if(secret == NULL)
    {
        perror("fopen");
    }

    // duplico algun archivo
    fseek(files[0], 0, SEEK_SET);
    int c = fgetc(files[0]);
    while (c != EOF)
    {
        fputc(c, secret);
        c = fgetc(files[0]);
    }

    // cambio los pixeles del contenido
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

