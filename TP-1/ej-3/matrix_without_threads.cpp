#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// multiplicación de dos matrices de punto flotante
vector<vector<float>> multiply_matrices(const vector<vector<float>>& matrix1, const vector<vector<float>>& matrix2) {
    int N = (int) matrix1.size();
    vector<vector<float>> result(N, vector<float>(N, 0.0));  // inicializa la matriz de resultados con ceros

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];  // multiplica las matrices
            }
        }
    }

    return result;
}

// calcula la sumatoria de todos los elementos de una matriz
float sum_elements(const vector<vector<float>>& matrix) {
    int N = (int) matrix.size();
    float sum = 0.0;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sum += matrix[i][j];  
        }
    }

    return sum;
}

chrono::duration<double> matrix_without_threads(const vector<vector<float>>& matrix1, const vector<vector<float>>& matrix2) {
    int N = (int) matrix1.size();
    
    cout << "Sin Hilos:" << endl;
    cout << "Calculando..." << endl;

    auto start = chrono::high_resolution_clock::now();

    vector<vector<float>> result = multiply_matrices(matrix1, matrix2);  // realiza la multiplicación
    float sum = sum_elements(result);  // sumatoria de los elementos de la matriz resultante

    auto end = chrono::high_resolution_clock::now();  
    chrono::duration<double> duration = end - start;

    cout << "\nResultados:" << endl;
    cout << " - Esquina superior izquierda: " << result[0][0] << endl;
    cout << " - Esquina superior derecha: " << result[0][N - 1] << endl;
    cout << " - Esquina inferior izquierda: " << result[N - 1][0] << endl;
    cout << " - Esquina inferior derecha: " << result[N - 1][N - 1] << endl;
    cout << "Sumatoria: " << sum << endl;
    cout << "Tiempo sin hilos: " << duration.count() << " segundos." << endl;

    return duration;
}