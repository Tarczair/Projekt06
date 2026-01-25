/**
 * @file Projekt06.cpp
 * @brief Glowny plik zrodlowy programu do analizy danych energetycznych.
 *
 * Plik zawiera funkcje main, ktora steruje calym programem. Odpowiada za
 * interakcje z uzytkownikiem poprzez konsolowe menu, inicjalizacje glownych
 * obiektow (EnergyTree, Analyzer) oraz wywoływanie odpowiednich funkcji
 * w zaleznosci od wyboru uzytkownika.
 */

#include <iostream>
#include "FileManager.h"
#include "Analyzer.h"

 /**
  * @brief Pobiera od uzytkownika date i czas w formacie numerycznym.
  *
  * Funkcja wyswietla prosbe o podanie roku, miesiaca, dnia, godziny i minuty.
  * Wczytane wartosci sa nastepnie dostosowywane do formatu wymaganego przez
  * strukture std::tm (rok pomniejszony o 1900, miesiac pomniejszony o 1).
  * Pole tm_isdst jest ustawiane na -1, aby system sam okreslil czas letni/zimowy.
  *
  * @return std::tm Struktura zawierajaca wprowadzona date i czas.
  */
std::tm inputTime() {
    std::tm t = {};
    std::cout << "RRRR MM DD GG MM: ";
    std::cin >> t.tm_year >> t.tm_mon >> t.tm_mday >> t.tm_hour >> t.tm_min;
    t.tm_year -= 1900; t.tm_mon -= 1; t.tm_isdst = -1;
    return t;
}

/**
 * @brief Glowna funkcja programu (punkt wejscia).
 *
 * Funkcja tworzy instancje drzewa danych (EnergyTree) oraz analizatora (Analyzer).
 * Uruchamia petle do-while, ktora wyswietla menu i oczekuje na wybor opcji
 * przez uzytkownika. Obsluguje nastepujace funkcjonalnosci:
 * - 1: Wczytanie danych z pliku CSV.
 * - 2: Zapis danych do pliku binarnego.
 * - 3: Odczyt danych z pliku binarnego.
 * - 4: Obliczenie sumy wartosci dla danego typu i przedzialu czasu.
 * - 5: Obliczenie sredniej wartosci dla danego typu i przedzialu czasu.
 * - 7: Wyszukiwanie rekordow o zadanej wartosci z okreslona tolerancja.
 * - 0: Wyjscie z programu.
 *
 * @return int Kod wyjscia (0 oznacza poprawne zakonczenie).
 */
int main() {
    EnergyTree tree;
    Analyzer analyzer(tree);
    int choice;
    do {
        std::cout << "\n1. CSV 2. Zapis Bin 3. Odczyt Bin 4. Suma 5. Srednia 6. Porownaj 7. Szukaj 0. Wyjscie\nWybor: ";
        std::cin >> choice;

        // Obsluga wczytywania pliku CSV
        if (choice == 1) FileManager::loadCSV(tree, "Chart_Export.csv");

        // Obsluga zapisu do pliku binarnego
        if (choice == 2) FileManager::saveBinary(tree, "data.bin");

        // Obsluga odczytu z pliku binarnego
        if (choice == 3) FileManager::loadBinary(tree, "data.bin");

        // Obsluga operacji analitycznych: Suma (4) i Srednia (5)
        if (choice == 4 || choice == 5) {
            std::tm s = inputTime(), e = inputTime();
            int type; std::cout << "Typ (0-4): "; std::cin >> type;
            if (choice == 4) std::cout << "Suma: " << analyzer.getSum(s, e, (DataType)type) << "\n";
            else std::cout << "Srednia: " << analyzer.getAvg(s, e, (DataType)type) << "\n";
        }

        // Obsluga wyszukiwania wartosci z tolerancja
        if (choice == 7) {
            std::tm s = inputTime(), e = inputTime();
            double v, t; std::cout << "Wartosc i tolerancja: "; std::cin >> v >> t;
            // Domyslnie szuka dla typu IMPORT (mozna zmienic w kodzie w razie potrzeby)
            analyzer.search(DataType::IMPORT, v, t, s, e);
        }
    } while (choice != 0);
    return 0;
}