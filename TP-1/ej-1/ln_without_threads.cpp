// Sin multihilo 2545

// Escribir un programa que calcule el logaritmo natural de números mayores a
// 1500000 (1.5*10^6) en punto flotante de doble precisión largo (tipo long double en
// C++) mediante serie de Taylor, empleando 10000000 (diez millones) de términos de
// dicha serie. El resultado debe imprimirse con 15 dígitos (ver anexo). 

// el operando del logaritmo natural debe ingresarse por teclado.

// a) Incluya código que permita obtener el tiempo de ejecución en cada programa, y
// calcule el speedup (ver Anexo 1.4).

// b) Observe el porcentaje de uso de cada núcleo en cada implementación (ver Anexo
// 1.6).

// Serie de Taylor del logaritmo natural:
// Ayuda: ln(1500000)=14.2209756660724

// Nota: Se sugiere no mezclar variables de diferentes tipos en una sola operación
// matemática. Algunas variables podrían desbordarse, y algunas arquitecturas
// podrían arrojar resultados según la variable de menor longitud. Puede usar
// máscaras para transformar todas las variables al formato requerido.

#include <cmath>

// func aux para calcular la serie
long double cal_series(long double x) {
    long double _2n_mas_1; 
    long double result = 0.0;
    long double term = ((x - 1)/(x + 1));

    for (int i = 0; i <= 10000000; ++i) {
        _2n_mas_1 = 2* (long double) i + 1;
        result += pow(term, _2n_mas_1)/_2n_mas_1;
    }
    return 2 * result;
}