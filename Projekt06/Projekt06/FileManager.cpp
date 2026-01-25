/**
 * @file FileManager.cpp
 * @brief Implementacja metod klasy FileManager odpowiedzialnej za operacje wejscia/wyjscia.
 *
 * Plik zawiera definicje funkcji sluzacych do importu danych z plikow tekstowych (CSV)
 * oraz zapisu i odczytu stanu aplikacji do plikow binarnych. Dodatkowo obslugiwane
 * jest logowanie przebiegu operacji importu.
 */

#define _CRT_SECURE_NO_WARNINGS
#include "FileManager.h"
#include <sstream>
#include <iomanip>

 /**
  * @brief Generuje biezacy znacznik czasowy jako lancuch znakow.
  *
  * Funkcja pomocnicza uzywana do tworzenia unikalnych nazw plikow logow.
  * Pobiera aktualny czas systemowy i formatuje go w postaci "RRRRMMDD_GGMMSS".
  * Korzysta z bezpiecznej funkcji localtime_s (standard MSVC/C11).
  *
  * @return std::string Znacznik czasu, np. "20231027_153000". W przypadku bledu zwraca "00000000_000000".
  */
std::string FileManager::getTimestampStr() {
    auto t = std::time(nullptr);
    std::tm tm;

    // localtime_s zwraca 0 w przypadku sukcesu (standard MSVC)
    if (localtime_s(&tm, &t) != 0) {
        return "00000000_000000"; // Fallback w razie bledu
    }

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

/**
 * @brief Wczytuje dane pomiarowe z pliku CSV do drzewa.
 *
 * Funkcja otwiera plik CSV, pomija pierwszy wiersz (naglowek) i parsuje kolejne linie.
 * Dla kazdej linii:
 * 1. Rozdziela dane separatorem srednika (;).
 * 2. Konwertuje date i czas oraz wartosci liczbowe (double).
 * 3. Tworzy obiekt Measurement i probuje dodac go do EnergyTree.
 *
 * Podczas dzialania tworzone sa dwa pliki logow z unikalnym znacznikiem czasu:
 * - log_DATA_CZAS.txt: Zawiera informacje o kazdej przetworzonej linii (sukces lub blad).
 * - log_error_DATA_CZAS.txt: Zawiera wylacznie informacje o bledach (np. bledny format, duplikat).
 *
 * @param tree Referencja do drzewa, do ktorego beda dodawane pomiary.
 * @param filename Sciezka do pliku CSV.
 */
void FileManager::loadCSV(EnergyTree& tree, const std::string& filename) {
    std::ifstream file(filename);
    std::string ts = getTimestampStr();
    std::ofstream logAll("log_" + ts + ".txt");
    std::ofstream logErr("log_error_" + ts + ".txt");

    int valid = 0, invalid = 0;
    std::string line;
    std::getline(file, line); // Pomin naglowek

    while (std::getline(file, line)) {
        if (line.empty()) { invalid++; continue; }
        std::stringstream ss(line);
        std::string part;
        std::vector<std::string> parts;
        // Rozdzielanie linii po srednikach
        while (std::getline(ss, part, ';')) parts.push_back(part);

        try {
            // Sprawdzenie czy linia ma wystarczajaca liczbe kolumn
            if (parts.size() < 6) throw std::runtime_error("Niepelna linia");

            auto m = std::make_unique<Measurement>();
            std::stringstream dss(parts[0]);
            char d;
            // Parsowanie formatu daty: DD.MM.RRRR GG:MM
            dss >> m->timestamp.tm_mday >> d >> m->timestamp.tm_mon >> d >> m->timestamp.tm_year >> m->timestamp.tm_hour >> d >> m->timestamp.tm_min;

            // Korekta dla struktury std::tm
            m->timestamp.tm_mon -= 1;   // Miesiace 0-11
            m->timestamp.tm_year -= 1900; // Lata od 1900

            // Konwersja wartosci liczbowych
            m->autoconsumption = std::stod(parts[1]);
            m->exportEnergy = std::stod(parts[2]);
            m->importEnergy = std::stod(parts[3]);
            m->consumption = std::stod(parts[4]);
            m->production = std::stod(parts[5]);

            // Proba dodania do drzewa (zwraca false jesli duplikat daty)
            if (tree.addMeasurement(std::move(m))) {
                valid++;
                logAll << "OK: " << line << "\n";
            }
            else {
                throw std::runtime_error("Duplikat");
            }
        }
        catch (std::exception& e) {
            invalid++;
            // Logowanie bledow do obu plikow
            logAll << "ERR: " << e.what() << " | " << line << "\n";
            logErr << "ERR: " << e.what() << " | " << line << "\n";
        }
    }
    std::cout << "Wczytano: " << valid << ", Blednych: " << invalid << "\n";
}

/**
 * @brief Zapisuje stan calego drzewa do pliku binarnego.
 *
 * Wykorzystuje iterator drzewa (EnergyTree::Iterator) aby przejsc sekwencyjnie
 * przez wszystkie pomiary i wywolac na nich metode serialize().
 *
 * @param tree Referencja do drzewa danych.
 * @param filename Nazwa pliku wyjsciowego.
 */
void FileManager::saveBinary(EnergyTree& tree, const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    for (auto it = tree.begin(); it != tree.end(); ++it) it->serialize(ofs);
}

/**
 * @brief Odtwarza stan drzewa z pliku binarnego.
 *
 * Funkcja najpierw czysci biezaca zawartosc drzewa. Nastepnie w petli
 * odczytuje kolejne obiekty Measurement az do napotkania konca pliku (EOF).
 * Obiekty sa deserializowane i dodawane do struktury drzewiastej.
 *
 * @param tree Referencja do drzewa danych (zostanie wyczyszczone przed wczytaniem).
 * @param filename Nazwa pliku wejsciowego.
 */
void FileManager::loadBinary(EnergyTree& tree, const std::string& filename) {
    tree.clear();
    std::ifstream ifs(filename, std::ios::binary);
    while (ifs.peek() != EOF) {
        auto m = std::make_unique<Measurement>();
        m->deserialize(ifs);
        tree.addMeasurement(std::move(m));
    }
}