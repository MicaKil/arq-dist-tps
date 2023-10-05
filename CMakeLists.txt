cmake_minimum_required(VERSION 3.22)
project(Arquitecturas_Distribuidas)

set(CMAKE_CXX_STANDARD 14)

add_executable(Arquitecturas_Distribuidas
        TP-1/ej1/ln_with_threads.cpp
        TP-1/ej1/ln_without_threads.cpp
        TP-1/ej1/main.cpp
        TP-1/ej2/kmp.cpp
        TP-1/ej2/main.cpp
        TP-1/ej3/main.cpp
        TP-1/ej3/matrix_without_threads.cpp
        TP-1/ej4/main.cpp
        TP-1/ej4/primes_without_threads.cpp
        TP-3/ej-0/direccion_IP.cpp
        TP-3/ej-0/main.cpp
        TP-3/ej-1/main.cpp
        TP-3/ej-2/main.cpp
        TP-3/ej-2/kmp.cpp
        TP-3/ej-2/direccion_IP.cpp
        Utils/main.cpp
        Utils/globals.h)
