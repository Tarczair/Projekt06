#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <iostream>
#include <fstream>
#include <iomanip>

struct Measurement {
    std::tm timestamp;
    double autoconsumption;
    double exportEnergy;
    double importEnergy;
    double consumption;
    double production;

    Measurement() : autoconsumption(0), exportEnergy(0), importEnergy(0), consumption(0), production(0) {
        timestamp = {};
    }

    // Pomocnicza funkcja do porównywania czasu
    bool operator<(const Measurement& other) const {
        if (timestamp.tm_year != other.timestamp.tm_year) return timestamp.tm_year < other.timestamp.tm_year;
        if (timestamp.tm_mon != other.timestamp.tm_mon) return timestamp.tm_mon < other.timestamp.tm_mon;
        if (timestamp.tm_mday != other.timestamp.tm_mday) return timestamp.tm_mday < other.timestamp.tm_mday;
        if (timestamp.tm_hour != other.timestamp.tm_hour) return timestamp.tm_hour < other.timestamp.tm_hour;
        return timestamp.tm_min < other.timestamp.tm_min;
    }

    // Serializacja do pliku binarnego
    void serialize(std::ofstream& ofs) const {
        ofs.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
        ofs.write(reinterpret_cast<const char*>(&autoconsumption), sizeof(autoconsumption));
        ofs.write(reinterpret_cast<const char*>(&exportEnergy), sizeof(exportEnergy));
        ofs.write(reinterpret_cast<const char*>(&importEnergy), sizeof(importEnergy));
        ofs.write(reinterpret_cast<const char*>(&consumption), sizeof(consumption));
        ofs.write(reinterpret_cast<const char*>(&production), sizeof(production));
    }

    // Deserializacja z pliku binarnego
    void deserialize(std::ifstream& ifs) {
        ifs.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
        ifs.read(reinterpret_cast<char*>(&autoconsumption), sizeof(autoconsumption));
        ifs.read(reinterpret_cast<char*>(&exportEnergy), sizeof(exportEnergy));
        ifs.read(reinterpret_cast<char*>(&importEnergy), sizeof(importEnergy));
        ifs.read(reinterpret_cast<char*>(&consumption), sizeof(consumption));
        ifs.read(reinterpret_cast<char*>(&production), sizeof(production));
    }
};

#endif