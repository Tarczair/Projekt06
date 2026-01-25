/**
 * @file EnergyTree.cpp
 * @brief Implementacja metod klasy EnergyTree oraz jej iteratora.
 *
 * Plik zawiera logike budowania hierarchicznej struktury danych (drzewa)
 * w momencie dodawania nowych pomiarow oraz implementacje iteratora,
 * ktory umozliwia splaszczone (liniowe) przegladanie zagniezdzonych danych.
 */

#include "EnergyTree.h"

 /**
  * @brief Dodaje nowy pomiar do struktury drzewiastej.
  *
  * Metoda analizuje date pomiaru, aby okreslic sciezke w drzewie:
  * Rok -> Miesiac -> Dzien -> Kwadrans (blok 6-godzinny).
  * Wykorzystuje mechanizm leniwej inicjalizacji (lazy initialization) -
  * jesli wezel dla danego roku, miesiaca, dnia lub kwadransa nie istnieje,
  * jest tworzony dynamicznie za pomoca std::make_unique.
  *
  * @param m Unikalny wskaznik do obiektu Measurement. Przejmuje wlasnosc obiektu.
  * @return bool Zwraca true, jesli pomiar udalo sie dodac (np. nie byl duplikatem).
  */
bool EnergyTree::addMeasurement(std::unique_ptr<Measurement> m) {
    // Ekstrakcja kluczy dla poszczegolnych poziomow drzewa
    int y = m->timestamp.tm_year + 1900;
    int mon = m->timestamp.tm_mon + 1;
    int d = m->timestamp.tm_mday;
    int q = m->timestamp.tm_hour / 6; // Obliczenie indeksu kwadransa (0-3)

    // Tworzenie brakujacych wezlow w sciezce
    if (!root[y]) root[y] = std::make_unique<YearNode>();
    if (!root[y]->months[mon]) root[y]->months[mon] = std::make_unique<MonthNode>();
    if (!root[y]->months[mon]->days[d]) root[y]->months[mon]->days[d] = std::make_unique<DayNode>();
    if (!root[y]->months[mon]->days[d]->quarters[q]) root[y]->months[mon]->days[d]->quarters[q] = std::make_unique<QuarterNode>();

    // Delegacja dodania do liscia drzewa (wezel QuarterNode)
    return root[y]->months[mon]->days[d]->quarters[q]->add(std::move(m));
}

/**
 * @brief Konstruktor iteratora EnergyTree.
 *
 * Inicjalizuje zagniezdzone iteratory (dla lat, miesiecy, dni, kwadransow i wektora pomiarow).
 * Jesli tworzony jest iterator begin, ustawia wskazniki na pierwszy dostepny element.
 * Jesli tworzony jest iterator end lub drzewo jest puste, ustawia flage isEnd na true.
 *
 * @param r Referencja do mapy glownej (korzenia drzewa).
 * @param end Flaga okreslajaca, czy tworzymy iterator konca.
 */
EnergyTree::Iterator::Iterator(std::map<int, std::unique_ptr<YearNode>>& r, bool end) : isEnd(end) {
    yIt = r.begin(); yEnd = r.end();

    // Obsluga pustego drzewa lub zadania iteratora end
    if (isEnd || yIt == yEnd) { isEnd = true; return; }

    // Inicjalizacja iteratorow w dol hierarchii
    mIt = yIt->second->months.begin(); mEnd = yIt->second->months.end();
    dIt = mIt->second->days.begin(); dEnd = mIt->second->days.end();
    qIt = dIt->second->quarters.begin(); qEnd = dIt->second->quarters.end();
    vIt = qIt->second->measurements.begin(); vEnd = qIt->second->measurements.end();
}

/**
 * @brief Operator pre-inkrementacji iteratora (++it).
 *
 * Odpowiada za przejscie do nastepnego pomiaru. Posiada zlozona logike
 * wyjscia z zagniezdzen:
 * 1. Przesuwa iterator wektora pomiarow.
 * 2. Jesli wektor sie skonczyl, przechodzi do nastepnego kwadransa.
 * 3. Jesli kwadransy sie skonczyly, szuka nastepnego dnia, miesiaca lub roku.
 *
 * @return EnergyTree::Iterator& Referencja do zaktualizowanego iteratora.
 */
EnergyTree::Iterator& EnergyTree::Iterator::operator++() {
    // 1. Probuj przesunac wewnatrz biezacego wektora pomiarow
    if (++vIt != vEnd) return *this;

    // 2. Probuj przesunac na nastepny kwadrans w biezacym dniu
    if (++qIt != qEnd) {
        vIt = qIt->second->measurements.begin();
        vEnd = qIt->second->measurements.end();
        return *this;
    }

    // Lambda pomocnicza do szukania nastepnego niepustego wezla wyzszego rzedu
    auto nextDay = [&]() {
        // Sprawdz kolejne dni
        if (++dIt != dEnd) {
            qIt = dIt->second->quarters.begin();
            qEnd = dIt->second->quarters.end();
            return true;
        }
        // Sprawdz kolejne miesiace
        if (++mIt != mEnd) {
            dIt = mIt->second->days.begin();
            dEnd = mIt->second->days.end();
            return true;
        }
        // Sprawdz kolejne lata
        if (++yIt != yEnd) {
            mIt = yIt->second->months.begin();
            mEnd = yIt->second->months.end();
            return true;
        }
        return false; // Koniec danych w drzewie
        };

    // 3. Szukaj danych w kolejnych wezlach (dzien -> miesiac -> rok)
    while (nextDay()) {
        if (!dIt->second->quarters.empty()) {
            qIt = dIt->second->quarters.begin(); qEnd = dIt->second->quarters.end();
            vIt = qIt->second->measurements.begin(); vEnd = qIt->second->measurements.end();
            return *this;
        }
    }

    // Jesli nic nie znaleziono, ustaw flage konca
    isEnd = true;
    return *this;
}