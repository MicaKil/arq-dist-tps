#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

bool isPrime(long long int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long long int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

chrono::duration<double> primes_without_threads(long long int N) {
    auto start_time = chrono::high_resolution_clock::now();

    vector<long long int> primes;
    for (long long int num = 2; num < N; ++num) {
        if (isPrime(num)) {
            primes.push_back(num);
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_time - start_time;

    cout << "Cantidad de números primos menores que N: " << primes.size() << endl;

    //------------------------------------------------------------------------------------------
    int num_primes_to_display = min(10, static_cast<int>(primes.size()));
    cout << "Los 10 mayores números primos: ";
    for (int i = (int) primes.size() - 1; i >= primes.size() - num_primes_to_display; --i) {
        cout << primes[i] << " ";
    }
    cout << endl;

    cout << "Tiempo de ejecución: " << duration.count() << " segundos." << endl;

    return duration;
}
