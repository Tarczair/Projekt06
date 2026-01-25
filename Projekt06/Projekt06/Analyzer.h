// Analyzer.h
#ifndef ANALYZER_H
#define ANALYZER_H
#include "EnergyTree.h"
#include <functional>

enum class DataType { AUTO, EXPORT, IMPORT, CONS, PROD };

class Analyzer {
    EnergyTree& tree;
public:
    Analyzer(EnergyTree& t) : tree(t) {}
    static std::function<double(const Measurement&)> getSelector(DataType type);

    double getSum(std::tm s, std::tm e, DataType type);
    double getAvg(std::tm s, std::tm e, DataType type);
    void search(DataType type, double value, double tolerance, std::tm s, std::tm e);
    void printRange(std::tm s, std::tm e);
    void compare(std::tm s1, std::tm e1, std::tm s2, std::tm e2, DataType type);
};
#endif