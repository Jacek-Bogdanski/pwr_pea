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

    const double ALPHA = 1.0;  // Wpływ feromonów
    const double BETA = 2.0;   // Wpływ widoczności (odległości)
    const double RHO = 0.5;    // Współczynnik parowania feromonów


    // Parametry algorytmu mrówkowego
    const int numAnts = 10;
    const int numIterations = 100;

    struct City {
        int id;
        double x, y;
    };

    /**
     * @brief Konstruktor
     */
    TSP::TSP(std::string configFileName) {
        this->configFileName = configFileName;
        initFiles();
    }

    /**
     * @brief Destruktor
     */
    TSP::~TSP() {
        configFile.close();
        outputFile.close();
    }

    /**
     * @brief Uruchomienie programu
     */
    void TSP::handleConfigFile() {
        if (!this->configFile.is_open()) {
            std::cout << "Config file error." << std::endl;
            exit(1);
        }

        if (!this->outputFile.is_open()) {
            std::cout << "Output file error." << std::endl;
            exit(1);
        }

        outputFile << "Time[ms];Distance;Path" << std::endl;

        this->configFile.clear();
        this->configFile.seekg(0, std::ios::beg);
        std::string line;

        while (getline(this->configFile, line)) {
            this->handleConfigLine(line);
        }

        return;
    }

    /**
     * @brief Inicjalizacja plików
     */
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
     * @brief Sprawdzenie liczby linii w pliku konfiguracyjnym
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
     * @brief Ustawienie parametrów
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

    /**
     * @brief Wykonanie operacji z linii pliku konfiguracyjnego
     */
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

        std::istringstream(tokens[1]) >> repeatCount;
        std::istringstream(tokens[2]) >> expectedLength;

        // Wczytaj dane z pliku
        if (!this->readSourceFile()) {
            return;
        }

        // wyswietlenie linii informacyjnej
        this->outputFile << this->sourceFileName << ";" << expectedLength << ";";
        std::cout << this->sourceFileName << " " << expectedLength << " ";

        for (size_t i = 3; i < tokens.size(); i++) {
            this->outputFile << tokens[i] << " ";
            std::cout << tokens[i] << " ";
        }

        this->outputFile << std::endl;
        std::cout << std::endl;

        for (int i = 1; i <= repeatCount; i++) {
            std::cout<<"Starting algorithm iteration " << i << std::endl;
            // Start pomiaru czasu
            auto start_time = std::chrono::high_resolution_clock::now();

            // Wywołanie algorytmu mrówkowego
            std::pair<std::vector<int>, int> result = this->antAlgorithm();

            // Wynik pomiaru czasu
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            long double miliseconds = duration.count() / 1000.0;
            this->outputFile << std::fixed << std::setprecision(4) << miliseconds << ";" << result.second << ";[";
            std::cout << std::fixed << std::setprecision(4) << miliseconds << " " << result.second << " [";

            // Wyświetl najlepszą trasę i jej koszt
            this->outputFile << result.first[0];
            std::cout << result.first[0];
            for (int i = 1; i < cities.size();
            i++) {
                this->outputFile << " " << result.first[i];
                std::cout << " " << result.first[i];
            }
            // dopisz nawias koncowy
            this->outputFile << ", 0]" << std::endl;
            std::cout << ", 0]" << std::endl;
        }
        this->outputFile << std::endl;
        std::cout << std::endl;
    }

    /**
     * Wczytanie macierzy odległości z pliku
     */
    bool TSP::readSourceFile() {
        this->sourceFile.open(sourceDirectory + this->sourceFileName);
        if (!this->sourceFile.is_open()) {
            std::cout << "Source file error: " + this->sourceFileName + " not found in " + sourceDirectory << std::endl;
            return false;
        }

        std::vector<City> cities;
        int id;
        double x, y;

        std::string tmp;
        do{
            this->sourceFile >> tmp;
        } while(tmp != "NODE_COORD_SECTION");

        while (this->sourceFile >> id >> x >> y) {
            cities.push_back({id, x, y});
        }

        this->sourceFile.close();
        this->cities = cities;
        return true;
    }

    /**
     * Algorytm mrówkowy dla problemu TSP
     */
    std::pair<std::vector<int>, int> TSP::antAlgorithm() {
        std::vector<int> bestSolution;
        double bestCost;

        int numCities = cities.size();
        if(numCities == 0){
            throw std::runtime_error(std::string("Empty city list error"));
        }
        std::cout << "City count: " << numCities << std::endl;
        std::vector<std::vector<double>> distances(numCities, std::vector<double>(numCities, 0.0));
        for (int i = 0; i < numCities; ++i) {
            for (int j = 0; j < numCities; ++j) {
                distances[i][j] = calculateDistance(cities[i], cities[j]);
            }
        }

        std::vector<std::vector<double>> pheromones(numCities, std::vector<double>(numCities, 1.0));

        // Wywołanie algorytmu mrówkowego
        runAnts(cities, pheromones);

        // Znajdowanie najlepszej trasy
        std::vector<int> bestTour(numCities, 0);
        for (int i = 1; i < numCities; ++i) {
            bestTour[i] = i;
        }
        double bestTourLength = std::numeric_limits<double>::max();

        for (int i = 0; i < numCities; ++i) {
            std::rotate(bestTour.begin(), bestTour.begin() + 1, bestTour.end());

            double tourLength = 0.0;
            for (int j = 0; j < numCities - 1; ++j) {
                tourLength += distances[bestTour[j]][bestTour[j + 1]];
            }
            tourLength += distances[bestTour[numCities - 1]][bestTour[0]];

            if (tourLength < bestTourLength) {
                bestTourLength = tourLength;
            }
        }

        std::pair<std::vector<int>, int> result;
        result.first = bestTour;
        result.second = bestTourLength;
        return result;
    }

    /**
     * Obliczenie odległości pomiedzy miastami
     */
    double TSP::calculateDistance(const City &city1, const City &city2) {
        double dx = city1.x - city2.x;
        double dy = city1.y - city2.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    /**
     * Inicjalizacja feromonów
     */
    void TSP::initializePheromones(std::vector<std::vector<double>> &pheromones, double initialValue) {
        int numCities = pheromones.size();
        for (int i = 0; i < numCities; ++i) {
            for (int j = 0; j < numCities; ++j) {
                pheromones[i][j] = initialValue;
            }
        }
    }

    /**
     * Ruchy mrówek
     */
    void TSP::antSteps(const std::vector<City> &cities, std::vector<std::vector<double>> &pheromones,
                       std::vector<int> &tour) {
        int numCities = cities.size();
        std::vector<bool> visited(numCities, false);

        // Początkowe miasto (startowe)
        int currentCity = tour[0];
        visited[currentCity] = true;

        // Wybór kolejnych miast
        for (int i = 1; i < numCities; ++i) {
            // Obliczenie prawdopodobieństw wyboru kolejnych miast
            std::vector<double> probabilities(numCities, 0.0);
            double totalProbability = 0.0;

            for (int j = 0; j < numCities; ++j) {
                if (!visited[j]) {
                    // Prawdopodobieństwo wyboru miasta j
                    probabilities[j] = std::pow(pheromones[currentCity][j], ALPHA) *
                                       std::pow(1.0 / calculateDistance(cities[currentCity], cities[j]), BETA);

                    totalProbability += probabilities[j];
                }
            }

            // Wybór miasta na podstawie ruletki
            double randomValue = (rand() / (RAND_MAX + 1.0)) * totalProbability;
            double cumulativeProbability = 0.0;
            int chosenCity = -1;

            for (int j = 0; j < numCities; ++j) {
                if (!visited[j]) {
                    cumulativeProbability += probabilities[j];
                    if (cumulativeProbability >= randomValue) {
                        chosenCity = j;
                        break;
                    }
                }
            }

            // Aktualizacja trasy
            tour[i] = chosenCity;
            visited[chosenCity] = true;
            currentCity = chosenCity;
        }
    }

    /**
     * Aktualizacja feromonów globalnie po przejściu mrówek
     */
    void TSP::updatePheromones(std::vector<std::vector<double>> &pheromones,
                               const std::vector<std::vector<double>> &deltaPheromones) {
        int numCities = pheromones.size();
        for (int i = 0; i < numCities; ++i) {
            for (int j = 0; j < numCities; ++j) {
                pheromones[i][j] = (1.0 - RHO) * pheromones[i][j] + deltaPheromones[i][j];
            }
        }
    }


    /**
     * Pętla powtórzeń iteracji
     */
    void TSP::runAnts(const std::vector<City> &cities, std::vector<std::vector<double>> &pheromones) {
        int numCities = cities.size();
        std::vector<std::vector<double>> deltaPheromones(numCities, std::vector<double>(numCities, 0.0));

        for (int iteration = 0; iteration < numIterations; ++iteration) {
            // Symulacja ruchu mrówek
            for (int ant = 0; ant < numAnts; ++ant) {
                std::vector<int> tour(numCities, 0);
                for (int i = 1; i < numCities; ++i) {
                    tour[i] = i;
                }

                // Implementacja ruchu mrówki
                antSteps(cities, pheromones, tour);

                // Obliczanie długości trasy mrówki
                double tourLength = 0.0;
                for (int i = 0; i < numCities - 1; ++i) {
                    tourLength += calculateDistance(cities[tour[i]], cities[tour[i + 1]]);
                }
                tourLength += calculateDistance(cities[tour[numCities - 1]], cities[tour[0]]);

                // Aktualizacja deltaPheromones
                for (int i = 0; i < numCities - 1; ++i) {
                    deltaPheromones[tour[i]][tour[i + 1]] += 1.0 / tourLength;
                    deltaPheromones[tour[i + 1]][tour[i]] += 1.0 / tourLength;
                }
                deltaPheromones[tour[numCities - 1]][tour[0]] += 1.0 / tourLength;
                deltaPheromones[tour[0]][tour[numCities - 1]] += 1.0 / tourLength;
            }

            // Aktualizacja feromonów globalnie
            updatePheromones(pheromones, deltaPheromones);

            // Parowanie feromonów
            for (int i = 0; i < numCities; ++i) {
                for (int j = 0; j < numCities; ++j) {
                    pheromones[i][j] *= (1.0 - RHO);
                }
            }
        }
    }

} // PEA

