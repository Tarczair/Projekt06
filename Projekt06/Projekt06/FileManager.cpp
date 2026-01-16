#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <vector>

// Helper do parsowania daty i godziny
// Format w pliku: DD.MM.YYYY HH:MM
std::tm parseDateTime(const std::string& dateStr, const std::string& timeStr) {
    std::tm t = {};
    std::stringstream ss(dateStr + " " + timeStr);
    char delim;
    ss >> t.tm_mday >> delim >> t.tm_mon >> delim >> t.tm_year >> t.tm_hour >> delim >> t.tm_min;
    t.tm_mon -= 1;     // 0-11
    t.tm_year -= 1900; // lata od 1900
    t.tm_sec = 0;
    t.tm_isdst = -1;
    return t;
}

void FileManager::loadCSV(EnergyTree& tree, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku CSV!" << std::endl;
        return;
    }

    std::string line;
    // Pominiecie pierwszej linii
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> parts;

        // Zak³adamy separator œrednik ';' (typowy dla polskich CSV) lub przecinek
        while (std::getline(ss, segment, ';')) {
            parts.push_back(segment);
        }
        // Fallback dla przecinka, jeœli œrednik nie zadzia³a³
        if (parts.size() < 2) {
            parts.clear();
            std::stringstream ss2(line);
            while (std::getline(ss2, segment, ',')) {
                parts.push_back(segment);
            }
        }

        if (parts.size() < 6) continue;

        // Parsowanie (Data i Godzina s¹ czêsto w 1 kolumnie lub 2)
        // Zak³adam strukturê: Data Godzina; Auto; Eksport; Import; Pobór; Produkcja
        // Czasem CSV rozdziela datê i godzinê spacj¹ wewn¹trz jednej kolumny

        std::string dateTimeStr = parts[0];
        // Rozdzielenie daty i godziny spacj¹
        size_t spacePos = dateTimeStr.find(' ');
        std::string datePart = dateTimeStr.substr(0, spacePos);
        std::string timePart = (spacePos != std::string::npos) ? dateTimeStr.substr(spacePos + 1) : "00:00";

        auto m = std::make_unique<Measurement>();
        m->timestamp = parseDateTime(datePart, timePart);

        try {
            m->autoconsumption = std::stod(parts[1]);
            m->exportEnergy = std::stod(parts[2]);
            m->importEnergy = std::stod(parts[3]);
            m->consumption = std::stod(parts[4]);
            m->production = std::stod(parts[5]);
            tree.addMeasurement(std::move(m));
        }
        catch (...) {
            std::cerr << "B³¹d parsowania linii: " << line << std::endl;
        }
    }
    file.close();
    std::cout << "Wczytano dane CSV." << std::endl;
}

void FileManager::saveBinary(const EnergyTree& tree, const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open()) return;

    // Przechodzimy iteratorem i zapisujemy p³ask¹ listê
    // Alternatywnie: zapisujemy strukturê drzewiast¹ (trudniejsze)
    // Proœciej: Zapisz liczbê rekordów, potem rekordy

    // Policz rekordy (nieoptymalne, ale proste)
    // W prawdziwym iteratorze nie mamy metody size() dla ca³oœci ³atwo dostêpnej
    // Zapiszmy marker koñca pliku lub po prostu strumieñ danych

    // Poniewa¿ mamy iterator, u¿yjmy go
    // Uwaga: Iterator w EnergyTree jest const, trzeba dodaæ const do metod begin/end

    // Brutal force const cast hack for simplicity because iterator was defined inside class without const overload
    // W "Clean Code" powinniœmy dodaæ wersjê const_iterator.
    auto& nonConstTree = const_cast<EnergyTree&>(tree);

    for (auto it = nonConstTree.begin(); it != nonConstTree.end(); ++it) {
        it->serialize(ofs);
    }
    ofs.close();
    std::cout << "Zapisano do pliku binarnego." << std::endl;
}

void FileManager::loadBinary(EnergyTree& tree, const std::string& filename) {
    tree.clear(); // Wymóg: odczyt do pustego programu
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku binarnego!" << std::endl;
        return;
    }

    // Czytamy dopóki plik ma dane
    while (ifs.peek() != EOF) {
        auto m = std::make_unique<Measurement>();
        m->deserialize(ifs);
        if (ifs.fail()) break;
        tree.addMeasurement(std::move(m));
    }
    ifs.close();
    std::cout << "Wczytano z pliku binarnego." << std::endl;
}