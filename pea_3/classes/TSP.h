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
        void handleConfigFile();

        TSP(std::string configFileName);

        ~TSP();

    private:
        std::string configFileName;
        std::string sourceDirectory = "source/";
        std::string outputFileName = "output.txt";
        std::string sourceFileName;

        std::ifstream configFile;
        std::ofstream outputFile;
        std::ifstream sourceFile;

        std::vector<std::vector<int>> sourceMatrix;


        void initFiles();

        void handleConfigLine(std::string line);

        bool readSourceFile();

        int countConfigLines();

        void setOutputFileName();

        std::pair<std::vector<int>, int> SimulatedAnnealing(const std::vector<std::vector<int>>& distanceMatrix, double initialTemperature, double coolingRate, int epochs);

        /**
         * @brief Ocena długości trasy
         * @param route
         * @param distanceMatrix
         * @return double
         */
        double evaluateRoute(const std::vector<int>& route, const std::vector<std::vector<int>>& distanceMatrix);

        /**
         * @brief Generowanie losowej trasy
         * @return Losowa trasa
         */
        std::vector<int> generateRandomRoute(int n);

        /**
         * @brief Zamiana miast w trasie
         */
        void swapCities(std::vector<int>& route, int index1, int index2);
    };


} // PEA

#endif //PEA_3_TSP_H
