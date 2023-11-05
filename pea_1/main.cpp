#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <climits>
#include <sstream>
#include <chrono>
#include <iomanip>


using namespace std;

/**
 * Jacek Bogdański - PWR 2023
 */

/**
 * Funkcja obliczająca koszt trasy
 * @param graf
 * @param trasa
 * @return koszt_trasy
 */
int ObliczKosztTrasy(const vector<vector<int>> &graf, const vector<int> &trasa);

/**
 * Funkcja rozwiązująca problem TSP metodą Brute-Force
 * @param graf
 * @return pair <najlepsza_trasa, najlepszy_koszt>
 */
pair<vector<int>, int> BruteForceTSP(const vector<vector<int>> &graf);

int main(int argc, char *argv[]) {

    ifstream env_file(".env");
    if(!env_file.is_open()){
        cout << "Błąd pliku srodowiskowego" << endl;
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
}


/**
 * Funkcja obliczająca koszt trasy
 * @param graf
 * @param trasa
 * @return koszt_trasy
 */
int ObliczKosztTrasy(const vector<vector<int>> &graf, const vector<int> &trasa) {
    int koszt = 0;
    int n = trasa.size();
    int miasto_aktualne;
    int miasto_nastepne;

    for (int i = 0; i < n - 1; i++) {
        miasto_aktualne = trasa[i];
        miasto_nastepne = trasa[i + 1];
        koszt += graf[miasto_aktualne][miasto_nastepne];
    }

    // droga powrotna
    miasto_aktualne = trasa[n-1];
    miasto_nastepne = 0;
    koszt += graf[miasto_aktualne][miasto_nastepne];

    return koszt;
}

/**
 * Funkcja rozwiązująca problem TSP metodą Brute-Force
 * @param graf
 * @return pair <najlepsza_trasa, najlepszy_koszt>
 */
pair<vector<int>, int> BruteForceTSP(const vector<vector<int>> &graf) {
    // Inicjalizuj zmienne (wektor permutacji, najlepsza trasa, najlepszy koszt)
    vector<int> permutacja;
    vector<int> najlepsza_trasa;
    int najlepszy_koszt = INT_MAX;
    int n = graf.size();

    // Generuj permutacje (początkowa permutacja 0, 1, 2, ..., n-1)
    for (int i = 0; i < n; i++) {
        permutacja.push_back(i);
    }
    // Dla każdej permutacji:

    do {
        // Oblicz koszt trasy
        int koszt_trasy = ObliczKosztTrasy(graf, permutacja);

        // Porównaj koszt z najlepszym kosztem
        if (koszt_trasy < najlepszy_koszt) {
            // Jeśli koszt jest niższy: Zaktualizuj najlepszy koszt i najlepszą trasę

            najlepszy_koszt = koszt_trasy;
            najlepsza_trasa = permutacja;
        }

        // Przejdź do następnej permutacji, powtarzaj dopóki wszystkie nie sprawdzone
    } while (next_permutation(permutacja.begin() + 1, permutacja.end()));

    // Zakończ
    return make_pair(najlepsza_trasa, najlepszy_koszt);
}
