//
// Created by micaela on 11/10/2023.
//

// Escriba un programa que busque todos los números primos menores a un número N
// que se ingresará por teclado. Debe mostrar por pantalla los 10 mayores números
// primos y la cantidad de números primos menores que N. Utilice como datos números
// del tipo long long int si trabaja con C++. Con Python no es necesario indicar el tipo
// de variable.
// El problema debe resolverse de modo que el usuario pueda elegir cualquier número
// de procesos. Cada proceso debe resolver una parte del problema.
// a) Incluya código que permita obtener el tiempo de ejecución en cada programa, y
// calcule el speedup.
// b) Observe el porcentaje de uso de cada núcleo en cada implementación.

// Ayuda:
// Hay 78498 números primos menores que 10^6 (un millón), siendo los 5 mayores:
// 999953, 999959, 999961, 999979, 999983.
// Hay 664579 números primos menores que 10^7 (diez millones), siendo los 5 mayores:
// 9999937, 9999943, 9999971, 9999973, 9999991.

#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>
#include <algorithm>  // sort y min
#include <chrono>

using namespace std;

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


// función para reordenar un vector de modo que los elementos en las posiciones impares estén en orden descendente
// para lograr una carga balanceada de trabajo entre los procesos
// ejemplo: 11 97 15 93 19 89 23 85 27 81 31 77 35 73 39 69 43 65 47 61 51 57 55 53 59 49 63 45 67 41 71 37 75 33 79 29 83 25 87 21 91 17 95 13 99


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


result calc_primes(int size, int rank, vector<long long int> &candidates, const string& type="is_prime",
                   const vector<long long int>& less=vector<long long int>()) {
    vector<int> counts(size);
    vector<int> displs(size);
    int elements_per_process = (int) candidates.size() / size;
    int remainder_candidates = (int) candidates.size() % size;
    int start = 0;
    for (int i = 0; i < size; ++i) {
        counts[i] = elements_per_process +
                    (i < remainder_candidates ? 1 : 0);  // (i < remainder_candidates ? 1 : 0) es 1 si i < remainder_candidates, 0 si no
        displs[i] = start;
        start += counts[i];
    }

    vector<long long int> local;
    for (int i = displs[rank]; i < displs[rank] + counts[rank]; ++i) {
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

    int local_count = (int) local.size();

    vector<int> local_counts(size);
    // MPI_Allgather toma el valor de local_count de cada proceso y lo guarda en local_counts de todos los procesos
    MPI_Allgather(&local_count, 1, MPI_INT, local_counts.data(), 1, MPI_INT, MPI_COMM_WORLD);

    vector<int> primes_displs(size);
    start = 0;
    for (int i = 0; i < size; ++i) {
        primes_displs[i] = start;
        start += local_counts[i];
    }

    int total_count = primes_displs[size - 1] + local_counts[size - 1];  // suma de todos los elementos de local_counts
    vector<long long int> primes(total_count);

    // MPI_Allgatherv toma el valor de local de cada proceso y lo guarda en primes de todos los procesos
    MPI_Allgatherv(local.data(), local_count, MPI_LONG_LONG_INT,
                   primes.data(), local_counts.data(), primes_displs.data(), MPI_LONG_LONG_INT, MPI_COMM_WORLD);

    sort(primes.begin(), primes.end(), greater<>());

    return result{primes, total_count};
}


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        cout << "Debe ingresar un número N como argumento." << endl;
        return 1;
    }
    long long int N = stoll(argv[1]);
    if (N <= 1) {
        cout << "Debe ingresar un número N mayor que 1." << endl;
        return 1;
    }

    auto start_time = chrono::high_resolution_clock::now();
    // cálculo de candidatos -------------------------------------------------------------------------------------------
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
    reorder(less_than_sqrt_N);
    reorder(greater_than_sqrt_N);


    // cálculo de primos hasta sqrt(N) ---------------------------------------------------------------------------------

    result result_less = calc_primes(size, rank, less_than_sqrt_N);
    vector<long long int> lesser_primes = result_less.primes;
    int total_lesser_count = result_less.count;

    // cálculo de primos mayores que sqrt(N) ---------------------------------------------------------------------------

    result result_greater = calc_primes(size, rank, greater_than_sqrt_N, "pseudo_sieve", lesser_primes);
    vector<long long int> greater_primes = result_greater.primes;
    int total_greater_count = result_greater.count;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    // impresión de resultados -----------------------------------------------------------------------------------------
    if (rank == 0){
        sort(greater_primes.begin(), greater_primes.end(), greater<> ());

        long long int total_count = total_lesser_count + total_greater_count;
        vector<long long int> total_primes(total_count);
        merge(lesser_primes.begin(), lesser_primes.end(), greater_primes.begin(), greater_primes.end(), total_primes.begin(), greater<>());

        cout << "Primos menores que " << N << ": ";
        for (int i = 0; i < min(10, total_lesser_count + total_greater_count); ++i) {
            cout << total_primes[i] << " ";
        }
        cout << "... " << endl;
        cout << "Cantidad de primos menores que " << N << ": " << total_lesser_count + total_greater_count << endl;
        cout << "Tiempo de ejecución: " << duration.count() << " ms." << endl;
        cout << "Cantidad de procesos: " << size << endl;

    }

    MPI_Finalize();
    return 0;
}
