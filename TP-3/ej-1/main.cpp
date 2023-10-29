//
// Created by Micaela on 04/10/2023.
//

//Escribir un programa que calcule el logaritmo natural de números mayores a 1500000 (1.5*106) en punto flotante de doble
//precisión largo (tipo long double en C++, float en Python) mediante serie de Taylor, empleando 10000000 (diez millones)
//de términos de dicha serie. El resultado debe imprimirse con 15 dígitos. Deberá utilizar N procesos que se ejecuten en
//paralelo, resolviendo cada proceso una parte de la sumatoria de la Serie de Taylor.
//Ayuda: ln(1500000)=14.2209756660724

#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <mpi.h>

using namespace std;

// función auxiliar para calcular la serie
long double cal_series(long double x, int start, int end) {
    long double _2n_mas_1;
    long double result = 0.0;
    long double term = ((x - 1) / (x + 1));

    for (int i = start; i <= end; ++i) {
        _2n_mas_1 = 2 * (long double) i + 1;
        result += pow(term, _2n_mas_1) / _2n_mas_1;
    }
    return 2 * result;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long double x;
    if (argc < 2) {
        x = 1500000.0;  // valor predeterminado para x
    } else {
        x = stold(argv[1]); // asigna el input a x
    }

    int num_terms = 10000000;
    int terms_per_process = num_terms / size;
    int start_term = rank * terms_per_process;
    int end_term = (rank + 1) * terms_per_process - 1;

    long double result = 0.0;

    //------------------------------------------------------------------------------------------
    cout << "\nCalculando en proceso " << rank << "...\n" << endl;

    auto start_time = chrono::high_resolution_clock::now();  // empieza el clock

    result = cal_series(x, start_term, end_term);

    long double global_result = 0.0;
    MPI_Reduce(&result, &global_result, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    // MPI_Reduce toma el valor de result de cada proceso y lo suma en global_result del proceso 0

    // MPI_Reduce(void* send_data, void* recv_data, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm communicator)
    // The send_data parameter is an array of elements of type datatype that each process wants to reduce. The recv_data
    // is only relevant on the process with a rank of root. The recv_data array contains the reduced result and has a
    // size of sizeof(datatype) * count. The op parameter is the operation that you wish to apply to your data.

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    if (rank == 0) {
        cout << "Resultado: " << setprecision(15) << global_result << endl;
        cout << "Tiempo de ejecución: " << duration.count() << " ms\n" << endl;
        cout << "Cantidad de procesos: " << size << endl;
    }

    MPI_Finalize();
    return 0;
}
