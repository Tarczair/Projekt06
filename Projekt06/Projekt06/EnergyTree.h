#ifndef ENERGYTREE_H
#define ENERGYTREE_H

#include "TreeStructure.h"
#include <iostream>

class EnergyTree {
private:
    std::map<int, std::unique_ptr<YearNode>> root; // Mapa lat

public:
    void addMeasurement(std::unique_ptr<Measurement> m);
    void clear() { root.clear(); }

    // --- ITERATOR ---
    class Iterator {
    private:
        // Iteratory do poruszania siê po mapach i wektorze
        std::map<int, std::unique_ptr<YearNode>>::iterator yearIt, yearEnd;
        std::map<int, std::unique_ptr<MonthNode>>::iterator monthIt, monthEnd;
        std::map<int, std::unique_ptr<DayNode>>::iterator dayIt, dayEnd;
        std::map<int, std::unique_ptr<QuarterNode>>::iterator quartIt, quartEnd;
        std::vector<std::unique_ptr<Measurement>>::iterator vecIt, vecEnd;

        bool isEnd; // Flaga koñca

    public:
        Iterator(std::map<int, std::unique_ptr<YearNode>>& root, bool end = false);

        // Operator dereferencji zwraca wskaŸnik do Measurement
        const Measurement& operator*() const;
        const Measurement* operator->() const;

        // Operator inkrementacji
        Iterator& operator++();

        bool operator!=(const Iterator& other) const {
            return isEnd != other.isEnd; // Uproszczone porównanie tylko na koniec
        }
    };

    Iterator begin() { return Iterator(root, false); }
    Iterator end() { return Iterator(root, true); }

    // Dostêp do korzenia dla serializacji (przyjaciel lub getter)
    const std::map<int, std::unique_ptr<YearNode>>& getRoot() const { return root; }
};

#endif