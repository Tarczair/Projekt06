#include "EnergyTree.h"

void EnergyTree::addMeasurement(std::unique_ptr<Measurement> m) {
    int year = m->timestamp.tm_year + 1900;
    int month = m->timestamp.tm_mon + 1;
    int day = m->timestamp.tm_mday;
    int hour = m->timestamp.tm_hour;

    // Obliczanie æwiartki: 0 (0-5), 1 (6-11), 2 (12-17), 3 (18-23)
    int quarterIdx = hour / 6;

    // Tworzenie struktury jeœli nie istnieje (lazy initialization)
    if (root.find(year) == root.end())
        root[year] = std::make_unique<YearNode>();

    auto& monthMap = root[year]->months;
    if (monthMap.find(month) == monthMap.end())
        monthMap[month] = std::make_unique<MonthNode>();

    auto& dayMap = monthMap[month]->days;
    if (dayMap.find(day) == dayMap.end())
        dayMap[day] = std::make_unique<DayNode>();

    auto& quartMap = dayMap[day]->quarters;
    if (quartMap.find(quarterIdx) == quartMap.end())
        quartMap[quarterIdx] = std::make_unique<QuarterNode>();

    quartMap[quarterIdx]->add(std::move(m));
}

// --- IMPLEMENTACJA ITERATORA ---

EnergyTree::Iterator::Iterator(std::map<int, std::unique_ptr<YearNode>>& root, bool end)
    : isEnd(end) {

    if (isEnd || root.empty()) {
        isEnd = true;
        return;
    }

    yearIt = root.begin(); yearEnd = root.end();

    // Ustawianie iteratorów w dó³ hierarchii na pierwszy element
    monthIt = yearIt->second->months.begin(); monthEnd = yearIt->second->months.end();
    dayIt = monthIt->second->days.begin(); dayEnd = monthIt->second->days.end();
    quartIt = dayIt->second->quarters.begin(); quartEnd = dayIt->second->quarters.end();
    vecIt = quartIt->second->measurements.begin(); vecEnd = quartIt->second->measurements.end();
}

const Measurement& EnergyTree::Iterator::operator*() const {
    return *(*vecIt);
}

const Measurement* EnergyTree::Iterator::operator->() const {
    return vecIt->get();
}

EnergyTree::Iterator& EnergyTree::Iterator::operator++() {
    if (isEnd) return *this;

    ++vecIt;
    if (vecIt != vecEnd) return *this;

    // Koniec wektora w æwiartce, idŸ do nastêpnej æwiartki
    ++quartIt;
    while (quartIt == quartEnd) {
        ++dayIt;
        while (dayIt == dayEnd) {
            ++monthIt;
            while (monthIt == monthEnd) {
                ++yearIt;
                if (yearIt == yearEnd) {
                    isEnd = true;
                    return *this;
                }
                monthIt = yearIt->second->months.begin();
                monthEnd = yearIt->second->months.end();
            }
            dayIt = monthIt->second->days.begin();
            dayEnd = monthIt->second->days.end();
        }
        quartIt = dayIt->second->quarters.begin();
        quartEnd = dayIt->second->quarters.end();
    }

    // Znaleziono now¹ æwiartkê, ustawiamy wektor
    vecIt = quartIt->second->measurements.begin();
    vecEnd = quartIt->second->measurements.end();

    return *this;
}