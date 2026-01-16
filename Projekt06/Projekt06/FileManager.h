#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "EnergyTree.h"
#include <string>

class FileManager {
private:
    // Pomocnicza metoda do usuwania cudzys³owów z pól CSV
    static std::string cleanQuotes(std::string str);

public:
    static void loadCSV(EnergyTree& tree, const std::string& filename);
    static void saveBinary(const EnergyTree& tree, const std::string& filename);
    static void loadBinary(EnergyTree& tree, const std::string& filename);
};

#endif