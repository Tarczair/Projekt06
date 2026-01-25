#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <iostream>
#include <fstream>
#include <ctime>

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

    // Porównanie dla sortowania i unikalnoœci
    bool operator<(const Measurement& other) const {
        return tmToTime() < other.tmToTime();
    }

    bool operator==(const Measurement& other) const {
        return tmToTime() == other.tmToTime();
    }

    time_t tmToTime() const {
        std::tm temp = timestamp;
        return std::mktime(&temp);
    }

    void serialize(std::ofstream& ofs) const {
        ofs.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
        ofs.write(reinterpret_cast<const char*>(&autoconsumption), sizeof(autoconsumption));
        ofs.write(reinterpret_cast<const char*>(&exportEnergy), sizeof(exportEnergy));
        ofs.write(reinterpret_cast<const char*>(&importEnergy), sizeof(importEnergy));
        ofs.write(reinterpret_cast<const char*>(&consumption), sizeof(consumption));
        ofs.write(reinterpret_cast<const char*>(&production), sizeof(production));
    }

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