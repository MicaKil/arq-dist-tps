//
// Created by micaela on 11/10/2023.
//

//Escriba un programa que realice la multiplicación de dos matrices de N*N elementos (siendo N un número grande, como 300,
//1000 o 3000) del tipo float (número en punto flotante), y luego realice la sumatoria de todos los elementos de la matriz
//resultante. Muestre por pantalla los elementos de cada esquina de las matrices y el resultado de la sumatoria.
//Escriba el programa de modo que múltiples procesos trabajen concurrentemente, resolviendo cada proceso un grupo de las
//N filas o columnas de la matriz resultado (nota, usar miles de procesos no es la forma más eficiente de resolver el
//problema, a menos que se disponga de miles de núcleos). Incluya código que permita obtener el tiempo de ejecución.
//Requisitos:
//El programa deberá permitir cambiar el número N de filas y columnas de las matrices.
//Ayuda:
//Si los elementos de la matriz1 son 0.1, y los elementos de la matriz2 son 0.2, suponiendo matrices de 300*300, la
//matriz resultante será (se muestran solo los elementos en los extremos):
//|6.000 …… 6.000|
//| ………………….|
//|6.000 …… 6.000|
//y el resultado de la sumatoria será 540000.
//
//Si el número de elementos de las matrices es 1000*1000, la matriz resultante será (se muestran solo los elementos en
//los extremos):
//|20.0003 …… 20.0003|
//| ………………………..|
//|20.0003 …… 20.0003|
//y el resultado de la sumatoria será 2*10^7.
//
//Si el número de elementos de las matrices es 3000, la matriz resultante será (se muestran solo los elementos en los
//extremos):
//|60.0012 …… 60.0012|
//| …………………….….|
//|60.0012 …… 60.0012|
//y el resultado de la sumatoria será 5.71526*10^8.

#include <iostream>
#include <vector>
#include <chrono>
#include <mpi.h>

using namespace std;

// función para imprimir los elementos en las esquinas de una matriz
void print_corner_elements(const vector<vector<float>>& matrix, int size) {
    cout << "Esquina Superior Izquierda: " << matrix[0][0] << endl;
    cout << "Esquina Superior Derecha: " << matrix[0][size - 1] << endl;
    cout << "Esquina Inferior Izquierda: " << matrix[size - 1][0] << endl;
    cout << "Esquina Inferior Derecha: " << matrix[size - 1][size - 1] << endl;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 300; // tamaño de la matriz
    int rows_per_process = N / size; // filas por proceso

    vector<vector<float>> matrix1(N, vector<float>(N, 0.1));  // crea la matriz 1 y la llena con 0.1
    vector<vector<float>> matrix2(N, vector<float>(N, 0.2));  // crea la matriz 2 y la llena con 0.2
    vector<vector<float>> result(rows_per_process, vector<float>(N));


    auto start = chrono::high_resolution_clock::now();

    // multiplicación de matrices
    for (int i = rank * rows_per_process; i < (rank + 1) * rows_per_process; ++i) {
        for (int j = 0; j < N; ++j) {
            result[i - rank * rows_per_process][j] = 0.0;
            for (int k = 0; k < N; ++k) {
                result[i - rank * rows_per_process][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    // reducción de datos y cálculo de la suma total de los elementos de la matriz resultante
    float total_sum = 0.0;
    float local_sum = 0.0;
    for (int i = 0; i < rows_per_process; ++i) {
        for (int j = 0; j < N; ++j) {
            local_sum += result[i][j];
        }
    }

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    // imprimir resultados
    if (rank == 0) {
        print_corner_elements(result, rows_per_process);
        cout << "Resultado de la sumatoria: " << total_sum << endl;
        cout << "Tiempo de ejecución: " << duration.count() << " segundos." << endl;
    }

    MPI_Finalize();

    return 0;
}
