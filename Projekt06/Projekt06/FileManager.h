#pragma once
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "EnergyTree.h"
#include <string>

class FileManager {
public:
    static void loadCSV(EnergyTree& tree, const std::string& filename);
    static void saveBinary(const EnergyTree& tree, const std::string& filename);
    static void loadBinary(EnergyTree& tree, const std::string& filename);
};

#endif