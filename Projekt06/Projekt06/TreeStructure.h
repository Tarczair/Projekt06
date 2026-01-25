#ifndef TREESTRUCTURE_H
#define TREESTRUCTURE_H

#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include "Measurement.h"

struct QuarterNode {
    std::vector<std::unique_ptr<Measurement>> measurements;

    bool add(std::unique_ptr<Measurement> m) {
        for (const auto& existing : measurements) {
            if (*existing == *m) return false; // Duplikat
        }
        measurements.push_back(std::move(m));
        std::sort(measurements.begin(), measurements.end(),
            [](const std::unique_ptr<Measurement>& a, const std::unique_ptr<Measurement>& b) {
                return *a < *b;
            });
        return true;
    }
};

struct DayNode { std::map<int, std::unique_ptr<QuarterNode>> quarters; };
struct MonthNode { std::map<int, std::unique_ptr<DayNode>> days; };
struct YearNode { std::map<int, std::unique_ptr<MonthNode>> months; };

#endif