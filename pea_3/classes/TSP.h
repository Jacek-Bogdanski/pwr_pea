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

#ifndef PEA_2_TSP_H
#define PEA_2_TSP_H

namespace PEA {

    class TSP {
    public:
        void handleConfigFile();

        TSP(string configFileName);

        ~TSP();

    private:
        void initFiles();

        void handleConfigLine(string line);

        bool readSourceFile();

        pair<vector<int>, int> SimulatedAnnealing();

        int countConfigLines();

        void setOutputFileName();

        string configFileName;
        string sourceDirectory = "source/";
        string outputFileName = "output.txt";
        string sourceFileName;

        vector<vector<int>> sourceMatrix;
        vector<bool> visited;
        vector<int> finalPath;
        int finalCost;

        ifstream configFile;
        ofstream outputFile;
        ifstream sourceFile;
    };


} // PEA

#endif //PEA_2_TSP_H
