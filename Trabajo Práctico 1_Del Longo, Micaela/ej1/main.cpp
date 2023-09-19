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

#include <string>
#include <iostream>
#include <chrono>
#include "ln_without_threads.cpp"
#include "ln_with_threads.cpp"

using namespace std;

int main() {
    long double x = 1500000.0;  // Valor predeterminado para x
    string inputX;

    long double _2n_mas_1;

    int num_threads = 4; 
    string input_num_threads;

    cout << "Ingrese el operando del logaritmo natural. Presione Enter para usar el valor predeterminado " << setprecision(15) << x << ":";
    getline(cin, inputX);
    if (!inputX.empty()) { //si no es enter 
        x = stold(inputX); //asigna el input a x
    }

    cout << "Ingrese el número de hilos a emplear. Presione Enter para usar el valor predeterminado " << num_threads << ":";
    getline(cin, input_num_threads);
    if (!input_num_threads.empty()) { //si no es enter 
        num_threads = stoi(input_num_threads); //asigna el input a num_threads
    }

    //------------------------------------------------------------------------------------------
    cout << "\nCalculando...\n" << endl;

    auto start_time1 = chrono::high_resolution_clock::now(); //empieza el clock
    
    long double result1 = cal_series(x);

    auto end_time1 = chrono::high_resolution_clock::now();
    auto duration1 = chrono::duration_cast<chrono::milliseconds>(end_time1 - start_time1);

    cout << "Sin Hilos:" << endl; 
    cout << " - Resultado: " << setprecision(15) << result1 << endl;
    cout << " - Tiempo de ejecución: " << duration1.count() << " ms\n" << endl;

    //------------------------------------------------------------------------------------------
    cout << "Con Hilos:" << endl;

    auto start_time2 = chrono::high_resolution_clock::now(); //empieza el clock
    
    long double result2 = cal_series_threads(num_threads, x);

    auto end_time2 = chrono::high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<chrono::milliseconds>(end_time2 - start_time2);

    cout << " - Resultado: " << setprecision(15) << result2 << endl;
    cout << " - Tiempo de ejecución: " << duration2.count() << " ms\n" << endl;
    
    //------------------------------------------------------------------------------------------
    double speed_up = ((double) duration1.count())/duration2.count(); //solo en 1 porque automáticamente castea el de abajo
    cout << "Speed up: " << speed_up << endl;
    return 0;
}