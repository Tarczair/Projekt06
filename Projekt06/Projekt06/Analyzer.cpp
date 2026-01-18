#include "Analyzer.h"
#include <ctime> // W standardzie C++ po prostu <ctime> i mktime, 
// ale tutaj u?yjemy pomocniczej funkcji wewn?trz klasy

time_t Analyzer::tmToTimeT(const std::tm& t) {
    std::tm temp = t;
    return std::mktime(&temp);
}

bool Analyzer::isBetween(const std::tm& current, const std::tm& start, const std::tm& end) {
    // Por?wnanie prostsze przez zamian? na time_t
    // Uwaga: mktime modyfikuje struktur?, wi?c kopiujemy w tmToTimeT
    time_t currT = tmToTimeT(current);
    time_t startT = tmToTimeT(start);
    time_t endT = tmToTimeT(end);
    return (currT >= startT && currT <= endT);
}

double Analyzer::calculateSum(const std::tm& start, const std::tm& end,
    std::function<double(const Measurement&)> selector) {
    double sum = 0.0;
    // U?ycie wzorca iterator do przej?cia po drzewie
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        if (isBetween(it->timestamp, start, end)) {
            sum += selector(*it); // Wywo?anie lambdy
        }
    }
    return sum;
}

double Analyzer::calculateAvg(const std::tm& start, const std::tm& end,
    std::function<double(const Measurement&)> selector) {
    double sum = 0.0;
    int count = 0;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        if (isBetween(it->timestamp, start, end)) {
            sum += selector(*it);
            count++;
        }
    }
    return (count > 0) ? (sum / count) : 0.0;
}

std::function<double(const Measurement&)> Analyzer::getSelector(ValueType type) {
    // Zwracanie lambd w zale?no?ci od typu
    switch (type) {
    case ValueType::AUTOCONSUMPTION: return [](const Measurement& m) { return m.autoconsumption; };
    case ValueType::EXPORT: return [](const Measurement& m) { return m.exportEnergy; };
    case ValueType::IMPORT: return [](const Measurement& m) { return m.importEnergy; };
    case ValueType::CONSUMPTION: return [](const Measurement& m) { return m.consumption; };
    case ValueType::PRODUCTION: return [](const Measurement& m) { return m.production; };
    default: return [](const Measurement& m) { return 0.0; };
    }
}