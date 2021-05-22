#include <stdio.h>
#include <stdlib.h>
#include "headers/utils.h"

#define MAX_DEGREE 8 // GF(2^8)
#define GX 355 // x8 + x6 + x5 + x1 +1 (polinomio primitivo para el GF(2^8))
#define ELEMS 256 // 2^8
#define MAX_DIGITS 3 // 255

int inverseTable[ELEMS] = 
        {0,1,177,222,233,74,111,140,197,165,
        37,193,134,84,70,231,211,93,227,133,
        163,52,209,237,67,166,42,99,35,158,
        194,119,216,45,159,28,192,10,243,171,
        224,181,26,98,217,33,199,189,144,86,
        83,234,21,162,128,121,160,91,79,229,
        97,73,138,205,108,154,167,24,254,124,
        14,230,96,61,5,232,200,130,228,58,
        112,106,235,50,13,135,49,145,221,246,
        161,57,210,17,239,191,72,60,43,27,
        152,213,117,151,187,184,81,113,64,155,
        141,6,80,107,156,142,150,102,195,31,
        129,55,149,178,69,255,215,136,54,120,
        77,201,226,19,12,85,127,214,62,204,
        7,110,115,157,48,87,175,248,179,122,
        116,103,100,212,65,109,114,143,29,34,
        56,90,53,20,196,9,25,66,183,172,
        242,39,169,182,249,146,223,2,123,148,
        225,41,173,168,105,186,185,104,198,47,
        238,95,36,11,30,118,164,8,188,46,
        76,131,219,253,139,63,250,245,236,22,
        92,16,153,101,137,126,32,44,252,202,
        247,88,3,176,40,180,132,18,78,59,
        71,15,75,4,51,82,208,23,190,94,
        241,240,170,38,251,207,89,220,147,174,
        206,244,218,203,68,125};

size_t getDegree(int accum);

int sum(int a, int b){
    return a^b;
}

int multiply(int a, int b){
    // Multiplicación
    int answer = 0;
    for(size_t degree = 0, base = 1; degree <= MAX_DEGREE; degree++, base<<=1){
        if((b & base) != 0){
            answer ^= (a << degree); // ans += a * x^degree
        }
    }

    // Reduccion modulo g(x)
    for (int degree = getDegree(answer); degree >= MAX_DEGREE; degree = getDegree(answer)) {
        int factor = GX << (degree - MAX_DEGREE); // factor = g(x) * x^(resta de grados)
        answer ^= factor; // ans -= factor   
    }
    return answer;
}

int power(int polynomial, size_t exponent){
    if(exponent == 0)
        return 1;
    int ans = polynomial;
    for (size_t i = 0; i < exponent; i++)
    {
        ans = multiply(ans, polynomial);
    }
    return ans;
    
}

size_t getDegree(int accum){
    if(accum == 0)
        return 0;
    
    int degree = 0;
    while (accum != 0)
    {
        accum >>= 1;
        degree++;
    }
    return degree - 1;
}

int inverse(int a){
    return inverseTable[a];
}
