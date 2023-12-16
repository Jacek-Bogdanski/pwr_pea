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

#ifndef PEA_3_TSP_H
#define PEA_3_TSP_H

namespace PEA {

    class TSP {
    public:
        void handleConfigFile();

        TSP(std::string configFileName);

        ~TSP();

    private:
        void initFiles();

        void handleConfigLine(std::string line);

        bool readSourceFile();

        std::pair<std::vector<int>, int> SimulatedAnnealing();

        int countConfigLines();

        void setOutputFileName();

        std::string configFileName;
        std::string sourceDirectory = "source/";
        std::string outputFileName = "output.txt";
        std::string sourceFileName;

        std::vector<std::vector<int>> sourceMatrix;
        std::vector<bool> visited;
        std::vector<int> finalPath;
        int finalCost;

        std::ifstream configFile;
        std::ofstream outputFile;
        std::ifstream sourceFile;

        /**
         * @brief Ocena długości trasy
         * @param route
         * @param distanceMatrix
         * @return double
         */
        double evaluateRoute(const std::vector<int>& route, const std::vector<std::vector<int>>& distanceMatrix);
    };


} // PEA

#endif //PEA_3_TSP_H
