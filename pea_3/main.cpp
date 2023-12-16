//
// Autor: Jacek Bogdański | PWR 2023
//

#include "classes/TSP.h"

int main() {
    srand(static_cast<unsigned>(time(0)));
    PEA::TSP tsp("config.ini");
    tsp.handleConfigFile();
    return 0;
}
