/**
 * @file Analyzer.h
 * @brief Plik naglowkowy definiujacy klase Analyzer i typy danych.
 *
 * Ten plik zawiera definicje klasy Analyzer, ktora odpowiada za logike biznesowa
 * aplikacji, w tym obliczenia statystyczne (suma, srednia), wyszukiwanie
 * oraz porownywanie danych zgromadzonych w strukturze EnergyTree.
 */

#ifndef ANALYZER_H
#define ANALYZER_H

#include "EnergyTree.h"
#include <functional>

 /**
  * @brief Typ wyliczeniowy okreslajacy rodzaj danych energetycznych.
  *
  * Uzywany do wskazywania, na ktorym polu struktury Measurement ma operowac
  * dana funkcja analityczna.
  */
enum class DataType {
    AUTO,   /**< Autokonsumpcja */
    EXPORT, /**< Energia wyeksportowana do sieci */
    IMPORT, /**< Energia pobrana z sieci */
    CONS,   /**< Calkowita konsumpcja */
    PROD    /**< Calkowita produkcja */
};

/**
 * @class Analyzer
 * @brief Klasa odpowiedzialna za analize danych pomiarowych.
 *
 * Klasa ta dostarcza metody do przetwarzania danych przechowywanych w drzewie
 * BST (EnergyTree). Umozliwia wykonywanie operacji takich jak sumowanie,
 * obliczanie sredniej, wyszukiwanie wartosci z tolerancja oraz porownywanie okresow.
 */
class Analyzer {
    /**
     * @brief Referencja do obiektu drzewa danych.
     * * Analizator nie posiada danych na wlasnosc, lecz operuje na istniejacym
     * obiekcie EnergyTree przekazanym w konstruktorze.
     */
    EnergyTree& tree;

public:
    /**
     * @brief Konstruktor klasy Analyzer.
     *
     * @param t Referencja do istniejacego obiektu EnergyTree z danymi.
     */
    Analyzer(EnergyTree& t) : tree(t) {}

    /**
     * @brief Statyczna metoda pomocnicza wybierajaca pole pomiaru.
     *
     * Zwraca funkcje lambda (selektor), ktora dla zadanego obiektu Measurement
     * zwraca wartosc liczbowa odpowiadajaca wskazanemu typowi danych (DataType).
     *
     * @param type Typ danych, ktory chcemy odczytac (np. IMPORT, PROD).
     * @return std::function<double(const Measurement&)> Funkcja zwracajaca wartosc pola.
     */
    static std::function<double(const Measurement&)> getSelector(DataType type);

    /**
     * @brief Oblicza sume wartosci w zadanym przedziale czasu.
     *
     * @param s Data poczatkowa przedzialu (start).
     * @param e Data koncowa przedzialu (end).
     * @param type Typ danych do zsumowania.
     * @return double Suma wartosci wybranego typu w zadanym okresie.
     */
    double getSum(std::tm s, std::tm e, DataType type);

    /**
     * @brief Oblicza srednia wartosc w zadanym przedziale czasu.
     *
     * @param s Data poczatkowa przedzialu.
     * @param e Data koncowa przedzialu.
     * @param type Typ danych do uœrednienia.
     * @return double Srednia wartosc wybranego typu w zadanym okresie.
     */
    double getAvg(std::tm s, std::tm e, DataType type);

    /**
     * @brief Wyszukuje pomiary o zadanej wartosci z okreslona tolerancja.
     *
     * Przeszukuje drzewo w zadanym przedziale czasu i wypisuje na wyjscie
     * te rekordy, ktorych wartosc wskazanego typu miesci sie w zakresie
     * [value - tolerance, value + tolerance].
     *
     * @param type Typ danych do sprawdzenia.
     * @param value Szukana wartosc wzorcowa.
     * @param tolerance Dopuszczalne odchylenie od wartosci wzorcowej.
     * @param s Data poczatkowa przeszukiwania.
     * @param e Data koncowa przeszukiwania.
     */
    void search(DataType type, double value, double tolerance, std::tm s, std::tm e);

    /**
     * @brief Wypisuje wszystkie pomiary z zadanego zakresu.
     *
     * @param s Data poczatkowa.
     * @param e Data koncowa.
     */
    void printRange(std::tm s, std::tm e);

    /**
     * @brief Porownuje dwa okresy czasowe dla wybranego typu danych.
     *
     * Metoda moze byc uzyta do wyswietlenia zestawienia statystyk (np. sumy)
     * dla dwoch roznych przedzialow czasu, aby porownac zuzycie lub produkcje.
     *
     * @param s1 Poczatek pierwszego okresu.
     * @param e1 Koniec pierwszego okresu.
     * @param s2 Poczatek drugiego okresu.
     * @param e2 Koniec drugiego okresu.
     * @param type Typ danych do porownania.
     */
    void compare(std::tm s1, std::tm e1, std::tm s2, std::tm e2, DataType type);
};

#endif