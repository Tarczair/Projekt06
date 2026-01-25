// Analyzer.cpp
#include "Analyzer.h"
#include <iostream>

std::function<double(const Measurement&)> Analyzer::getSelector(DataType type) {
    switch (type) {
    case DataType::AUTO: return [](const Measurement& m) { return m.autoconsumption; };
    case DataType::EXPORT: return [](const Measurement& m) { return m.exportEnergy; };
    case DataType::IMPORT: return [](const Measurement& m) { return m.importEnergy; };
    case DataType::CONS: return [](const Measurement& m) { return m.consumption; };
    case DataType::PROD: return [](const Measurement& m) { return m.production; };
    default: return [](const Measurement& m) { return 0.0; };
    }
}

double Analyzer::getSum(std::tm s, std::tm e, DataType type) {
    double sum = 0;
    auto sel = getSelector(type);
    time_t start = mktime(&s), end = mktime(&e);
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        time_t cur = it->tmToTime();
        if (cur >= start && cur <= end) sum += sel(*it);
    }
    return sum;
}

double Analyzer::getAvg(std::tm s, std::tm e, DataType type) {
    double sum = 0; int count = 0;
    auto sel = getSelector(type);
    time_t start = mktime(&s), end = mktime(&e);
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        time_t cur = it->tmToTime();
        if (cur >= start && cur <= end) { sum += sel(*it); count++; }
    }
    return count > 0 ? sum / count : 0;
}

void Analyzer::search(DataType type, double val, double tol, std::tm s, std::tm e) {
    auto sel = getSelector(type);
    time_t start = mktime(&s), end = mktime(&e);
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        double v = sel(*it);
        if (it->tmToTime() >= start && it->tmToTime() <= end && v >= val - tol && v <= val + tol) {
            std::cout << "Znaleziono: " << v << " W przy dacie " << it->timestamp.tm_mday << "." << it->timestamp.tm_mon + 1 << "\n";
        }
    }
}

void Analyzer::compare(std::tm s1, std::tm e1, std::tm s2, std::tm e2, DataType type) {
    double sum1 = getSum(s1, e1, type);
    double sum2 = getSum(s2, e2, type);
    std::cout << "Przedzial 1: " << sum1 << " W, Przedzial 2: " << sum2 << " W. Roznica: " << sum1 - sum2 << " W\n";
}