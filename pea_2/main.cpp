#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <iomanip>

using namespace std;


class TSP {
public:
    void handleConfigFile();

    TSP(string configFileName);

    ~TSP();

private:
    void initFiles();

    void handleConfigLine(string line);

    bool readSourceFile();

    pair<vector<int>, int> branchAndBoundTSP();

    void ProcessBounds(int curr_bound, int curr_weight, int level, vector<int> &curr_path);

    int firstMinValue(int i);

    int secondMinValue(int i);

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


int main() {
    TSP tsp("config.ini");
    tsp.handleConfigFile();
    return 0;
}


TSP::TSP(string configFileName) {
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
        cout << "Config file error. Make sure that file config.ini exists. " << endl;
        exit(1);
    }

    int configLineCount = this->countConfigLines();
    if (configLineCount == -1) {
        cout << "Config file error. Make sure that file config.ini exists. " << endl;
        exit(1);
    }
    if (configLineCount == 0) {
        cout << "Config file error. Make sure that file config.ini is not empty. " << endl;
        exit(1);
    }

    this->setOutputFileName();
    this->outputFile.open(outputFileName, ios::trunc);
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
    this->configFile.seekg(0, ios::beg);

    int count = 0;
    string line;
    while (getline(this->configFile, line)) {
        // Check if the line has any non-whitespace characters.
        if (line.find_first_not_of(" \t") != string::npos) {
            count++;
        }
    }

    return count;
}

/**
 * @brief Funkcja sprawdza plik konfiguracyjny, ustawia nazwe pliku wynikowego
 *
 * @return string
 */
void TSP::setOutputFileName() {
    if (!this->configFile.is_open()) {
        return;
    }

    this->configFile.clear();
    this->configFile.seekg(0, ios::beg);
    string line;

    while (getline(this->configFile, line)) {
        istringstream iss(line);
        vector<string> tokens;
        string token;

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
        cout << "Config file error." << endl;
        exit(1);
    }

    if (!this->outputFile.is_open()) {
        cout << "Output file error." << endl;
        exit(1);
    }

    this->configFile.clear();
    this->configFile.seekg(0, ios::beg);
    string line;

    while (getline(this->configFile, line)) {
        this->handleConfigLine(line);
    }

    return;
}

void TSP::handleConfigLine(string line) {
    istringstream iss(line);
    vector<string> tokens;
    string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() < 2) {
        return;
    }

    this->sourceFileName = tokens[0];
    int repeatCount, expectedLength;
    vector<int> values;

    istringstream(tokens[1]) >> repeatCount;
    istringstream(tokens[2]) >> expectedLength;

    // wyswietlenie linii informacyjnej
    this->outputFile << this->sourceFileName << " " << expectedLength << " ";
    cout << this->sourceFileName << " " << expectedLength << " ";

    for (size_t i = 3; i < tokens.size(); i++) {
        this->outputFile << tokens[i] << " ";
        cout << tokens[i] << " ";
    }
    this->outputFile << endl;
    cout << endl;

    // Wczytaj dane z pliku
    if (!this->readSourceFile()) {
        return;
    }

    for (int i = 1; i <= repeatCount; i++) {
        // Start pomiaru czasu
        auto start_time = chrono::high_resolution_clock::now();

        // Algorytm
        pair<vector<int>, int> result = this->branchAndBoundTSP();

        // Wynik pomiaru czasu
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
        long double miliseconds = duration.count() / 1000.0;
        this->outputFile << fixed << setprecision(4) << miliseconds << " " << result.second << " [";
        cout << fixed << setprecision(4) << miliseconds << " " << result.second << " [";

        // Wyświetl najlepszą trasę i jej koszt
        this->outputFile << result.first[0];
        cout << result.first[0];
        for (int i=1;i<=this->sourceMatrix.size();i++) {
            this->outputFile << " " << result.first[i];
            cout << " " << result.first[i];
        }

        // dopisz nawias koncowy
        this->outputFile << "]" << endl;
        cout << "]" << endl;
    }
    this->outputFile << endl;
    cout << endl;

    return;
}

