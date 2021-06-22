TP-Criptografia-y-Seguridad
Grupo 6


COMPILACIÓN
1) En la carpeta root, ejecutar 
    make all
Esto generará el ejecutable "ss"


CODIFICACIÓN
Ejemplo: 
    ./ss d imagenes/secreto.bmp 6 portadoras/
. "d" significa que se va a distribuir el secreto hacia las imágenes portadoras.
. "imagenes/secreto.bmp" es algún path hacia una imagen con extensión .bmp. Esta imagen será el secreto cuyos pixeles se distribuirán en las portadoras.
. "6" es el número k que se usará para el esquema de Shamir (k,n). Este número k puede ser solamente 4, 5 ó 6.
. "portadoras/" es el path hacia un directorio que debe contener al menos k imágenes con extensión .bmp. Estas k imágenes serán las portadoras en esta codificación.


DECODIFICACIÓN
Ejemplo:
     ./ss r secreto.bmp 6 portadoras/
. "r" significa que se va a recuperar el secreto de las imágenes portadoras.
. "secreto.bmp" será el nombre del secreto que se recuperará con este comando. Es decir, se creará una nueva imagen "secreto.bmp".
. "6" es el número k que se usará para el esquema de Shamir (k,n). Este número k puede ser solamente 4, 5 ó 6.
. "portadoras/" es el path hacia un directorio que debe contener al menos k imágenes con extensión .bmp. Estas k imágenes son las que contienen el secreto a recuperar con este comando