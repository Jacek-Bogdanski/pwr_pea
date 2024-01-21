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
        statFile.close();
        sourceFile.close();
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

        outputFile << "Time[ms];Error;Distance;Path" << std::endl;
        statFile << "Filename;Type;AS Type;Expected Distance;Best Distance;Avg Error[%];Avg Time[ms];Best Path" << std::endl;

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
        this->statFile.open(statFileName, std::ios::trunc);
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

                if(tokens[0] == "MODE_CAS"){
                    this->asType = "CAS";
                } else if(tokens[0] == "MODE_DAS"){
                    this->asType = "DAS";
                }
                else{
                    this->outputFileName = tokens[0];
                }

            } else if (tokens.size() == 2 && tokens[0].length() > 0 && tokens[1].length() > 0) {
                this->outputFileName = tokens[0];
                this->statFileName = tokens[1];
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

        long double avgError = 0, avgTime = 0;
        std::vector<int> bestPath;
        int forAvgCount = 0, bestTourCost = std::numeric_limits<int>::max();

        for (int i = 1; i <= repeatCount; i++) {
            std::cout << "## Starting algorithm iteration " << i << std::endl;
            // Start pomiaru czasu
            auto start_time = std::chrono::high_resolution_clock::now();

            // Wywołanie algorytmu mrówkowego
            std::pair<std::vector<int>, int> result = this->antAlgorithm();

            // Wynik pomiaru czasu
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            long double miliseconds = duration.count() / 1000.0;
            this->outputFile << std::fixed << std::setprecision(4) << miliseconds << ";";
            std::cout << std::fixed << std::setprecision(4) << miliseconds << " ";

            avgTime += miliseconds;

            // wyswietlenie bledu
            double error = ((double) result.second - (double) expectedLength) / (double) expectedLength * 100.0;
            this->outputFile << error << "%;";
            std::cout << error << "% ";

            avgError += error;
            forAvgCount++;

            if (result.second < bestTourCost) {
                bestTourCost = result.second;
                bestPath = result.first;
            }

            // Wyświetl najlepszą trasę i jej koszt
            this->outputFile << result.second << ";[" << result.first[0];
            std::cout << result.second << " [" << result.first[0];
            for (int i = 1; i < result.first.size();
                 i++) {
                this->outputFile << " " << result.first[i];
                std::cout << " " << result.first[i];
            }
            // dopisz nawias koncowy
            this->outputFile << "]" << std::endl;
            std::cout << "]" << std::endl;
        }
        avgError /= (double) forAvgCount;
        avgTime /= (double) forAvgCount;


        this->statFile << this->sourceFileName << ";" << this->tspType << ";" << this->asType << ";" << expectedLength << ";" << bestTourCost
                       << ";" << avgError << ";" << avgTime << ";";
        std::cout << this->sourceFileName << " " << this->tspType << " " << this->asType << " " << expectedLength << " " << bestTourCost << " "
                  << " " << avgError << " " << " " << avgTime << " ";

        this->statFile << "[" << bestPath[0];
        std::cout << "[" << bestPath[0];
        for (int i = 1; i < bestPath.size();
             i++) {
            this->statFile << " " << bestPath[i];
            std::cout << " " << bestPath[i];
        }
        // dopisz nawias koncowy
        this->statFile << "]";
        std::cout << "]";

        this->outputFile << std::endl;
        this->statFile << std::endl;
        std::cout << std::endl;
    }

    /**
     * Wczytanie macierzy odległości z pliku
     */
    bool TSP::readSourceFile() {
        if (this->sourceFile.is_open()) {
            this->sourceFile.close();
        }

        this->sourceFile.open(sourceDirectory + this->sourceFileName);
        if (!this->sourceFile.is_open()) {
            std::cout << "Source file error: " + this->sourceFileName + " not found in " + sourceDirectory << std::endl;
            return false;
        }

        this->tspType = "TSP";

        std::cout << "## Reading file: " << this->sourceFileName << std::endl;
        this->sourceFile.seekg(0, std::ios::beg);
        int id;
        double x, y;

        int mode = 1;
        int size;

        int countEmpty = 0;

        std::string tmp;
        do {
            this->sourceFile >> tmp;
            if (tmp == "DIMENSION:") {
                this->sourceFile >> size;
            }

            if (tmp == "TYPE:") {
                this->sourceFile >> tmp;
                if (tmp == "ATSP") {
                    this->tspType = "ATSP";
                }
            }

            if (tmp == "LOWER_DIAG_ROW") {
                mode = 2;
            }
            if (tmp == "FULL_MATRIX") {
                mode = 3;
            }
            std::cout << "### TOKEN: " << tmp << std::endl;
            if (tmp.empty()) {
                countEmpty++;
            } else {
                countEmpty = 0;
            }

        } while (tmp != "NODE_COORD_SECTION" && tmp != "EDGE_WEIGHT_SECTION" && countEmpty < 10);

        if (mode == 2) {
            //LOWER_DIAG_ROW
            std::cout << "## Mode: LOWER_DIAG_ROW" << std::endl;
            std::vector<std::vector<double>> distances(size, std::vector<double>(size, 0.0));

            for (int i = 0; i < size; ++i) {
                for (int j = 0; j <= i; ++j) {
                    this->sourceFile >> distances[i][j];

                    distances[j][i] = distances[i][j];
                }
            }
            this->distances = distances;

            return true;
        }

        if (mode == 3) {
            //FULL_MATRIX
            std::cout << "## Mode: FULL_MATRIX" << std::endl;
            std::vector<std::vector<double>> distances(size, std::vector<double>(size, 0.0));

            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    this->sourceFile >> distances[i][j];

                    distances[j][i] = distances[i][j];
                }
            }
            this->distances = distances;

            return true;
        }

        std::cout << "## Mode: COORD_DISPLAY" << std::endl;
        std::vector<City> cities;
        while (this->sourceFile >> id >> x >> y) {
            cities.push_back({id, x, y});
        }

        this->sourceFile.close();

        int numCities = cities.size();
        std::vector<std::vector<double>> distances(numCities, std::vector<double>(numCities, 0.0));
        for (int i = 0; i < numCities; ++i) {
            for (int j = 0; j < numCities; ++j) {
                distances[i][j] = calculateDistance(cities[i], cities[j]);
                distances[j][i] =  distances[i][j];
            }
        }
        this->distances = distances;

        return true;
    }

    /**
     * Algorytm mrówkowy dla problemu TSP
     */
    std::pair<std::vector<int>, int> TSP::antAlgorithm() {
        int numCities = distances.size();
        if (numCities == 0) {
            throw std::runtime_error(std::string("## Empty city list error"));
        }
        std::cout << "## City count: " << numCities << std::endl;

        // inicjalizacja feromonow
        std::vector<std::vector<double>> pheromones(numCities, std::vector<double>(numCities, FEROMON_INITIAL_VALUE));
        std::cout << "## Feromones initialized" << std::endl;

        // Wywołanie algorytmu mrówkowego
        std::vector<int> bestTour = runAnts(pheromones);
        std::cout << "## Ants gone" << std::endl;

        double bestTourLength = 0.0;
        for (int j = 0; j < numCities; ++j) {
            bestTourLength += distances[bestTour[j]][bestTour[j + 1]];
        }

        std::cout << "## Result found" << std::endl;

        std::pair<std::vector<int>, int> result;
        result.first = bestTour;
        result.second = ceil(bestTourLength);
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
     * Ruchy mrówek
     */
    void TSP::antSteps(std::vector<std::vector<double>> &pheromones,
                       std::vector<int> &tour) {
        int numCities = distances.size();
        std::vector<bool> visited(numCities, false);

        // Początkowe miasto (startowe)
        int currentCity = getRandomInt(0, numCities-1);
        visited[currentCity] = true;
        tour[0] = currentCity;
        tour[numCities] = currentCity;

        // Wybór kolejnych miast
        for (int i = 1; i < numCities; ++i) {
            // Obliczenie prawdopodobieństw wyboru kolejnych miast
            std::vector<double> probabilities(numCities, 0.0);
            double totalProbability = 0.0;

            for (int j = 0; j < numCities; ++j) {
                // brak trasy
                if (distances[currentCity][j] == 0) {
                    continue;
                }

                // juz odwiedzono
                if (visited[j]) {
                    continue;
                }

                // Prawdopodobieństwo wyboru miasta j
                double pheromone = std::pow(pheromones[currentCity][j], ALPHA);
                double heuristic = std::pow(1.0 / distances[currentCity][j], BETA);
                probabilities[j] = pheromone * heuristic;

                totalProbability += probabilities[j];
            }

            // Wybór miasta na podstawie prawdopodobienstwa
            double randomValue = (rand() / (RAND_MAX + 1.0)) * totalProbability;
            double cumulativeProbability = 0.0;
            int chosenCity = -1;

            for (int j = 0; j < numCities; ++j) {
                // brak trasy
                if (distances[currentCity][j] == 0) {
                    continue;
                }

                // juz odwiedzono
                if (visited[j]) {
                    continue;
                }

                cumulativeProbability += probabilities[j];
                if (cumulativeProbability >= randomValue) {
                    chosenCity = j;
                    break;
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
    std::vector<int> TSP::runAnts(std::vector<std::vector<double>> &pheromones) {
        std::vector<int> bestTour(0);
        int bestTourLength = std::numeric_limits<int>::max();
        int numCities = distances.size();
        std::vector<std::vector<double>> deltaPheromones(numCities, std::vector<double>(numCities, 0.0));

        for (int iteration = 0; iteration < numIterations; iteration++) {
            // Symulacja ruchu mrówek
            for (int ant = 0; ant < numAnts; ++ant) {
                std::vector<int> tour(numCities+1, 0);

                // Implementacja ruchu mrówki
                antSteps(pheromones, tour);

                // Obliczanie długości trasy mrówki
                double tourLength = 0.0;
                for (int i = 0; i < numCities; ++i) {
                    tourLength += distances[tour[i]][tour[i + 1]];
                }

                // Aktualizacja deltaPheromones
                if(this->asType == "CAS"){
                    for (int i = 0; i < numCities; ++i) {
                        deltaPheromones[tour[i]][tour[i + 1]] += 1.0 / tourLength;
                        deltaPheromones[tour[i + 1]][tour[i]] += 1.0 / tourLength;
                    }
                }
                else if(this->asType == "DAS"){
                    for (int i = 0; i < numCities; ++i) {
                        deltaPheromones[tour[i]][tour[i + 1]] += 1.0;
                        deltaPheromones[tour[i + 1]][tour[i]] += 1.0;
                    }
                }


                if(tourLength < bestTourLength){
                    bestTourLength = tourLength;
                    bestTour = tour;
                }
            }
          
            // Aktualizacja i parowanie feromonów globalnie
            updatePheromones(pheromones, deltaPheromones);
        }

        return bestTour;
    }

    /**
    * Generator liczb losowych
    */
    int TSP::getRandomInt(int minValue, int maxValue) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distribution(minValue, maxValue);
        return distribution(gen);
    }

} // PEA

