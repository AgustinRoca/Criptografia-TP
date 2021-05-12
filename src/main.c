#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "headers/utils.h"
#include "headers/distribuir.h"
#include "headers/recuperar.h"

// Prototypes
static void show_help();


int main(int argc, const char *argv[]) {
    if(argc == 2) {
        if(strcmp(argv[1],"--help") == 0) {
            show_help();
            return 0;
        } else {
            printf("Se esperan 4 parámetros.\nVease la opción --help para ver cómo invocar al programa.\n");
            return -1;
        }
    }
    if(argc != 5) {
        printf("Se esperan 4 parámetros.\nVease la opción --help para ver cómo invocar al programa.\n");
        return -1;
    }
    // Tenemos los 4 parámetros
    const int k = atoi(argv[3]);

    // Verificamos que el archivo sea de extensión .bmp
    if(!string_ends_with(argv[2], ".bmp")) {
        printf("El nombre del archivo debe ser de formato \"nombre.bmp\"\n");
        return -1;
    }

    // Verificamos que el directorio exista
    char directory_path[256];
    if (getcwd(directory_path, sizeof(directory_path)) == NULL) {
        perror("getcwd()");
        return -1;
    }
    struct stat sb;
    strcat(directory_path, "/");
    strcat(directory_path, argv[4]);
    if (stat(directory_path, &sb) != 0 || !S_ISDIR(sb.st_mode)) {
        printf("El directorio provisto debe existir\n");
        return -1;
    }

    // Verificamos que el directorio contenga k archivos
    if(k != number_of_files_in_directory(directory_path)) {
        printf("El directorio \"%s\" debe tener al menos %d archivos\n", argv[4], k);
        return -1;
    }

    // Verificamos que todos los archivos sean de extensión .bmp
    if(!all_files_are_bmp(directory_path)) {
        printf("Error: todos los archivos del directorio \"%s\" deben ser del tipo .bmp\n", argv[4]);
        return -1;
    }

    // Ejecutamos distribuir() o recuperar() segun corresponda
    if(strcmp(argv[1], "d") == 0) {
        distribuir(argv[2], atoi(argv[3]), argv[4]);
    } else if(strcmp(argv[1], "r") == 0) {
        recuperar(argv[2], atoi(argv[3]), argv[4]);
    } else {
        printf("Mal uso del programa.\nVease la opción --help para ver cómo invocar al programa.\n");
        return -1;
    }

    return 0;
}

static void show_help() {
    printf("\nEl programa debe recibir como parámetros (EN EL SIGUIENTE ORDEN):\n\n");
    printf("\t. d : indica que se va a distribuir una imagen secreta en otras imágenes\n\n");
    printf("\t. r : indica que se va a recuperar una imagen secreta a partir de otras imágenes\n\n");
    printf("\t\tNOTA: los parámetros d y r no pueden ingresarse al mismo tiempo\n\n");
    printf("\t. imagenSecreta : Corresponde al nombre de un archivo de extensión .bmp. En el caso de que se haya elegido la opción (d) este archivo debe existir ya que es la imagen a ocultar y debe ser una imagen en blanco y negro (8 bits por pixel) Si se eligió la opción (r) este archivo será el archivo de salida, con la imagen secreta revelada al finalizar el programa.\n\n");
    printf("\t. Número k : El número corresponde a la cantidad mínima de sombras necesarias para recuperar el secreto en un esquema (k, n).\n\n");
    printf("\t. directorio : El directorio donde se encuentran las imágenes en las que se distribuirá el secreto (en el caso de que se haya elegido la opción (d)), o donde están las imágenes que contienen oculto el secreto ( en el caso de que se haya elegido la opción (r)). Debe contener imágenes de extensión .bmp, de 8 bits por pixel, de igual tamaño que la imagen secreta. Además, deberá verificarse que existan por lo menos k imágenes en el directorio.\n\n");
    printf("EJEMPLOS:\n");
    printf("1) Distribuir la imagen “Albert.bmp” según esquema (4,8) guardando las sombras en imágenes del directorio “color280x440”:\n");
    printf("\t./ss  d  Albert.bmp  4  color280x440/\n");
    printf("2) Recuperar la imagen “secreto.bmp”, en un esquema (4,8) buscando imágenes en el directorio “color280x440/”\n");
    printf("\t./ss r secreto.bmp 4 color280x440/\n\n");
}

