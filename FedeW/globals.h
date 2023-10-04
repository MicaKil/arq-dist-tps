#ifndef ARQ_DIST_TPS_GLOBALS_H
#define ARQ_DIST_TPS_GLOBALS_H

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <cmath>
#include <functional>
#include <chrono>
#include <fstream>
#include <mpi.h>

#define D_TYPE long double
#define ULL unsigned long long int
#define DBG(x) #x << ": " << x
#define PROMPT(x) cout << #x << ": "; cin >> x
#define mpi(x) if (x != MPI_SUCCESS) { printf("MPI %s error\n", #x); exit(1); }
#define onlyMainThread if (rank == 0)
#define initMPI() int rank, size; mpi(MPI_Init(NULL, NULL)); MPI_Comm_rank(MPI_COMM_WORLD, &rank); MPI_Comm_size(MPI_COMM_WORLD, &size)
#define endMPI() mpi(MPI_Finalize());
#define withMPI(x) initMPI(); x; endMPI();
#define MPI_BCast_String(str, from) { int len = str.length(); mpi(MPI_Bcast(&len, 1, MPI_INT, from, MPI_COMM_WORLD)); str.resize(len); mpi(MPI_Bcast((void*)str.c_str(), str.length(), MPI_CHAR, from, MPI_COMM_WORLD)); }
#define MPI_BCast_StringVec(vec, from) { int len = vec.size(); mpi(MPI_Bcast(&len, 1, MPI_INT, from, MPI_COMM_WORLD)); vec.resize(len); for (int i = 0; i < n_patterns; i++) MPI_BCast_String(patterns[i], from); }

using namespace std;

long long time(function<void(void)> fn) {
	auto _start = std::chrono::high_resolution_clock::now();
	fn();
	auto _end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(_end - _start).count();
}


#endif //ARQ_DIST_TPS_GLOBALS_H
