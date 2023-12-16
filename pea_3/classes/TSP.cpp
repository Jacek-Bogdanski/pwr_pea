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

#include "TSP.h"

namespace PEA {


    TSP::TSP(std::string configFileName) {
        this->configFileName = configFileName;
        this->initFiles();
    }

    
    TSP::~TSP() {
        // zamknij pliki
        configFile.close();
        outputFile.close();
    }


    void TSP::initFiles() {
        this->configFile.open(this->configFileName);

        if (!configFile.is_open()) {
            std::cout << "Config file error. Make sure that file config.ini exists. " << std::endl;
            exit(1);
        }

        int configLineCount = this->countConfigLines();
        if (configLineCount == -1) {
            std::cout << "Config file error. Make sure that file config.ini exists. " << std::endl;
            exit(1);
        }
        if (configLineCount == 0) {
            std::cout << "Config file error. Make sure that file config.ini is not empty. " << std::endl;
            exit(1);
        }

        this->setOutputFileName();
        this->outputFile.open(outputFileName, std::ios::trunc);
    }


    /**
     * @brief Funkcja sprawdza plik konfiguracyjny, oblicza ile linii kodu posiada plik config.ini
     *
     * @return int
     */
    int TSP::countConfigLines() {
        if (!this->configFile.is_open()) {
            return -1;
        }
        this->configFile.clear();
        this->configFile.seekg(0, std::ios::beg);

        int count = 0;
        std::string line;
        while (getline(this->configFile, line)) {
            // Check if the line has any non-whitespace characters.
            if (line.find_first_not_of(" \t") != std::string::npos) {
                count++;
            }
        }

        return count;
    }

    /**
     * @brief Funkcja sprawdza plik konfiguracyjny, ustawia nazwe pliku wynikowego
     *
     * @return std::string
     */
    void TSP::setOutputFileName() {
        if (!this->configFile.is_open()) {
            return;
        }

        this->configFile.clear();
        this->configFile.seekg(0, std::ios::beg);
        std::string line;

        while (getline(this->configFile, line)) {
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            if (tokens.size() == 1 && tokens[0].length() > 0) {
                this->outputFileName = tokens[0];
            }
        }
    }


    void TSP::handleConfigFile() {
        if (!this->configFile.is_open()) {
            std::cout << "Config file error." << std::endl;
            exit(1);
        }

        if (!this->outputFile.is_open()) {
            std::cout << "Output file error." << std::endl;
            exit(1);
        }

        this->configFile.clear();
        this->configFile.seekg(0, std::ios::beg);
        std::string line;

        while (getline(this->configFile, line)) {
            this->handleConfigLine(line);
        }

        return;
    }

