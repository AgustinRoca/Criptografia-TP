#ifndef DISTRIBUIR_H
#define DISTRIBUIR_H

/**
 *
 * @param nombreImagenSecreta: Corresponde al nombre de un archivo de extensión .bmp.
 * @param k: El número corresponde a la cantidad mínima de sombras necesarias para recuperar
             el secreto en un esquema (k, n).
 * @param nombreDirectorio: El directorio donde se encuentran las imágenes en las que se distribuirá el
                            secreto (en el caso de que se haya elegido la opción (d)), o donde están las imágenes que
                            contienen oculto el secreto ( en el caso de que se haya elegido la opción (r))
 */
void distribuir(const char * nombreImagenSecreta, size_t k, const char *nombreDirectorio);


#endif //DISTRIBUIR_H
