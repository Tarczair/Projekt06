#include <iostream>
#include "FileManager.h"
#include "Analyzer.h"

std::tm inputTime() {
    std::tm t = {};
    std::cout << "RRRR MM DD GG MM: ";
    std::cin >> t.tm_year >> t.tm_mon >> t.tm_mday >> t.tm_hour >> t.tm_min;
    t.tm_year -= 1900; t.tm_mon -= 1; t.tm_isdst = -1;
    return t;
}

int main() {
    EnergyTree tree;
    Analyzer analyzer(tree);
    int choice;
    do {
        std::cout << "\n1. CSV 2. Zapis Bin 3. Odczyt Bin 4. Suma 5. Srednia 6. Porownaj 7. Szukaj 0. Wyjscie\nWybor: ";
        std::cin >> choice;
        if (choice == 1) FileManager::loadCSV(tree, "Chart_Export.csv");
        if (choice == 2) FileManager::saveBinary(tree, "data.bin");
        if (choice == 3) FileManager::loadBinary(tree, "data.bin");
        if (choice == 4 || choice == 5) {
            std::tm s = inputTime(), e = inputTime();
            int type; std::cout << "Typ (0-4): "; std::cin >> type;
            if (choice == 4) std::cout << "Suma: " << analyzer.getSum(s, e, (DataType)type) << "\n";
            else std::cout << "Srednia: " << analyzer.getAvg(s, e, (DataType)type) << "\n";
        }
        if (choice == 7) {
            std::tm s = inputTime(), e = inputTime();
            double v, t; std::cout << "Wartosc i tolerancja: "; std::cin >> v >> t;
            analyzer.search(DataType::IMPORT, v, t, s, e);
        }
    } while (choice != 0);
    return 0;
}