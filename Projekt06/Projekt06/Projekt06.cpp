#include <iostream>
#include "EnergyTree.h"
#include "FileManager.h"
#include "Analyzer.h"

// Funkcja pomocnicza do pobrania daty od użytkownika
std::tm inputDateTime() {
    std::tm t = {};
    std::cout << "Podaj rok (np. 2021): "; std::cin >> t.tm_year; t.tm_year -= 1900;
    std::cout << "Podaj miesiac (1-12): "; std::cin >> t.tm_mon; t.tm_mon -= 1;
    std::cout << "Podaj dzien (1-31): "; std::cin >> t.tm_mday;
    std::cout << "Podaj godzine (0-23): "; std::cin >> t.tm_hour;
    std::cout << "Podaj minute (0-59): "; std::cin >> t.tm_min;
    t.tm_sec = 0; t.tm_isdst = -1;
    return t;
}

int main() {
    EnergyTree tree;
    Analyzer analyzer(tree);
    int choice;

    do {
        std::cout << "\n--- MENU ---\n";
        std::cout << "1. Wczytaj CSV\n";
        std::cout << "2. Zapisz do pliku binarnego\n";
        std::cout << "3. Wczytaj z pliku binarnego (czysci obecne dane)\n";
        std::cout << "4. Analiza (Suma)\n";
        std::cout << "5. Analiza (Srednia)\n";
        std::cout << "0. Wyjscie\n";
        std::cout << "Wybor: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            // Zakładam nazwę pliku, w realnym programie można pobrać cin >> nazwa
            FileManager::loadCSV(tree, "Chart_Export.csv");
            break;
        }
        case 2:
            FileManager::saveBinary(tree, "data.bin");
            break;
        case 3:
            FileManager::loadBinary(tree, "data.bin");
            break;
        case 4:
        case 5: {
            std::cout << "Data poczatkowa:\n";
            std::tm start = inputDateTime();
            std::cout << "Data koncowa:\n";
            std::tm end = inputDateTime();

            std::cout << "Typ danych (1-Auto, 2-Export, 3-Import, 4-Pobor, 5-Produkcja): ";
            int typeInt;
            std::cin >> typeInt;
            ValueType type;
            if (typeInt >= 1 && typeInt <= 5)
                type = static_cast<ValueType>(typeInt - 1);
            else {
                std::cout << "Bledny typ.\n";
                break;
            }

            auto selector = Analyzer::getSelector(type); // Lambda retrieval

            double result = 0;
            if (choice == 4)
                result = analyzer.calculateSum(start, end, selector);
            else
                result = analyzer.calculateAvg(start, end, selector);

            std::cout << "Wynik: " << result << " W" << std::endl;
            break;
        }
        case 0:
            std::cout << "Koniec.\n";
            break;
        default:
            std::cout << "Nieznana opcja.\n";
        }
    } while (choice != 0);

    return 0;
}