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

#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>
#include <algorithm>  // sort y min

using namespace std;

// función para verificar si un número es primo
bool es_primo(long long int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
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
    auto sqrt_N = (long long int) sqrt(N);

    vector<long long int> prime_candidates;
    prime_candidates.push_back(2);
    for (long long int i = 3; i <= sqrt_N; i += 2) {
        prime_candidates.push_back(i);
    }

    // print prime_candidates
    // for (long long int i = 0; i < prime_candidates.size(); i++) {
    //     cout << prime_candidates[i] << " ";
    // }
    // cout << endl;

    vector<long long int> local_primes;

    int local_size = prime_candidates.size() / size;
    int remainder = prime_candidates.size() % size;
    int local_start = rank * local_size + min(rank, remainder);
    int local_end = local_start + local_size + (rank < remainder ? 1 : 0);

    for (int i = local_start; i < local_end; i++) {
        if (es_primo(prime_candidates[i])) {
            local_primes.push_back(prime_candidates[i]);
        }
    }

    // print local_primes
//    for (long long int i = 0; i < local_primes.size(); i++) {
//        cout << local_primes[i] << " ";
//    }
//    cout << endl;

    int local_prime_count = local_primes.size();

    // Vector para almacenar la cantidad de números primos encontrados en cada proceso
    vector<int> prime_counts(size);

    // Reunir el número de primos encontrados en cada proceso
    MPI_Allgatherv(&local_prime_count, 1, MPI_INT, prime_counts.data(), 1, MPI_INT, MPI_COMM_WORLD);

    // Calcular la cantidad total de números primos encontrados
    int total_prime_count = 0;
    for (int count : prime_counts) {
        total_prime_count += count;
        //cout << total_prime_count << endl;
    }

    // Vector para almacenar todos los números primos encontrados
    vector<long long int> all_primes(total_prime_count);

    // Reunir los números primos encontrados en cada proceso
    MPI_Allgatherv(local_primes.data(), local_prime_count, MPI_LONG_LONG_INT, all_primes.data(), prime_counts.data(), prime_counts.data(), MPI_LONG_LONG_INT, MPI_COMM_WORLD);

    for (long long int prime_number : all_primes) {
        cout << prime_number << " ";
    }
    cout << endl;

    // wait until all processes have finished
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        sort(all_primes.begin(), all_primes.end());
        cout << "Los números primos menores a la raíz de N (" << sqrt_N << ") son: ";
        for (long long prime_number : all_primes) {
            cout << prime_number << " ";
        }
        cout << endl;

        sort(all_primes.rbegin(), all_primes.rend());
        long long int top_primes = min(10, total_prime_count);
        cout << "Los 10 números primos más grandes son: ";
        for (long long int i = 0; i < top_primes; i++) {
            cout << all_primes[i] << " ";
        }
        cout << endl;

        cout << "Cantidad total de números primos menores que " << N << ": " << total_prime_count << endl;
    }

    MPI_Finalize();
    return 0;
}
