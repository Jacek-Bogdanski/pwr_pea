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

        const double ALPHA = 1;  // Wpływ feromonów
        const double BETA = 2.0;   // Wpływ widoczności (odległości)
        const double RHO = 0.5;    // Współczynnik parowania feromonów

        const int numIterations = 100; // Parametry algorytmu mrówkowego


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
        std::string statFileName = "stats.txt";
        std::string sourceFileName;

        std::ifstream configFile;
        std::ofstream outputFile;
        std::ofstream statFile;
        std::ifstream sourceFile;

        std::string tspType = "TSP";
        std::string asType = "CAS";

        int repeatCount, expectedLength;

        std::vector<std::vector<double>> distances;

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
         * Przesuniecie trasy tak, aby zaczynala sie od 0
         */
        void shiftTour(std::vector<int> &tour);

        /**
        * Obliczenie odległości pomiedzy miastami
        */
        double calculateDistance(const City &city1, const City &city2);

        /**
        * Ruchy mrówek
        */
        void
        antSteps(std::vector<std::vector<double>> &pheromones, std::vector<int> &tour);

        /**
        * Aktualizacja feromonów globalnie po przejściu mrówek
        */
        void updatePheromones(std::vector<std::vector<double>> &pheromones,
                              const std::vector<std::vector<double>> &deltaPheromones);

        /**
        * Pętla powtórzeń iteracji
        */
        std::vector<int> runAnts(std::vector<std::vector<double>> &pheromones);

        /**
        * Generator liczb losowych
        */
        int getRandomInt(int minValue, int maxValue);

        /**
       * Obliczenie szacunkowej długości trasy algorytmem najbliższego sąsiada
       */
        double estimateTourLength(const std::vector<std::vector<double>>& distances);
    };

} // PEA

#endif //PEA_3_TSP_H
