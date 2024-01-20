//
// Autor: Jacek Bogdański | PWR 2023
//

#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <random>

#ifndef PEA_3_TSP_H
#define PEA_3_TSP_H

namespace PEA {

    struct City;

    class TSP {

        const double ALPHA = 1.0;  // Wpływ feromonów
        const double BETA = 2.0;   // Wpływ widoczności (odległości)
        const double RHO = 0.5;    // Współczynnik parowania feromonów

        // Parametry algorytmu mrówkowego
        const int numAnts = 10;
        const int numIterations = 100;

        // Miasta wczytane z pliku
        std::vector<City> cities;

    public:
        /**
        * @brief Konstruktor
        */
        TSP(std::string configFileName);

        /**
        * @brief Destruktor
        */
        ~TSP();

        /**
        * @brief Uruchomienie programu
        */
        void handleConfigFile();

    private:
        std::string configFileName;
        std::string sourceDirectory = "source/";
        std::string outputFileName = "output.txt";
        std::string sourceFileName;

        std::ifstream configFile;
        std::ofstream outputFile;
        std::ifstream sourceFile;

        /**
        * @brief Macierz sąsiedztwa
        */
        std::vector<std::vector<int>> sourceMatrix;

        /**
        * @brief Inicjalizacja plików
        */
        void initFiles();

        /**
        * @brief Sprawdzenie liczby linii w pliku konfiguracyjnym
        * @return int
        */
        int countConfigLines();

        /**
        * @brief Ustawienie parametrów
        */
        void setOutputFileName();

        /**
        * @brief Wykonanie operacji z linii pliku konfiguracyjnego
        */
        void handleConfigLine(std::string line);

        /**
        * Wczytanie macierzy odległości z pliku
        */
        bool readSourceFile();

        /**
        * Algorytm mrówkowy dla problemu TSP
        */
        std::pair<std::vector<int>, int> antAlgorithm();

        /**
        * Obliczenie odległości pomiedzy miastami
        */
        double calculateDistance(const City &city1, const City &city2);

        /**
        * Inicjalizacja feromonów
        */
        void initializePheromones(std::vector<std::vector<double>> &pheromones, double initialValue);

        /**
        * Ruchy mrówek
        */
        void
        antSteps(const std::vector<City> &cities, std::vector<std::vector<double>> &pheromones, std::vector<int> &tour);

        /**
        * Aktualizacja feromonów globalnie po przejściu mrówek
        */
        void updatePheromones(std::vector<std::vector<double>> &pheromones,
                              const std::vector<std::vector<double>> &deltaPheromones);

        /**
        * Pętla powtórzeń iteracji
        */
        void runAnts(const std::vector<City> &cities, std::vector<std::vector<double>> &pheromones);

    };

} // PEA

#endif //PEA_3_TSP_H