// Funkcja do wczytywania macierzy odległości z pliku
bool TSP::readSourceFile() {
    this->sourceFile.open(sourceDirectory + this->sourceFileName);
    if (!this->sourceFile.is_open()) {
        cout << "Source file error: " + this->sourceFileName + " not found in " + sourceDirectory << endl;
        return false;
    }

    // Liczba miast
    int n;
    this->sourceFile >> n;

    if (n < 0) {
        cout << "Source file format error." << endl;
        return false;
    }

    // Dane z pliku
    vector<vector<int>> matrix(n, vector<int>(n, 0));

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


int TSP::firstMinValue(int i) {
    int min_val = INT_MAX;
    for (int j = 0; j < this->sourceMatrix.size(); j++) {
        if (this->sourceMatrix[i][j] < min_val && i != j) {
            min_val = this->sourceMatrix[i][j];
        }
    }
    return min_val;
}

int TSP::secondMinValue(int i) {
    int first = INT_MAX, second = INT_MAX;
    for (int j = 0; j < this->sourceMatrix.size(); j++) {
        if (i == j) {
            continue;
        }

        if (this->sourceMatrix[i][j] <= first) {
            second = first;
            first = this->sourceMatrix[i][j];
        } else if (this->sourceMatrix[i][j] <= second && this->sourceMatrix[i][j] != first) {
            second = this->sourceMatrix[i][j];
        }
    }
    return second;
}


// Algorytm Branch and Bound dla problemu TSP
pair<vector<int>, int> TSP::branchAndBoundTSP() {
    int n = this->sourceMatrix.size();
    this->visited.assign(n, false);
    this->finalPath.assign(n + 1, 0);
    this->finalCost = INT_MAX;

    vector<int> current_path(n + 1, 0);
    int current_bound = 0;

    visited[0] = true;
    current_path.push_back(0);

    for (int i = 0; i < n; i++) {
        current_bound += (this->firstMinValue(i) + this->secondMinValue(i));
    }

    current_bound = (current_bound & 1) ? current_bound / 2 + 1 : current_bound / 2;

    this->ProcessBounds(current_bound, 0, 1, current_path);

    pair<vector<int>, int> result;
    result.first = this->finalPath;
    result.second = this->finalCost;
    return result;
}


// Funkcja pomocnicza przeszukujaca graf
void TSP::ProcessBounds(int curr_bound, int curr_weight, int level, vector<int> &curr_path) {
    int n = this->sourceMatrix.size();
    if (level == n) {
        if (this->sourceMatrix[curr_path[level - 1]][curr_path[0]] != 0) {
            int current_cost = curr_weight + this->sourceMatrix[curr_path[level - 1]][curr_path[0]];
            if (current_cost < this->finalCost) {
                this->finalPath = curr_path;
                this->finalCost = current_cost;
            }
        }
        return;
    }

    for (int i = 0; i < n; i++) {
        if (this->sourceMatrix[curr_path[level - 1]][i] != 0 && !this->visited[i]) {
            int temp = curr_bound;
            curr_weight += this->sourceMatrix[curr_path[level - 1]][i];

            if (level == 1)
                curr_bound -= ((this->firstMinValue(curr_path[level - 1]) + this->firstMinValue(i)) / 2);
            else
                curr_bound -= ((this->secondMinValue(curr_path[level - 1]) + this->firstMinValue(i)) / 2);

            if (curr_bound + curr_weight < this->finalCost) {
                curr_path[level] = i;
                this->visited[i] = true;
                this->ProcessBounds(curr_bound, curr_weight, level + 1, curr_path);
            }

            curr_weight -= this->sourceMatrix[curr_path[level - 1]][i];
            curr_bound = temp;
            this->visited.assign(n, false);
            for (int j = 0; j <= level - 1; j++) {
                this->visited[curr_path[j]] = true;
            }
        }
    }
}


