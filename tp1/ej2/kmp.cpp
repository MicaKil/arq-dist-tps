#include <vector>
#include <string>

using namespace std;

// implementa el algoritmo de construcción de la función de prefijo para un patrón dado
vector<int> compute_prefix_function(const string& pattern) {
    int m = pattern.length();
    vector<int> prefix(m, 0); //vector de enteros que representa la función de prefijo
    int k = 0;

    for (int i = 1; i < m; i++) { // se calcula el valor de la función de prefijo para cada posición
        while (k > 0 && pattern[k] != pattern[i])
            k = prefix[k - 1];
        if (pattern[k] == pattern[i])
            k++;
        prefix[i] = k;
    }
    return prefix;
}

//utiliza el algoritmo KMP para contar el número de ocurrencias de un patrón en un texto dado
int count_pattern_occurrences(const string& pattern, const string& text) {
    int m = pattern.length();
    int n = text.length();
    int count = 0; //conteo de ocurrencias
    vector<int> prefix = compute_prefix_function(pattern); // calcula la función de prefijo del patrón
    int k = 0;

    for (int i = 0; i < n; i++) { //bucle que recorre los caracteres de text
        while (k > 0 && text[i] != pattern[k]) //ajusta el valor de k en función de los caracteres coincidentes entre pattern y text
            k = prefix[k - 1];
        if (text[i] == pattern[k]) //se ha encontrado una ocurrencia 
            k++;
        if (k == m) {
            count++;
            k = prefix[k - 1]; //se actualiza k
        }
    }
    return count;
}