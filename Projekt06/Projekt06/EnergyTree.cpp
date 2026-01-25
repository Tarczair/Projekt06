// EnergyTree.cpp
#include "EnergyTree.h"

bool EnergyTree::addMeasurement(std::unique_ptr<Measurement> m) {
    int y = m->timestamp.tm_year + 1900;
    int mon = m->timestamp.tm_mon + 1;
    int d = m->timestamp.tm_mday;
    int q = m->timestamp.tm_hour / 6;

    if (!root[y]) root[y] = std::make_unique<YearNode>();
    if (!root[y]->months[mon]) root[y]->months[mon] = std::make_unique<MonthNode>();
    if (!root[y]->months[mon]->days[d]) root[y]->months[mon]->days[d] = std::make_unique<DayNode>();
    if (!root[y]->months[mon]->days[d]->quarters[q]) root[y]->months[mon]->days[d]->quarters[q] = std::make_unique<QuarterNode>();

    return root[y]->months[mon]->days[d]->quarters[q]->add(std::move(m));
}

EnergyTree::Iterator::Iterator(std::map<int, std::unique_ptr<YearNode>>& r, bool end) : isEnd(end) {
    yIt = r.begin(); yEnd = r.end();
    if (isEnd || yIt == yEnd) { isEnd = true; return; }
    mIt = yIt->second->months.begin(); mEnd = yIt->second->months.end();
    dIt = mIt->second->days.begin(); dEnd = mIt->second->days.end();
    qIt = dIt->second->quarters.begin(); qEnd = dIt->second->quarters.end();
    vIt = qIt->second->measurements.begin(); vEnd = qIt->second->measurements.end();
}

EnergyTree::Iterator& EnergyTree::Iterator::operator++() {
    if (++vIt != vEnd) return *this;
    if (++qIt != qEnd) { vIt = qIt->second->measurements.begin(); vEnd = qIt->second->measurements.end(); return *this; }

    auto nextDay = [&]() {
        if (++dIt != dEnd) { qIt = dIt->second->quarters.begin(); qEnd = dIt->second->quarters.end(); return true; }
        if (++mIt != mEnd) { dIt = mIt->second->days.begin(); dEnd = mIt->second->days.end(); return true; }
        if (++yIt != yEnd) { mIt = yIt->second->months.begin(); mEnd = yIt->second->months.end(); return true; }
        return false;
        };

    while (nextDay()) {
        if (!dIt->second->quarters.empty()) {
            qIt = dIt->second->quarters.begin(); qEnd = dIt->second->quarters.end();
            vIt = qIt->second->measurements.begin(); vEnd = qIt->second->measurements.end();
            return *this;
        }
    }
    isEnd = true; return *this;
}