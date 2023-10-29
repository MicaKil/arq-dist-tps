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
#include <cmath>
#include <mutex>

using namespace std;

mutex mtx_less_than_sqrt_N;
mutex mtx_greater_than_sqrt_N;

struct result {
    vector<long long int> primes;
    int count;
};

// función para verificar si un número es primo
bool is_prime(long long int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

void reorder(vector<long long int> &v) {
    int len_v = (int) v.size();
    for (int i = 1; i < (len_v - 1)/2 ; i+=2) {
        swap(v[i], v[len_v - i - 1]);
    }
}

bool pseudo_sieve(long long int n, vector<long long int> primes) {
    return all_of(primes.begin(), primes.end(), [n](long long int prime) {
        return n % prime != 0;
    });
}

// calcula los números primos en el rango [start, end) de un vector de candidatos
// is_prime: calcula los números primos menores que sqrt(N)
// pseudo_sieve: calcula los números primos mayores que sqrt(N) con los números primos menores que sqrt(N) calculados previamente
result calc_primes(int start, int end, vector<long long int> &candidates, const string &type = "is_prime",
                   const vector<long long int> &less = vector<long long int>()) {
    vector<long long int> local;
    for (int i = start; i < end; i++) {
        if (type == "is_prime") {
            if (is_prime(candidates[i])) {
                local.push_back(candidates[i]);
            }
        } else if (type == "pseudo_sieve") {
            if (pseudo_sieve(candidates[i], less)) {
                local.push_back(candidates[i]);
            }
        }
    }

    int local_count = (int) (local.size());
    return result{local, local_count};
}


int main() {
    // entrada de datos -----------------------------------------------------------------------
    long long int N = 1000000; // valor predeterminado de N
    string input_N;
    cout << "Ingrese el valor de N: Presione Enter para usar el valor predeterminado " << setprecision(15) << N << ":";
    getline(cin, input_N);
    if (!input_N.empty()) { //si no es enter...
        N = stoll(input_N);
    }

    int num_threads = (int) thread::hardware_concurrency(); // obtiene el número de núcleos de CPU disponibles
    string input_num_threads;
    cout << "Ingrese el número de hilos a usar: Presione Enter para usar el valor predeterminado " << num_threads << ":";
    getline(cin, input_num_threads);
    if (!input_num_threads.empty()) { //si no es enter...
        num_threads = stoi(input_num_threads);
    }

    // cálculo de candidatos -------------------------------------------------------------------
    auto sqrt_N = (long long int) sqrt(N);

    vector<long long int> less_than_sqrt_N;
    vector<long long int> greater_than_sqrt_N;
    
    less_than_sqrt_N.push_back(2);  // 2 es el único número primo par
    // candidatos a primos son todos los números impares entre 3 y N
    for (long long int i = 3; i <= sqrt_N; i += 2) {
        less_than_sqrt_N.push_back(i);
    }
    for (long long int i = sqrt_N + 1; i <= N; i += 2) {
        greater_than_sqrt_N.push_back(i);
    }

    // copia de los vectores de candidatos para usarlos con hilos después
    vector<long long int> threads_less_than_sqrt_N(less_than_sqrt_N.size());
    copy(less_than_sqrt_N.begin(), less_than_sqrt_N.end(), threads_less_than_sqrt_N.begin());

    vector<long long int> threads_greater_than_sqrt_N(greater_than_sqrt_N.size());
    copy(greater_than_sqrt_N.begin(), greater_than_sqrt_N.end(), threads_greater_than_sqrt_N.begin());

    //------------------------------------------------------------------------------------------
    // sin hilos
    
    cout << "Sin Hilos:" << endl;
    cout << "Calculando..." << endl;

    auto start_time = chrono::high_resolution_clock::now();

    // cálculo de primos hasta sqrt(N) 
    result result_less = calc_primes(0, (int) less_than_sqrt_N.size(), less_than_sqrt_N);
    vector<long long int> lesser_primes = result_less.primes;
    int total_lesser_count = result_less.count;

    // cálculo de primos mayores que sqrt(N)
    result result_greater = calc_primes(0, (int) greater_than_sqrt_N.size(), greater_than_sqrt_N, "pseudo_sieve", lesser_primes);
    vector<long long int> greater_primes = result_greater.primes;
    int total_greater_count = result_greater.count;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    int total_count_no_threads = total_lesser_count + total_greater_count;
    vector<long long int> total_primes_no_threads(total_count_no_threads);
    sort(lesser_primes.begin(), lesser_primes.end(), greater<long long int>());
    sort(greater_primes.begin(), greater_primes.end(), greater<long long int>());
    merge(lesser_primes.begin(), lesser_primes.end(),
          greater_primes.begin(), greater_primes.end(),
          total_primes_no_threads.begin(), greater<long long int>());

    // impresión de resultados
    cout << "Primos menores que " << N << ": ";
    for (int i = 0; i < min(10, total_count_no_threads); ++i) {
        cout << total_primes_no_threads[i] << " ";
    }
    cout << "... " << endl;

    cout << "Cantidad de primos menores que " << N << ": " << total_count_no_threads << endl;
    cout << "Tiempo de ejecución: " << duration.count() << " ms." << endl;

    //------------------------------------------------------------------------------------------
    // con hilos

    cout << "Con Hilos:" << endl;
    cout << "Calculando..." << endl;
    
    // reordenar los vectores para que los hilos tengan un trabajo más equitativo
    reorder(threads_less_than_sqrt_N);
    reorder(threads_greater_than_sqrt_N);

    start_time = chrono::high_resolution_clock::now();

    vector<thread> threads;
    vector<result> results(num_threads);  // vector de resultados de cada hilo
    vector<long long int> threads_lesser_primes;  // vector de primos menores que sqrt(N) para usar en el cálculo de primos mayores que sqrt(N)
    int total_lesser_count_threads = 0;

    // cálculo de primos hasta sqrt(N)
    for (int i = 0; i < num_threads; i++) {
        int start = i * ((int) threads_less_than_sqrt_N.size() / num_threads);
        int end = (i == num_threads - 1) ? (int) threads_less_than_sqrt_N.size() : (i + 1) * ((int) threads_less_than_sqrt_N.size() / num_threads); // si es el último hilo, le da el resto de los números
        threads.emplace_back([&results, i, start, end, &threads_less_than_sqrt_N, &threads_lesser_primes, &total_lesser_count_threads]() {  // emplace_back: agrega un elemento al final del vector
            result r = calc_primes(start, end, threads_less_than_sqrt_N);
            lock_guard<mutex> lock(mtx_less_than_sqrt_N);
            results[i] = r;
            threads_lesser_primes.insert(threads_lesser_primes.end(), r.primes.begin(), r.primes.end());  // inserta los primos encontrados
            total_lesser_count_threads += r.count;
        });
    }

    for (int i = 0; i < num_threads; i++) {
        threads[i].join();  // espera a que todos los hilos terminen
    }

    // cálculo de primos mayores que sqrt(N)
    results.clear();  // limpia el vector de resultados para usarlo de nuevo
    threads.clear();  // idem con el vector de hilos
    vector<long long int> threads_greater_primes;
    int total_greater_count_threads = 0;

    for (int i = 0; i < num_threads; i++) {
        int start = i * ((int) threads_greater_than_sqrt_N.size() / num_threads);
        int end = (i == num_threads - 1) ? (int) threads_greater_than_sqrt_N.size() : (i + 1) * ((int) threads_greater_than_sqrt_N.size() / num_threads);
        threads.emplace_back([&results, i, start, end, &threads_greater_than_sqrt_N, &threads_lesser_primes, &threads_greater_primes, &total_greater_count_threads]() {
            result r = calc_primes(start, end, threads_greater_than_sqrt_N, "pseudo_sieve", threads_lesser_primes);
            lock_guard<mutex> lock(mtx_greater_than_sqrt_N);
            results[i] = r;
            threads_greater_primes.insert(threads_greater_primes.end(), r.primes.begin(), r.primes.end());
            total_greater_count_threads += r.count;
        });
    }

    for (int i = 0; i < num_threads; i++) {
        threads[i].join();
    }

    // unión de resultados
    sort(threads_lesser_primes.begin(), threads_lesser_primes.end(), greater<long long int>());
    sort(threads_greater_primes.begin(), threads_greater_primes.end(), greater<long long int>());

    vector<long long int> total_primes;
    int total_count_threads = total_lesser_count_threads + total_greater_count_threads;
    total_primes.reserve(total_count_threads);

    merge(threads_lesser_primes.begin(), threads_lesser_primes.end(),
          threads_greater_primes.begin(), threads_greater_primes.end(),
          back_inserter(total_primes), greater<long long int>());

    end_time = chrono::high_resolution_clock::now();
    auto threads_duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    cout << "Primos menores que " << N << ": ";
    for (int i = 0; i < min(10, (int) (total_count_threads)); ++i) {
        cout << total_primes[i] << " ";
    }
    cout << "... " << endl;
    cout << "Cantidad de primos menores que " << N << ": " << total_count_threads << endl;
    cout << "Tiempo de ejecución: " << threads_duration.count() << " ms." << endl;

    double speed_up = ((double) duration.count())/(double) threads_duration.count(); //solo en 1 porque automáticamente castea el de abajo
    cout << "\nSpeed up: " << speed_up << endl;
    return 0;
}