    void TSP::handleConfigLine(std::string line) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.size() < 2) {
            return;
        }

        this->sourceFileName = tokens[0];
        int repeatCount, expectedLength;
        std::vector<int> values;

        std::istringstream(tokens[1]) >> repeatCount;
        std::istringstream(tokens[2]) >> expectedLength;

        // wyswietlenie linii informacyjnej
        this->outputFile << this->sourceFileName << ";" << expectedLength << ";";
        std::cout << this->sourceFileName << " " << expectedLength << " ";

        for (size_t i = 3; i < tokens.size(); i++) {
            this->outputFile << tokens[i] << " ";
            std::cout << tokens[i] << " ";
        }
        this->outputFile << std::endl;
        std::cout << std::endl;

        // Wczytaj dane z pliku
        if (!this->readSourceFile()) {
            return;
        }

        for (int i = 1; i <= repeatCount; i++) {
            // Parametry algorytmu
            double initialTemperature = 1000.0;
            double coolingRate = 0.95; // geometryczny schemat chłodzenia
            int epochs = 10000;

            // Start pomiaru czasu
            auto start_time = std::chrono::high_resolution_clock::now();

            // Wywołanie algorytmu Symulowanego Wyżarzania
            std::pair<std::vector<int>, int> result = this->SimulatedAnnealing(this->sourceMatrix, initialTemperature, coolingRate, epochs);

            // Wynik pomiaru czasu
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            long double miliseconds = duration.count() / 1000.0;
            this->outputFile << std::fixed << std::setprecision(4) << miliseconds << ";" << result.second << ";[";
            std::cout << std::fixed << std::setprecision(4) << miliseconds << " " << result.second << " [";

            // Wyświetl najlepszą trasę i jej koszt
            this->outputFile << result.first[0];
            std::cout << result.first[0];
            for (int i = 1; i < this->sourceMatrix.size(); i++) {
                this->outputFile << " " << result.first[i];
                std::cout << " " << result.first[i];
            }

            // dopisz nawias koncowy
            this->outputFile << ", 0]" << std::endl;
            std::cout << "]" << std::endl;
        }
        this->outputFile << std::endl;
        std::cout << std::endl;

        return;
    }

    // Funkcja do wczytywania macierzy odległości z pliku
    bool TSP::readSourceFile() {
        this->sourceFile.open(sourceDirectory + this->sourceFileName);
        if (!this->sourceFile.is_open()) {
            std::cout << "Source file error: " + this->sourceFileName + " not found in " + sourceDirectory << std::endl;
            return false;
        }

        // Liczba miast
        int n;
        this->sourceFile >> n;

        if (n < 0) {
            std::cout << "Source file format error." << std::endl;
            return false;
        }

        // Dane z pliku
        std::vector<std::vector<int>> matrix(n, std::vector<int>(n, 0));

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                this->sourceFile >> matrix[i][j];
            }
        }

        // Zamknij plik
        this->sourceFile.close();
        this->sourceMatrix = matrix;
        return true;
    }

    // Algorytm Symulowanego Wyzarzania dla problemu TSP
    std::pair<std::vector<int>, int> TSP::SimulatedAnnealing(const std::vector<std::vector<int>>& distanceMatrix, double initialTemperature, double coolingRate, int epochs) {
        int n = distanceMatrix.size();
        std::vector<int> currentSolution = generateRandomRoute(n);
        std::vector<int> bestSolution = currentSolution;
        double bestCost = evaluateRoute(bestSolution, distanceMatrix);

        double currentTemperature = initialTemperature;

        // Pętla główna
        for (int epoch = 0; epoch < epochs; ++epoch) {
            // Generacja sąsiada
            std::vector<int> neighborSolution = currentSolution;
            int cityIndex1 = rand() % (n - 1) + 1; // Exclude the starting city
            int cityIndex2 = rand() % (n - 1) + 1;
            swapCities(neighborSolution, cityIndex1, cityIndex2);

            // Obliczenie długości trasy
            double currentCost = evaluateRoute(currentSolution, distanceMatrix);
            double neighborCost = evaluateRoute(neighborSolution, distanceMatrix);

            // Prawdopodobieństwo akceptacji
            double acceptanceProbability = exp((currentCost - neighborCost) / currentTemperature);

            // Akceptacja gorszego rozwiązania
            if (acceptanceProbability > (rand() / (RAND_MAX + 1.0))) {
                currentSolution = neighborSolution;
            }

            // Aktualizacja najlepszego rozwiązania
            if (currentCost < evaluateRoute(bestSolution, distanceMatrix)) {
                bestSolution = currentSolution;
                bestCost = evaluateRoute(bestSolution, distanceMatrix);
            }

            // Chłodzenie - geometryczny schemat
            currentTemperature *= coolingRate;
        }

        std::pair<std::vector<int>, int> result;
        result.first = bestSolution;
        result.second = bestCost;
        return result;
    }


    /**
     * @brief Ocena długości trasy
     * @param route
     * @param distanceMatrix
     * @return double
     */
    double TSP::evaluateRoute(const std::vector<int>& route, const std::vector<std::vector<int>>& distanceMatrix) {
        double distance = 0.0;
        for (size_t i = 0; i < route.size() - 1; ++i) {
            distance += distanceMatrix[route[i]][route[i + 1]];
        }
        // Dodaj odległość powrotną do pierwszego miasta
        distance += distanceMatrix[route.back()][route.front()];
        return distance;
    }


    /**
     * @brief Generowanie losowej trasy
     * @return Losowa trasa
     */
    std::vector<int> TSP::generateRandomRoute(int n) {
        std::vector<int> route(n);
        for (int i = 0; i < n; i++) {
            route[i] = i;
        }

        for (int i = 0; i <= n/2; i++) {
            swapCities(route,rand()%(n-1)+1,rand()%(n-1)+1);
        }

        return route;
    }


    /**
     * @brief Zamiana miast w trasie
     */
    void TSP::swapCities(std::vector<int>& route, int index1, int index2) {
        std::swap(route[index1], route[index2]);
    }

    
} // PEA