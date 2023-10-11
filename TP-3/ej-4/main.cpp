//
// Created by micae on 11/10/2023.
//

//Escriba un programa que busque todos los números primos menores a un número N
//que se ingresará por teclado. Debe mostrar por pantalla los 10 mayores números
//primos y la cantidad de números primos menores que N. Utilice como datos números
//del tipo long long int si trabaja con C++. Con Python no es necesario indicar el tipo
//de variable.
//El problema debe resolverse de modo que el usuario pueda elegir cualquier número
//        de procesos. Cada proceso debe resolver una parte del problema.
//a) Incluya código que permita obtener el tiempo de ejecución en cada programa, y
//calcule el speedup.
//b) Observe el porcentaje de uso de cada núcleo en cada implementación.

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <mpi.h>

using namespace std;


bool is_prime(long long int num) {
    if (num <= 1) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    for (long long int i = 3; i <= sqrt(num); i += 2) {
        if (num % i == 0) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        long long int N;
        cout << "Ingrese un número N: ";
        cin >> N;

        // Comparte N con todos los procesos
        for (int i = 1; i < size; i++) {
            MPI_Send(&N, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD);
        }

        // Dividir el rango de búsqueda en partes iguales
        long long int lower_bound = 2 + (rank * (N - 2) / size);
        long long int upper_bound = 2 + ((rank + 1) * (N - 2) / size);

        vector<long long int> primes;
        for (long long int num = lower_bound; num < upper_bound; num++) {
            if (is_prime(num)) {
                primes.push_back(num);
            }
        }

        // Recopilar resultados de todos los procesos
        for (int i = 1; i < size; i++) {
            int count;
            MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vector<long long int> buffer(count);
            MPI_Recv(buffer.data(), count, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            primes.insert(primes.end(), buffer.begin(), buffer.end());
        }

        // Ordenar los números primos encontrados
        sort(primes.begin(), primes.end());

        // Mostrar los 10 números primos más grandes y la cantidad de primos
        int total_primes = primes.size();
        cout << "Los 10 mayores números primos: ";
        for (int i = max(total_primes - 10, 0); i < total_primes; i++) {
            cout << primes[i] << " ";
        }
        cout << endl;
        cout << "Cantidad de números primos menores que N: " << total_primes << endl;

        // Calcular el tiempo de ejecución
//
//        cout << "Tiempo de ejecución: " << execution_time << " segundos" << endl;
//
//        // Calcular el speedup
//        double sequential_execution_time = end_time - start_time;
//        double speedup = sequential_execution_time / execution_time;
//        cout << "Speedup: " << speedup << endl;
    } else {
        long long int N;
        MPI_Recv(&N, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        long long int lower_bound = 2 + (rank * (N - 2) / size);
        long long int upper_bound = 2 + ((rank + 1) * (N - 2) / size);

        vector<long long int> primes;
        for (long long int num = lower_bound; num < upper_bound; num++) {
            if (is_prime(num)) {
                primes.push_back(num);
            }
        }

        int count = primes.size();
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(primes.data(), count, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
