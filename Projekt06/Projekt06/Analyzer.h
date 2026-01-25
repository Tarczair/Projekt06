#ifndef ANALYZER_H
#define ANALYZER_H

#include "EnergyTree.h"
#include <functional>
#include <ctime>

enum class ValueType {
    AUTOCONSUMPTION, EXPORT, IMPORT, CONSUMPTION, PRODUCTION
};

class Analyzer {
private:
    EnergyTree& tree;

    // Helper do porównywania czasu (tm)
    bool isBetween(const std::tm& current, const std::tm& start, const std::tm& end);
    time_t tmToTimeT(const std::tm& t);

public:
    Analyzer(EnergyTree& t) : tree(t) {}

    // Metoda generyczna wykorzystuj?ca lambd? do ekstrakcji pola
    double calculateSum(const std::tm& start, const std::tm& end,
        std::function<double(const Measurement&)> selector);

    double calculateAvg(const std::tm& start, const std::tm& end,
        std::function<double(const Measurement&)> selector);

    // Gettery do Lambd (u?atwiaj? wybór pola w menu)
    static std::function<double(const Measurement&)> getSelector(ValueType type);
};

#endif#pragma once
