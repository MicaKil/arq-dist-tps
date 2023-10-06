//
// Created by mica on 05/10/2023.
//

//Se da un archivo de datos en forma de caracteres llamado “texto.txt” de 200 MB (200 millones de caracteres), que puede
// descargar desde: https://drive.google.com/file/d/1A7hmHJ60ahudNIw7gRm6DezJIBdLHyfE/view?usp= sharing, y 32 patrones,
// cada uno en forma de cadena de caracteres almacenados una por línea en el archivo “patrones.txt” (que se encuentran
// en la carpeta del trabajo práctico N°3). Tanto el archivo “texto.txt” como el archivo “patrones.txt” son los mismos
// que se utilizaron en el Trabajo Práctico N°1. Cree un programa que busque la cantidad de veces que cada patrón
// aparece en el archivo “texto.txt”. El programa deberá generar una salida similar a la siguiente:
// el patron 0 aparece 14 veces. Buscado por <IP>
// el patron 1 aparece 3 veces. Buscado por <IP>
// el patron 2 aparece 0 veces. Buscado por <IP>
// el patron 3 aparece 0 veces. Buscado por <IP>
// el patron 4 aparece 0 veces. Buscado por <IP>
// el patron 5 aparece 0 veces. Buscado por <IP>
// ……………..
// ……………..
// Donde <IP> es la IP de la máquina que buscó el patrón.
// El archivo “texto.txt” no posee saltos de línea (es decir, posee solo una línea).
// Resuelva el problema de dos formas:
// Empleando un solo proceso que busque todos los patrones (en el Trabajo Práctico N°1 se creó este programa).
// Empleando 32 procesos de modo que cada proceso busque un patrón.
// Incluya código que permita obtener el tiempo de ejecución y calcule el speedup.
// a) Ejecute el programa en una sola máquina (MPI distribuirá los procesos en los núcleos disponibles). Luego en todas
// las máquinas disponibles.
// b) Tome nota del tiempo de ejecución y calcule el speedup respecto a un solo proceso.
// c) Compare los resultados con los obtenidos en el ejercicio N°2 del trabajo práctico N°1.

// Ayuda: El patrón 0 aparece 14 veces, el patrón 1 aparece 3 veces, el patrón 6 aparece 4 veces, el patrón 9 aparece
// 3622 veces, el patrón 11 aparece 2 veces, el patrón 13 aparece 6 veces, el patrón 16 aparece 2 veces, el patrón 18
// aparece 6 veces, el patrón 21 aparece 2 veces, el patrón 27 aparece 6 veces, todos los demás patrones aparecen 0
// veces.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <mpi.h>
#include "kmp.cpp"
#include "direccion_IP.cpp"

using namespace std;

vector<string> patterns;
string text;

// busca un patrón en el texto utilizando MPI
unsigned int search_pattern_in_process(unsigned int pattern_index) {
    unsigned int count = count_pattern_occurrences(patterns[pattern_index], text);
    return count;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int num_procs, rank;
    char ip_address[40];
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto start = chrono::high_resolution_clock::now();

    obtener_IP(ip_address);

    if (rank == 0) {
        ifstream patterns_file("patrones.txt");
        string pattern;
        while (getline(patterns_file, pattern)) {
            patterns.push_back(pattern);
        }
        patterns_file.close();

        ifstream text_file("texto.txt");
        getline(text_file, text);
        text_file.close();
    }

    unsigned int num_patterns = patterns.size();
    unsigned int patterns_per_process = num_patterns / num_procs;
    unsigned int remainder = num_patterns % num_procs;

    // distribuir los patrones entre los procesos
    vector<unsigned int> pattern_counts(num_procs, patterns_per_process);  // inicializar con la cantidad de patrones por proceso
    for (int i = 0; i < remainder; i++) {  // distribuir el resto de patrones
        pattern_counts[i]++;
    }

    vector<unsigned int> pattern_counts_prefix(num_procs);  // vector con el índice inicial de patrones que debe buscar cada proceso
    pattern_counts_prefix[0] = 0;
    for (int i = 1; i < num_procs; i++) {
        pattern_counts_prefix[i] = pattern_counts_prefix[i - 1] + pattern_counts[i - 1];
    }

    unsigned int local_count;
    unsigned int start_index = pattern_counts_prefix[rank];
    unsigned int end_index = start_index + pattern_counts[rank];


    for (unsigned int i = start_index; i < end_index; i++) {
        local_count = search_pattern_in_process(i);
        cout << "El patrón " << i << " aparece " << local_count << " veces. Buscado por IP = " << ip_address << ". Proceso número: "<< rank << " Patrón: " << patterns[i] << endl;
    }

    // sincronizar todos los procesos antes de continuar
    MPI_Barrier(MPI_COMM_WORLD);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    if (rank == 0) {
        cout << "\nTiempo total: " << duration.count() << " segundos." << endl;
        //cout << "\nSpeed up: " << duration.count() / (duration.count() * num_procs) << endl;
    }

    MPI_Finalize();
    return 0;
}
