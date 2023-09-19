#include <iostream>
#include <vector>
#include <chrono>

bool isPrime(long long int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long long int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

std::chrono::duration<double> primes_without_threads(int N) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<long long int> primes;
    for (long long int num = 2; num < N; ++num) {
        if (isPrime(num)) {
            primes.push_back(num);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;

    std::cout << "Cantidad de números primos menores que N: " << primes.size() << std::endl;

    //------------------------------------------------------------------------------------------
    int num_primes_to_display = std::min(10, static_cast<int>(primes.size()));
    std::cout << "Los 10 mayores números primos: ";
    for (int i = primes.size() - 1; i >= primes.size() - num_primes_to_display; --i) {
        std::cout << primes[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Tiempo de ejecución: " << duration.count() << " segundos." << std::endl;

    return duration;
}
