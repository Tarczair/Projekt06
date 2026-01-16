#ifndef TREESTRUCTURE_H
#define TREESTRUCTURE_H

#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include "Measurement.h"

// Æwiartka przechowuje posortowane pomiary
struct QuarterNode {
    std::vector<std::unique_ptr<Measurement>> measurements;

    void add(std::unique_ptr<Measurement> m) {
        measurements.push_back(std::move(m));
        // Sortowanie po dodaniu (mo¿na zoptymalizowaæ wstawiaj¹c w odpowiednie miejsce)
        std::sort(measurements.begin(), measurements.end(),
            [](const std::unique_ptr<Measurement>& a, const std::unique_ptr<Measurement>& b) {
                return *a < *b;
            });
    }
};

struct DayNode {
    std::map<int, std::unique_ptr<QuarterNode>> quarters; // 0, 1, 2, 3
};

struct MonthNode {
    std::map<int, std::unique_ptr<DayNode>> days;
};

struct YearNode {
    std::map<int, std::unique_ptr<MonthNode>> months;
};

#endif