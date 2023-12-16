//
// Autor: Jacek Bogdański | PWR 2023
//

#include "classes/TSP.h"

int main() {
    PEA::TSP tsp("config.ini");
    tsp.handleConfigFile();
    return 0;
}




// Funkcja generująca losową trasę początkową
vector<int> generateRandomRoute(int n) {
    vector<int> route(n);
    for (int i = 0; i < n; ++i) {
        route[i] = i;
    }
    random_shuffle(route.begin() + 1, route.end()); // Omit the first city (assumed starting city)
    return route;
}

// Funkcja zamieniająca dwa miasta w trasie
void swapCities(vector<int>& route, int index1, int index2) {
    swap(route[index1], route[index2]);
}

// Algorytm Symulowanego Wyżarzania
vector<int> simulatedAnnealing(const vector<vector<int>>& distanceMatrix, double initialTemperature, double coolingRate, int epochs) {
    // Inicjalizacja
    int n = distanceMatrix.size();
    vector<int> currentSolution = generateRandomRoute(n);
    vector<int> bestSolution = currentSolution;

    double currentTemperature = initialTemperature;

    // Pętla główna
    for (int epoch = 0; epoch < epochs; ++epoch) {
        // Generacja sąsiada
        vector<int> neighborSolution = currentSolution;
        int cityIndex1 = rand() % (n - 1) + 1; // Exclude the starting city
        int cityIndex2 = rand() % (n - 1) + 1;
        swapCities(neighborSolution, cityIndex1, cityIndex2);

        // Ewaluacja jakości rozwiązania
        double currentEnergy = evaluateRoute(currentSolution, distanceMatrix);
        double neighborEnergy = evaluateRoute(neighborSolution, distanceMatrix);

        // Prawdopodobieństwo akceptacji
        double acceptanceProbability = exp((currentEnergy - neighborEnergy) / currentTemperature);

        // Akceptacja gorszego rozwiązania
        if (acceptanceProbability > (rand() / (RAND_MAX + 1.0))) {
            currentSolution = neighborSolution;
        }

        // Aktualizacja najlepszego rozwiązania
        if (evaluateRoute(currentSolution, distanceMatrix) < evaluateRoute(bestSolution, distanceMatrix)) {
            bestSolution = currentSolution;
        }

        // Chłodzenie - geometryczny schemat
        currentTemperature *= coolingRate;
    }

    return bestSolution;
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    // TSP Obiekt
    TSP tsp;

    // Wczytaj dane z pliku
    if (!tsp.readSourceFile()) {
        return 1;
    }

    // Parametry algorytmu
    double initialTemperature = 1000.0;
    double coolingRate = 0.95; // geometryczny schemat chłodzenia
    int epochs = 10000;

    // Wywołanie algorytmu Symulowanego Wyżarzania
    vector<int> bestRoute = simulatedAnnealing(tsp.getSourceMatrix(), initialTemperature, coolingRate, epochs);

    // Wyświetlenie wyników
    cout << "Najlepsza trasa: ";
    for (int city : bestRoute) {
        cout << city << " ";
    }
    cout << "\nDługość trasy: " << evaluateRoute(bestRoute, tsp.getSourceMatrix()) << endl;

    return 0;
}