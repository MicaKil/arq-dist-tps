// Escriba un programa que realice la multiplicación de dos matrices de N*N elementos
// del tipo float (número en punto flotante), y luego realice la sumatoria de todos los
// elementos de la matriz resultante. Muestre por pantalla los elementos de cada
// esquina de las matrices y el resultado de la sumatoria. El programa deberá permitir
// cambiar el número de elementos de las matrices (es decir, que el número de
// elementos sea N*N, donde N pueda cambiarse).
// Resuelva el problema de dos formas:
// ● Sin emplear multihilos.
// ● Empleando múltiples hilos que trabajen concurrentemente, resolviendo cada
// hilo un grupo de las N filas de la matriz resultado (nota, usar N hilos no es la
// forma más eficiente de resolver el problema, 10 o 20 hilos son un número
// adecuado).
// El programa deberá poder resolver productos de matrices de 3000*3000 elementos.
// a) Incluya código que permita obtener el tiempo de ejecución en cada programa, y calcule el speedup (ver Anexo 1.4).
// b) Observe el porcentaje de uso de cada núcleo en cada implementación (ver Anexo 1.6).

// Ayuda:
// Si los elementos de la matriz1 son 0.1, y los elementos de la matriz2 son 0.2,
// suponiendo matrices de 1000*1000 elementos, la matriz resultante será (se
// muestran solo los elementos en los extremos):
// |20.0003 …… 20.0003|
// | ………………………..|
// |20.0003 …… 20.0003|
// el resultado de la sumatoria será 2*10^7.

// Si el número de elementos de las matrices es 300*300, la matriz resultante será (se
// muestran solo los elementos en los extremos):
// |6.000 …… 6.000|
// | ………………….|
// |6.000 …… 6.000|
// el resultado de la sumatoria será 540000.

// Si el número de elementos de las matrices es 3000*3000, la matriz resultante será
// (se muestran solo los elementos en los extremos):
// |60.0012 …… 60.0012|
// | …………………….….|
// |60.0012 …… 60.0012|
// el resultado de la sumatoria será 5.71526*10^8.

// Nota: En la unidad 2 se estudiará la forma de resolver este problema de forma
// eficiente mediante diagramas de dependencia.

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

#include "matrix_without_threads.cpp"

using namespace std;
std::mutex sum_mutex; // mutex para proteger sum_threads

int main() {
    int N = 1000;  // tamaño de las matrices 

    vector<vector<float>> matrix1(N, vector<float>(N, 0.1));  // crea la matriz 1 y la llena con 0.1
    vector<vector<float>> matrix2(N, vector<float>(N, 0.2));  // crea la matriz 2 y la llena con 0.2

    //------------------------------------------------------------------------------------------
    // sin hilos
    chrono::duration<double> duration = matrix_without_threads(matrix1, matrix2);

    //------------------------------------------------------------------------------------------
    int num_threads = 10;  // número de hilos 

    cout << "\nCon Hilos:" << endl;
    cout << "Calculando..." << endl;

    auto start_threads = chrono::high_resolution_clock::now();

    vector<vector<float>> result_threads(N, vector<float>(N, 0.0));  // inicializa la matriz de resultados con ceros
    vector<thread> threads;

    // divide las filas de la matriz resultado entre los hilos
    int rows_per_thread = N / num_threads;

    for (int t = 0; t < num_threads; t++) {
        int start_row = t * rows_per_thread;
        int end_row = (t == num_threads - 1) ? N : start_row + rows_per_thread;  //(? :) para determinar el valor de end_row de manera condicional
        //t == num_threads - 1 verifica si el hilo actual es el último hilo 
        // ? N : start_row + rows_per_thread -> si (t == num_threads - 1), end_row = N (último hilo debe procesar todas las filas restantes)
        // si (t == num_threads - 1) es falsa entonces end_row = start_row + rows_per_thread

        threads.push_back(thread([&, start_row, end_row]() {
            //[&, start_row, end_row]() { ... }: define una función lambda 
            // toma tres variables por captura: & significa "capturar por referencia" 
            // las variables en el alcance externo (start_row y end_row) se pueden usar dentro de la lambda
            for (int i = start_row; i < end_row; i++) {
                for (int j = 0; j < N; j++) {
                    for (int k = 0; k < N; k++) {
                        result_threads[i][j] += matrix1[i][k] * matrix2[k][j];  // multiplica las matrices
                    }
                }
            }
        }));
    }

    // espera a que todos los hilos terminen
    for (auto& thread : threads) {
        thread.join();
    }

    vector<thread> threads_sum;
    float sum_threads = 0.0; // suma total de elementos

    for (int t = 0; t < num_threads; t++) {
        int start_row = t * rows_per_thread;
        int end_row = (t == num_threads - 1) ? N : start_row + rows_per_thread;

        threads_sum.push_back(std::thread([&, start_row, end_row]() {
            float local_sum = 0.0;

            for (int i = start_row; i < end_row; i++) {
                for (int j = 0; j < N; j++) {
                    local_sum += result_threads[i][j];
                }
            }

            // bloquea el mutex antes de actualizar sum_threads
            sum_mutex.lock();
            sum_threads += local_sum;
            sum_mutex.unlock();
        }));
    }
    
    // espera a que todos los hilos terminen
    for (auto& thread : threads_sum) {
        thread.join();
    }

    auto end_threads = chrono::high_resolution_clock::now();  // tiempo de finalización
    chrono::duration<double> duration_threads = end_threads - start_threads;

    //------------------------------------------------------------------------------------------
    cout << "\nResultado con hilos:" << endl;
    cout << " - Esquina superior izquierda: " << result_threads[0][0] << endl;
    cout << " - Esquina superior derecha: " << result_threads[0][N - 1] << endl;
    cout << " - Esquina inferior izquierda: " << result_threads[N - 1][0] << endl;
    cout << " - Esquina inferior derecha: " << result_threads[N - 1][N - 1] << endl;
    cout << "Sumatoria: " << sum_threads  << endl;
    cout << "Tiempo con hilos: " << duration_threads.count() << " segundos." << endl;
    
    //------------------------------------------------------------------------------------------
    double speed_up = ((double) duration.count())/duration_threads.count(); //solo en 1 porque automáticamente castea el de abajo
    cout << "\nSpeed up: " << speed_up << endl;
    return 0;
}