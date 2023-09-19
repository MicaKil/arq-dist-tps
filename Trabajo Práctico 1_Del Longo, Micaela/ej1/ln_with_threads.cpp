// utilizando multihilo

// Escribir un programa que calcule el logaritmo natural de números mayores a
// 1500000 (1.5*10^6) en punto flotante de doble precisión largo (tipo long double en
// C++) mediante serie de Taylor, empleando 10000000 (diez millones) de términos de
// dicha serie. El resultado debe imprimirse con 15 dígitos (ver anexo). 

// Empleando múltiples hilos que trabajen concurrentemente, resolviendo cada
// hilo una parte de los 10 millones de términos de la serie de Taylor
// (sugerencia, utilizar como número de hilos divisores de 10000000).
// Tanto el operando del logaritmo natural como la cantidad de hilos a emplear deben
// ingresarse por teclado.

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

#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <string>

using namespace std;

volatile long double result = 0.0;

// función para calcular una parte de la serie de Taylor en paralelo
void cal_series_part(long double term, int start, int end) {
    long double _2n_mas_1;
    long double partial_result = 0.0;
    
    for (int i = start; i < end; ++i) {
        _2n_mas_1 = 2*(long double) i + 1;
        partial_result += pow(term, _2n_mas_1)/_2n_mas_1;
        
    }

    result += partial_result;
}

long double cal_series_threads(int num_threads, long double x) {
    vector<thread> threads(num_threads);

    int step = (10000000 / num_threads);
    int start = 0;
    int end = step;

    long double term = ((x - 1)/(x + 1));

    for (int i = 0; i < num_threads; ++i) {
        threads[i] = thread(cal_series_part, term, start, end);
        start = end + 1;
        end += step;
    }

    for (int i = 0; i < num_threads; ++i) {
        if (threads[i].joinable()){
            threads[i].join();
        }
    }
    
    //long double result2 = 2 * result;
    return 2 * result;  // El logaritmo natural es el doble serie
}