/**
 * @file Analyzer.cpp
 * @brief Implementacja metod klasy Analyzer.
 *
 * Plik ten zawiera definicje metod odpowiedzialnych za logike analityczna
 * programu. Znajduja sie tu implementacje funkcji obliczajacych sumy,
 * srednie, wykonujacych wyszukiwanie oraz porownywanie danych zgromadzonych
 * w strukturze EnergyTree.
 */

#include "Analyzer.h"
#include <iostream>

 /**
  * @brief Fabryka selektorow danych.
  *
  * Metoda statyczna, ktora na podstawie przekazanego typu wyliczeniowego DataType
  * zwraca odpowiednia funkcje lambda. Funkcja ta pozwala na wyciagniecie
  * konkretnej wartosci liczbowej (np. import, eksport, produkcja) z obiektu
  * klasy Measurement.
  *
  * @param type Typ danych, dla ktorego chcemy uzyskac dostep (np. IMPORT, PROD).
  * @return std::function<double(const Measurement&)> Funkcja zwracajaca wartosc pola jako double.
  * W przypadku nieznanego typu zwraca 0.0.
  */
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

/**
 * @brief Oblicza sume wartosci wybranego typu w zadanym zakresie dat.
 *
 * Metoda konwertuje struktury std::tm na czas liniowy (time_t), a nastepnie
 * iteruje po elementach drzewa. Sumowane sa wartosci tylko z tych wezlow,
 * ktorych data miesci sie w przedziale [s, e].
 *
 * @param s Data poczatkowa (wlacznie).
 * @param e Data koncowa (wlacznie).
 * @param type Typ danych do zsumowania (np. DataType::IMPORT).
 * @return double Sumaryczna wartosc energii w watach (W).
 */
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

/**
 * @brief Oblicza srednia arytmetyczna wartosci w zadanym zakresie dat.
 *
 * Dziala analogicznie do metody getSum, ale dodatkowo zlicza ilosc
 * rekordow spelniajacych kryterium czasowe.
 *
 * @param s Data poczatkowa.
 * @param e Data koncowa.
 * @param type Typ danych do analizy.
 * @return double Srednia wartosc. Jesli brak danych w przedziale, zwraca 0.
 */
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

/**
 * @brief Wyszukuje i wypisuje rekordy o zadanej wartosci z uwzglednieniem tolerancji.
 *
 * Przeszukuje drzewo w zadanym przedziale czasu. Jesli wartosc wskazanego typu
 * miesci sie w przedziale [val - tol, val + tol], rekord jest wypisywany
 * na standardowe wyjscie (std::cout).
 *
 * @param type Typ danych do sprawdzenia.
 * @param val Szukana wartosc wzorcowa.
 * @param tol Tolerancja (+/-) od wartosci wzorcowej.
 * @param s Data poczatkowa zakresu przeszukiwania.
 * @param e Data koncowa zakresu przeszukiwania.
 */
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

/**
 * @brief Porownuje sumy energii z dwoch roznych okresow.
 *
 * Oblicza sumy dla dwoch niezaleznych przedzialow czasu i wypisuje je
 * oraz ich roznice na standardowe wyjscie. Przydatne do porownywania
 * zuzycia lub produkcji rok do roku lub miesiac do miesiaca.
 *
 * @param s1 Poczatek pierwszego okresu.
 * @param e1 Koniec pierwszego okresu.
 * @param s2 Poczatek drugiego okresu.
 * @param e2 Koniec drugiego okresu.
 * @param type Typ danych, ktory jest porownywany.
 */
void Analyzer::compare(std::tm s1, std::tm e1, std::tm s2, std::tm e2, DataType type) {
    double sum1 = getSum(s1, e1, type);
    double sum2 = getSum(s2, e2, type);
    std::cout << "Przedzial 1: " << sum1 << " W, Przedzial 2: " << sum2 << " W. Roznica: " << sum1 - sum2 << " W\n";
}