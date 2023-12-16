//
// Autor: Jacek Bogdański | PWR 2023
//

#include "classes/TSP.h"

int main() {
    PEA::TSP tsp("config.ini");
    tsp.handleConfigFile();
    return 0;
}
