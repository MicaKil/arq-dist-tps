#include "../globals.h"

#define TERMS 100000000

__attribute__((always_inline)) inline D_TYPE ln_taylor_term(D_TYPE x, D_TYPE n, D_TYPE quot) {
	D_TYPE n21 = 2 * n + 1;
	return pow(quot, n21) / n21;
}

D_TYPE mt_ln_worker(D_TYPE x, D_TYPE quot, int worker, int count) {
	D_TYPE result = 0;
	for (long long i = worker * count, target = (worker + 1) * count; i < target; i++)
		result += 2 * ln_taylor_term(x, i, quot);
	return result;
}

void solve_ln(int n_to_use, int rank, int size) {
	mpi(MPI_Bcast(&n_to_use, 1, MPI_INT, 0, MPI_COMM_WORLD));
	D_TYPE result = 0;
	D_TYPE quot = ((D_TYPE) n_to_use - 1) / ((D_TYPE) n_to_use + 1);
	result = mt_ln_worker(n_to_use, quot, rank, TERMS / size);
	D_TYPE results[size] = {0};
	mpi(MPI_Gather(&result, 1, MPI_LONG_DOUBLE, results, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD));
	result = 0;
	onlyMainThread {
		for (int i = 0; i < size; i++) result += results[i];
		cout << std::setprecision(20) << result << endl;
	}
}

int main() {
	int n_to_use = 1500000;
	withMPI (
		//	onlyMainThread cin >> n_to_use;
		auto duration = time([n_to_use, rank, size]() { solve_ln(n_to_use, rank, size); });
		onlyMainThread cout << "Took " << (D_TYPE) duration / (D_TYPE) 1000000000 << " seconds" << endl;
	)
	return 0;
}
