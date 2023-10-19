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

using namespace std;

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

bool pseudo_sieve(long long int n, vector<long long int> &primes) {
    return all_of(primes.begin(), primes.end(), [n](long long int prime) {
        return n % prime != 0;
    });
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
    vector<int> counts_lesser(size);
    vector<int> displs_lesser(size);
    int elements_per_process_lesser = (int) less_than_sqrt_N.size() / size;
    int remainder_lesser = (int) less_than_sqrt_N.size() % size;
    int start_lesser = 0;
    for (int i = 0; i < size; ++i) {
        counts_lesser[i] = elements_per_process_lesser +
                           (i < remainder_lesser ? 1 : 0);  // (i < remainder_lesser ? 1 : 0) es 1 si i < remainder_lesser, 0 si no
        displs_lesser[i] = start_lesser;
        start_lesser += counts_lesser[i];
    }

    vector<long long int> local_primes;
    for (int i = displs_lesser[rank]; i < displs_lesser[rank] + counts_lesser[rank]; ++i) {
        if (is_prime(less_than_sqrt_N[i])) {
            local_primes.push_back(less_than_sqrt_N[i]);
        }
    }

    int local_primes_count = (int) local_primes.size();

    vector<int> primes_counts(size);
    // MPI_Allgather toma el valor de local_primes_count de cada proceso y lo guarda en primes_counts de todos los procesos
    MPI_Allgather(&local_primes_count, 1, MPI_INT, primes_counts.data(), 1, MPI_INT, MPI_COMM_WORLD);

    vector<int> primes_displs(size);
    start_lesser = 0;
    for (int i = 0; i < size; ++i) {
        primes_displs[i] = start_lesser;
        start_lesser += primes_counts[i];
    }

    int total_primes_count = primes_displs[size - 1] + primes_counts[size - 1];  // suma de todos los elementos de primes_counts
    vector<long long int> primes(total_primes_count);

    // MPI_Allgatherv toma el valor de local_primes de cada proceso y lo guarda en primes de todos los procesos
    MPI_Allgatherv(local_primes.data(), local_primes_count, MPI_LONG_LONG_INT,
                   primes.data(), primes_counts.data(), primes_displs.data(), MPI_LONG_LONG_INT, MPI_COMM_WORLD);

    sort(primes.begin(), primes.end(), greater<>());

    if (rank == 0) {
        cout << "Primos hasta sqrt(" << N << "):" << endl;
        for (int i = 0; i < min(10, total_primes_count); ++i) {
            cout << primes[i] << " ";
        }
        cout << endl;
    }


    // cálculo de primos mayores que sqrt(N) ---------------------------------------------------------------------------

    vector<int> counts_greater;
    vector<int> displs_greater;
    int elements_per_process_greater = (int) greater_than_sqrt_N.size() / size;
    int remainder_greater = (int) greater_than_sqrt_N.size() % size;
    start_lesser = 0;
    for (int i = 0; i < size; ++i) {
        counts_greater.push_back(elements_per_process_greater + (i < remainder_greater ? 1 : 0));
        displs_greater.push_back(start_lesser);
        start_lesser += counts_greater[i];
    }

    vector<long long int> local_greater;
    for (int i = displs_greater[rank]; i < displs_greater[rank] + counts_greater[rank]; ++i) {
        if (pseudo_sieve(greater_than_sqrt_N[i], primes)) {
            local_greater.push_back(greater_than_sqrt_N[i]);
        }
    }

    int local_greater_count = (int) local_greater.size();
    vector<int> greater_counts(size);
    MPI_Allgather(&local_greater_count, 1, MPI_INT, greater_counts.data(), 1, MPI_INT, MPI_COMM_WORLD);

    vector<int> greater_displs(size);
    start_lesser = 0;
    for (int i = 0; i < size; ++i) {
        greater_displs[i] = start_lesser;
        start_lesser += greater_counts[i];
    }

    int total_greater_count = greater_displs[size - 1] + greater_counts[size - 1];
    vector<long long int> greater_primes(total_greater_count);
    MPI_Allgatherv(local_greater.data(), local_greater_count, MPI_LONG_LONG_INT,
                   greater_primes.data(), greater_counts.data(), greater_displs.data(), MPI_LONG_LONG_INT, MPI_COMM_WORLD);

    sort(greater_primes.begin(), greater_primes.end(), greater<> ());

    if (rank == 0) {
        cout << "Primos mayores que sqrt(" << N << "):" << endl;
        for (int i = 0; i < min(10, total_greater_count); ++i) {
            cout << greater_primes[i] << " ";
        }
        cout << endl;
        cout << "Cantidad de primos menores que " << N << ": " << total_primes_count + total_greater_count << endl;
    }

    MPI_Finalize();
    return 0;
}
