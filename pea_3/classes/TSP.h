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

    class TSP {

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
        * @brief Algorytm Symulowanego Wyzarzania dla problemu TSP
        */
        std::pair<std::vector<int>, int> SimulatedAnnealing(const std::vector<std::vector<int>>& distanceMatrix, double alpha, int swapType, int coolType);

        /**
        * @brief Ocena długości trasy
        */
        double evaluateRoute(const std::vector<int>& route, const std::vector<std::vector<int>>& distanceMatrix);

        /**
        * @brief Generowanie losowej trasy
        * @return Losowa trasa
        */
        std::vector<int> generateRandomRoute(int n);

        /**
        * @brief Zamiana 2 miast w trasie
        */
        void swap2Cities(std::vector<int>& route);

        /**
        * @brief Zamiana 3 miast w trasie
        */
        void swap3Cities(std::vector<int>& route);

        /**
        * @brief Obliczenie temperatury początkowej
        */
        double calculateInitialTemperature(double initialDistance, double acceptanceProbability);

        /**
        * @brief Obliczenie symbolu newtona
        */
        int symbolNewtona(int n, int k);
    };


} // PEA

#endif //PEA_3_TSP_H
