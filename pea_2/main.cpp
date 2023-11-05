#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <iomanip>

using namespace std;

/**
 * @brief Funkcja sprawdza plik konfiguracyjny, oblicza ile linii kodu posiada
 * 
 * @return int 
 */
int countConfigLines() {
    std::ifstream file("/source/config.ini");
    if (!file.is_open()) {
        return -1;  
    }

    int count = 0;
    std::string line;

    while (std::getline(file, line)) {
        // Check if the line has any non-whitespace characters.
        if (line.find_first_not_of(" \t") != std::string::npos) {
            count++;
        }
    }

    file.close();
    return count;
}



// Funkcja do wczytywania macierzy odległości z pliku
vector<vector<int>> readDistanceMatrix(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Nie można otworzyć pliku: " << filename << endl;
        exit(1);
    }

    int n;
    file >> n;
    vector<vector<int>> matrix(n, vector<int>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            file >> matrix[i][j];
        }
    }

    file.close();
    return matrix;
}

// Algorytm Branch and Bound dla problemu TSP
void branchAndBoundTSP(const vector<vector<int>>& matrix) {
    int n = matrix.size();
    vector<bool> visited(n, false);
    vector<int> current_path;
    int current_cost = 0;
    int final_cost = INT_MAX;
    vector<int> final_path;

    visited[0] = true;
    current_path.push_back(0);

    function<void(int)> bnb = [&](int node) {
        if (current_path.size() == n) {
            current_cost += matrix[node][0];
            if (current_cost < final_cost) {
                final_cost = current_cost;
                final_path = current_path;
            }
            current_cost -= matrix[node][0];
            return;
        }

        int lb = 0;
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                lb += matrix[node][i];
            }
        }

        if (current_cost + lb >= final_cost) {
            return;
        }

        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                visited[i] = true;
                current_path.push_back(i);
                current_cost += matrix[node][i];
                bnb(i);
                visited[i] = false;
                current_path.pop_back();
                current_cost -= matrix[node][i];
            }
        }
    };

    bnb(0);

    cout << "Optymalna trasa: ";
    for (int city : final_path) {
        cout << city << " ";
    }
    cout << "0" << endl;
    cout << "Koszt trasy: " << final_cost << endl;
}

int main() {

    int result = countConfigLines();
    if (result != -1) {
        std::cout << "Number of lines with content: " << result << std::endl;
    }
    
    // Odczyt pliku konfiguracyjnego
    ifstream env_file("/source/config.ini");
    if(!env_file.is_open()){
        cout << "Config file error. Make sure that file config.ini is located in /source directory. " << endl;
    }

    ofstream outputFile;

    // Read the file line by line and extract values.
    string line;
    while (getline(env_file, line)) {
        // Split the line by space.
        istringstream iss(line);
        vector<string> tokens;
        string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.size() == 1 && !outputFile.is_open()) {
            outputFile.open(tokens[0], ios::trunc);
            if (!outputFile.is_open()) {
                cout << "Błąd pliku wynikowego" << endl;
            }
        }
    }

    env_file.clear();
    env_file.seekg(0,ios::beg);

    while (getline(env_file, line)) {
        // Split the line by space.
        istringstream iss(line);
        vector<string> tokens;
        string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.size() >= 4) {
            string filename = tokens[0];
            int repeatCount, expectedLength;
            vector<int> values;

            istringstream(tokens[1]) >> repeatCount;
            istringstream(tokens[2]) >> expectedLength;

            outputFile<<filename<<" "<<expectedLength<<" ";
            cout<<filename<<" "<<expectedLength<<" ";
            for (size_t i = 3; i < tokens.size(); i++) {
                outputFile<<tokens[i]<<" ";
                cout<<tokens[i]<<" ";
            }
            outputFile << endl;
            cout << endl;

            // Wczytaj dane z pliku
            ifstream plik(filename);
            if (!plik.is_open()) {
                cout << "Błąd pliku zrodlowego" << endl;
                return 0;
            }

            // Liczba miast
            int n;
            plik >> n;

            if (n < 0) {
                cout << "Błąd danych" << endl;
                return 0;
            }

            // Dane z pliku
            vector<vector<int>> macierz_odleglosci(n, vector<int>(n, 0));

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    plik >> macierz_odleglosci[i][j];
                }
            }

            // Zamknij plik
            plik.close();

            for(int i=1;i<=repeatCount;i++){
                // Start pomiaru czasu
                auto start_time = chrono::high_resolution_clock::now();

                pair<vector<int>, int> rozwiazanie = BruteForceTSP(macierz_odleglosci);


                // Wynik pomiaru czasu
                auto end_time = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::microseconds>(end_time-start_time);
                long double miliseconds = duration.count()/1000.0;
                outputFile<<fixed<<setprecision(4) << miliseconds<<" "<<rozwiazanie.second<<" [";
                cout<<miliseconds<<" "<<rozwiazanie.second<<" [";

                // Wyświetl najlepszą trasę i jej koszt
                for (int miasto: rozwiazanie.first) {
                    outputFile << miasto << " ";
                    cout << miasto << " ";
                }

                // dopisz punkt początkowy
                outputFile << "0]" << endl;
                cout << "0]" << endl;
            }
            outputFile << endl;
        }
    }

    // zamknij pliki
    env_file.close();
    outputFile.close();

    
    string filename = "/source/tsp_10.txt";
    vector<vector<int>> distanceMatrix = readDistanceMatrix(filename);
    branchAndBoundTSP(distanceMatrix);
    return 0;
}