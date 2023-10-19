// Escriba un programa que busque todos los números primos menores a un número N
// que se ingresará por teclado. Debe mostrar por pantalla los 10 mayores números
// primos y la cantidad de números primos menores que N. Utilice como datos
// números del tipo long long int.
// Resuelva el problema de dos formas:
// ● Sin emplear multihilos.
// ● Empleando múltiples hilos que trabajen concurrentemente.
// El valor de N deberá ser de al menos 10^7.
// a) Incluya código que permita obtener el tiempo de ejecución en cada programa, y
// calcule el speedup (ver Anexo 1.4).
// b) Observe el porcentaje de uso de cada núcleo en cada implementación (ver Anexo
// 1.6).
// Ayuda:
// Hay 78498 números primos menores que 10^6 (un millón), siendo los 5 mayores:
// 999953, 999959, 999961, 999979, 999983.
// Hay 664579 números primos menores que 10^7 (diez millones), siendo los 5
// mayores: 9999937, 9999943, 9999971, 9999973, 9999991.

// Nota 1: Resolver este problema requiere conocimientos de aritmética, uso de
// vectores de C++ y mecanismos de exclusión mutua. En el anexo 3 se brinda
// información para resolverlo.

// Nota 2: Este problema requiere un poder de cómputo muy grande si los números
// son mayores a 10^9 o 10^10 Con (C++ puede utilizar variables con tamaños hasta
// 1.1×104932), siendo el tipo de problemas que requiere grandes clusters para ser
// resuelto. La dificultad para resolver este problema es la base del algoritmo de
// encriptación RSA, el más seguro conocido hasta hoy día.

// Anexo 3: Repaso de aritmética de los números primos

// Un número primo es aquel que solo es divisible por 1 y por si mismo. Todo número
// puede factorizarse en números primos (a este enunciado se lo llama teorema
// fundamental del álgebra). Por ejemplo, los factores de 20 son 2, 2 y 5, ya que
// 20=2*2*5. Por lo tanto, 20 no es un número primo.
// Dificultades para resolver el ejercicio 4:
// No existe un algoritmo simple y rápido para detectar si un número es primo. La
// única forma forma de saber si un número K es primo, es dividirlo por todos los
// números primos menores a K^1/2 , y verificar que el resto de la división sea diferente
// de cero para todas las divisiones.

// Un algoritmo simple para hallar los números primos:
// La dificultad de hallar todos los números primos menores que N mediante
// paralelismo, es que cada hilo (o proceso) debe ir buscando números primos y
// escribirlos en una lista o vector de números primos que irá creciendo, pero esa lista
// lista será usada por cada uno de los hilos para buscar otros números primos. Esto
// puede conducir a que un hilo trabaje con datos incompletos. Tampoco puede
// imponer que un hilo no comience su tarea hasta que otro finalice, porque no estaría
// aprovechando el paralelismo.
// Encontrar un algoritmo eficiente y confiable que resuelva este problema es un
// problema abierto hoy día.
// Para este problema propondremos un algoritmo simple que sigue tres pasos:
// 1) Un solo hilo (puede ser el hilo principal del programa) busca los números primos
// menores a N^1/2.
// 2) Creamos y ejecutamos los hilos que trabajarán en paralelo. Cada hilo busca
// números primos en una parte del intervalo de números naturales entre N1/2 y N.
// 3) Ordenamos el vector resultante. Como los hilos buscan números primos de forma
// concurrente, los números escritos en el vector de números primos probablemente
// no estén ordenados.

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <mutex>
#include <atomic>
#include "primes_without_threads.cpp" 

using namespace std;

mutex mtx; // mutex para garantizar la exclusión mutua al modificar el vector de primos
vector<long long int> primes; // vector para almacenar los números primos encontrados
atomic<long long int> current(2); // variable atómica para mantener el valor actual a ser comprobado

void findPrimes(long long int N) {
    while (true) {
        long long int num = current++; // atomically incrementa y obtiene el valor actual
        if (num >= N) break; // si supera N, salimos del bucle
        if (isPrime(num)) { // comprueba si num es primo
            lock_guard<mutex> lock(mtx); // bloquea el mutex para modificar el vector de primos
            primes.push_back(num); // agrega el número primo al vector
        }
    }
}

int main() {
    long long int N = 1000000; // valor predeterminado de N
    string input_N;
    cout << "Ingrese el valor de N: Presione Enter para usar el valor predeterminado " << setprecision(15) << N << ":";
    getline(cin, input_N);
    if (!input_N.empty()) { //si no es enter...
        N = stoll(input_N);
    }

    //------------------------------------------------------------------------------------------
    // Sin hilos
    cout << "Sin Hilos:" << endl;
    cout << "Calculando..." << endl;
    chrono::duration<double> duration = primes_without_threads(N); 

    //------------------------------------------------------------------------------------------
    // Con hilos

    auto start_time = chrono::high_resolution_clock::now(); 

    int num_threads = (int) thread::hardware_concurrency(); // obtiene el número de núcleos de CPU disponibles
    vector<thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(thread(findPrimes, N)); // crea y lanza hilos que buscan primos concurrentemente
    }

    for (auto& thread : threads) {
        thread.join(); // espera a que todos los hilos terminen
    }

    auto end_time = chrono::high_resolution_clock::now(); 
    chrono::duration<double> duration_threads = end_time - start_time; 

    //------------------------------------------------------------------------------------------
    sort(primes.begin(), primes.end()); // ordena los números primos encontrados

    cout << "Cantidad de números primos menores que N: " << primes.size() << endl;

    int num_primes_to_display = min(10, static_cast<int>(primes.size()));
    cout << "Los 10 mayores números primos: ";
    for (int i = (int) primes.size() - 1; i >= primes.size() - num_primes_to_display; --i) {
        cout << primes[i] << " ";
    }
    cout << endl;

    cout << "Cantidad de núcleos usados: " << num_threads << endl;
    cout << "Tiempo de ejecución: " << duration_threads.count() << " segundos." << endl;
    
    //------------------------------------------------------------------------------------------
    double speed_up = duration.count() / duration_threads.count(); 
    cout << "\nSpeed up: " << speed_up << endl; 

    return 0;
}