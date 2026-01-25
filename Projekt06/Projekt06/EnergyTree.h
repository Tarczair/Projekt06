/**
 * @file EnergyTree.h
 * @brief Definicja glownego kontenera danych energetycznych.
 *
 * Plik zawiera definicje klasy EnergyTree, ktora stanowi punkt wejscia do
 * hierarchicznej struktury danych (drzewa). Klasa ta zarzadza korzeniem drzewa
 * (mapa lat) oraz udostepnia iterator pozwalajacy na liniowe przejscie przez
 * wszystkie zgromadzone pomiary, niezaleznie od ich zagniezdzenia.
 */

#ifndef ENERGYTREE_H
#define ENERGYTREE_H

#include "TreeStructure.h"

 /**
  * @class EnergyTree
  * @brief Glowna klasa przechowujaca cala historie pomiarow.
  *
  * Klasa ta pelni role fasady dla zlozonej struktury danych. Zamiast operowac
  * bezposrednio na wezlach (YearNode, MonthNode itd.), uzytkownik korzysta
  * z metod tej klasy do dodawania pomiarow oraz iterowania po nich.
  * Wewnetrznie dane sa zorganizowane w mapie, gdzie kluczem jest rok.
  */
class EnergyTree {
private:
    /**
     * @brief Korzen struktury - mapa lat.
     *
     * Klucz: numer roku (int).
     * Wartosc: unikalny wskaznik do wezla roku (YearNode).
     * Dzieki uzyciu std::map dane sa automatycznie posortowane chronologicznie.
     */
    std::map<int, std::unique_ptr<YearNode>> root;

public:
    /**
     * @brief Dodaje nowy pomiar do drzewa.
     *
     * Metoda ta deleguje wstawianie danych do odpowiednich wezlow podrzednych.
     * Jesli wezel dla danego roku nie istnieje, jest tworzony. Nastepnie
     * wywolywana jest metoda add wezla roku, ktora przekazuje dane nizej
     * (do miesiaca, dnia, itd.).
     *
     * @param m Unikalny wskaznik do obiektu Measurement (pomiaru).
     * @return bool Zwraca true, jesli pomiar zostal dodany, false w przypadku bledu.
     */
    bool addMeasurement(std::unique_ptr<Measurement> m);

    /**
     * @brief Czysci cala zawartosc drzewa.
     *
     * Usuwa wszystkie wezly i zwalnia pamiec. Po wywolaniu tej metody
     * kontener jest pusty.
     */
    void clear() { root.clear(); }

    /**
     * @class Iterator
     * @brief Iterator pozwalajacy na liniowe przejscie po wszystkich pomiarach.
     *
     * Iterator ten ukrywa skomplikowana, zagniezdzona strukture drzewa
     * (Rok -> Miesiac -> Dzien -> Kwadrans -> Wektor Pomiarow).
     * Umozliwia uzycie petli for-each lub standardowych algorytmow STL
     * na obiekcie EnergyTree tak, jakby byl to plaski kontener.
     */
    class Iterator {
        // Iteratory dla poszczegolnych poziomow zagniezdzenia
        std::map<int, std::unique_ptr<YearNode>>::iterator yIt, yEnd;
        std::map<int, std::unique_ptr<MonthNode>>::iterator mIt, mEnd;
        std::map<int, std::unique_ptr<DayNode>>::iterator dIt, dEnd;
        std::map<int, std::unique_ptr<QuarterNode>>::iterator qIt, qEnd;
        std::vector<std::unique_ptr<Measurement>>::iterator vIt, vEnd;

        // Flaga oznaczajaca koniec iteracji
        bool isEnd;

    public:
        /**
         * @brief Konstruktor iteratora.
         *
         * Inicjalizuje iterator na poczatek struktury (jesli end == false)
         * lub tworzy iterator konca (jesli end == true). Ustawia wewnetrzne
         * iteratory na pierwsze dostepne elementy w zagniezdzonej strukturze.
         *
         * @param r Referencja do korzenia drzewa (mapy lat).
         * @param end Flaga okreslajaca, czy tworzymy iterator begin (false) czy end (true).
         */
        Iterator(std::map<int, std::unique_ptr<YearNode>>& r, bool end);

        /**
         * @brief Operator dereferencji.
         * @return const Measurement& Referencja do biezacego pomiaru.
         */
        const Measurement& operator*() const { return *(*vIt); }

        /**
         * @brief Operator dostepu do skladowych (strzalka).
         * @return const Measurement* Wskaznik do biezacego pomiaru.
         */
        const Measurement* operator->() const { return vIt->get(); }

        /**
         * @brief Operator pre-inkrementacji (++it).
         *
         * Przesuwa iterator na nastepny element. Jesli wektor w biezacym wezle
         * sie skonczyl, przechodzi do nastepnego kwadransa, dnia, miesiaca lub roku,
         * az znajdzie kolejne dane lub osiagnie koniec struktury.
         *
         * @return Iterator& Referencja do zaktualizowanego iteratora.
         */
        Iterator& operator++();

        /**
         * @brief Operator porownania (nierownosci).
         * @param other Inny iterator do porownania.
         * @return bool True, jesli iteratory wskazuja na rozne elementy.
         */
        bool operator!=(const Iterator& other) const { return isEnd != other.isEnd; }
    };

    /**
     * @brief Zwraca iterator wskazujacy na pierwszy element drzewa.
     * @return Iterator Iterator begin.
     */
    Iterator begin() { return Iterator(root, false); }

    /**
     * @brief Zwraca iterator wskazujacy na koniec zakresu (za ostatnim elementem).
     * @return Iterator Iterator end.
     */
    Iterator end() { return Iterator(root, true); }
};

#endif