#include <stdio.h>
#include <stdlib.h>
#include "headers/utils.h"

#define MAX_DEGREE 8 // GF(2^8)
#define GX 355 // x8 + x6 + x5 + x1 +1 (polinomio primitivo para el GF(2^8))
#define ELEMS 256 // 2^8


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
    if(a == 0){
        return 0;
    }

    for (size_t inverse = 0; inverse < ELEMS; inverse++)
    {
        if(multiply(a, inverse) == 1){
            return inverse; 
        }
    }
    perror("Inverse not found");
    return 0;
}
