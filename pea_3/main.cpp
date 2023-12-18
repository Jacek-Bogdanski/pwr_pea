//
// Autor: Jacek Bogdański | PWR 2023
//

#include "classes/TSP.h"

int main() {
    srand((time(NULL)));
    PEA::TSP tsp("config.ini");
    tsp.handleConfigFile();
    return 0;
}
