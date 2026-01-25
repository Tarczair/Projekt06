#define _CRT_SECURE_NO_WARNINGS
#include "FileManager.h"
#include <sstream>
#include <iomanip>
std::string FileManager::getTimestampStr() {
    auto t = std::time(nullptr);
    std::tm tm;

    // localtime_s zwraca 0 w przypadku sukcesu (standard MSVC)
    if (localtime_s(&tm, &t) != 0) {
        return "00000000_000000"; // Fallback w razie b³êdu
    }

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

void FileManager::loadCSV(EnergyTree& tree, const std::string& filename) {
    std::ifstream file(filename);
    std::string ts = getTimestampStr();
    std::ofstream logAll("log_" + ts + ".txt");
    std::ofstream logErr("log_error_" + ts + ".txt");

    int valid = 0, invalid = 0;
    std::string line;
    std::getline(file, line); // Pomiñ nag³ówek

    while (std::getline(file, line)) {
        if (line.empty()) { invalid++; continue; }
        std::stringstream ss(line);
        std::string part;
        std::vector<std::string> parts;
        while (std::getline(ss, part, ';')) parts.push_back(part);

        try {
            if (parts.size() < 6) throw std::runtime_error("Niepe³na linia");
            auto m = std::make_unique<Measurement>();
            std::stringstream dss(parts[0]);
            char d;
            dss >> m->timestamp.tm_mday >> d >> m->timestamp.tm_mon >> d >> m->timestamp.tm_year >> m->timestamp.tm_hour >> d >> m->timestamp.tm_min;
            m->timestamp.tm_mon -= 1; m->timestamp.tm_year -= 1900;

            m->autoconsumption = std::stod(parts[1]);
            m->exportEnergy = std::stod(parts[2]);
            m->importEnergy = std::stod(parts[3]);
            m->consumption = std::stod(parts[4]);
            m->production = std::stod(parts[5]);

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
            logAll << "ERR: " << e.what() << " | " << line << "\n";
            logErr << "ERR: " << e.what() << " | " << line << "\n";
        }
    }
    std::cout << "Wczytano: " << valid << ", Blednych: " << invalid << "\n";
}

void FileManager::saveBinary(EnergyTree& tree, const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    for (auto it = tree.begin(); it != tree.end(); ++it) it->serialize(ofs);
}

void FileManager::loadBinary(EnergyTree& tree, const std::string& filename) {
    tree.clear();
    std::ifstream ifs(filename, std::ios::binary);
    while (ifs.peek() != EOF) {
        auto m = std::make_unique<Measurement>();
        m->deserialize(ifs);
        tree.addMeasurement(std::move(m));
    }
}