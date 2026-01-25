/**
 * @file TreeStructure.h
 * @brief Definicje struktur wezlow tworzacych hierarchie drzewa.
 *
 * Plik ten definiuje bloki budulcowe dla klasy EnergyTree. Struktura danych
 * oparta jest na zagniezdzeonych mapach: Rok -> Miesiac -> Dzien -> Kwadrans.
 * Ostatni poziom (QuarterNode) przechowuje faktyczne dane w wektorze.
 */

#ifndef TREESTRUCTURE_H
#define TREESTRUCTURE_H

#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include "Measurement.h"

 /**
  * @struct QuarterNode
  * @brief Wezel liscia w strukturze drzewa (najni¿szy poziom podzia³u czasu).
  *
  * Reprezentuje blok czasowy (w logice EnergyTree.cpp jest to cwiartka doby,
  * czyli blok 6-godzinny), ktory przechowuje liste pomiarow.
  * Odpowiada za utrzymanie posortowanej kolejnosci pomiarow oraz zapobieganie duplikatom.
  */
struct QuarterNode {
    /**
     * @brief Wektor przechowywujacy unikalne wskazniki do pomiarow.
     * Uzycie std::unique_ptr zapewnia automatyczne zarzadzanie pamiecia.
     */
    std::vector<std::unique_ptr<Measurement>> measurements;

    /**
     * @brief Dodaje nowy pomiar do wektora.
     *
     * Metoda wykonuje trzy kroki:
     * 1. Sprawdza, czy w wektorze istnieje juz pomiar o identycznym czasie (zapobieganie duplikatom).
     * 2. Jesli nie ma duplikatu, dodaje nowy pomiar na koniec wektora.
     * 3. Sortuje wektor chronologicznie, aby ulatwic pozniejsze przeszukiwanie i iteracje.
     *
     * @param m Unikalny wskaznik do nowego pomiaru (przejmuje wlasnosc).
     * @return bool Zwraca true, jesli dodano pomiar. Zwraca false, jesli wykryto duplikat.
     */
    bool add(std::unique_ptr<Measurement> m) {
        // Sprawdzenie unikalnosci
        for (const auto& existing : measurements) {
            if (*existing == *m) return false; // Duplikat znaleziony
        }

        // Dodanie i sortowanie
        measurements.push_back(std::move(m));
        std::sort(measurements.begin(), measurements.end(),
            [](const std::unique_ptr<Measurement>& a, const std::unique_ptr<Measurement>& b) {
                return *a < *b;
            });
        return true;
    }
};

/**
 * @struct DayNode
 * @brief Wezel reprezentujacy jeden dzien.
 *
 * Przechowuje mape, gdzie kluczem jest numer kwadransa/bloku dnia (int),
 * a wartoscia unikalny wskaznik do wezla QuarterNode.
 */
struct DayNode {
    std::map<int, std::unique_ptr<QuarterNode>> quarters;
};

/**
 * @struct MonthNode
 * @brief Wezel reprezentujacy jeden miesiac.
 *
 * Przechowuje mape, gdzie kluczem jest numer dnia miesiaca (int),
 * a wartoscia unikalny wskaznik do wezla DayNode.
 */
struct MonthNode {
    std::map<int, std::unique_ptr<DayNode>> days;
};

/**
 * @struct YearNode
 * @brief Wezel reprezentujacy jeden rok.
 *
 * Przechowuje mape, gdzie kluczem jest numer miesiaca (1-12),
 * a wartoscia unikalny wskaznik do wezla MonthNode.
 */
struct YearNode {
    std::map<int, std::unique_ptr<MonthNode>> months;
};

#endif