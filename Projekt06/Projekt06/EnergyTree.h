// EnergyTree.h
#ifndef ENERGYTREE_H
#define ENERGYTREE_H

#include "TreeStructure.h"

class EnergyTree {
private:
    std::map<int, std::unique_ptr<YearNode>> root;

public:
    bool addMeasurement(std::unique_ptr<Measurement> m);
    void clear() { root.clear(); }

    class Iterator {
        std::map<int, std::unique_ptr<YearNode>>::iterator yIt, yEnd;
        std::map<int, std::unique_ptr<MonthNode>>::iterator mIt, mEnd;
        std::map<int, std::unique_ptr<DayNode>>::iterator dIt, dEnd;
        std::map<int, std::unique_ptr<QuarterNode>>::iterator qIt, qEnd;
        std::vector<std::unique_ptr<Measurement>>::iterator vIt, vEnd;
        bool isEnd;

    public:
        Iterator(std::map<int, std::unique_ptr<YearNode>>& r, bool end);
        const Measurement& operator*() const { return *(*vIt); }
        const Measurement* operator->() const { return vIt->get(); }
        Iterator& operator++();
        bool operator!=(const Iterator& other) const { return isEnd != other.isEnd; }
    };

    Iterator begin() { return Iterator(root, false); }
    Iterator end() { return Iterator(root, true); }
};
#endif