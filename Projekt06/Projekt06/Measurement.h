/**
 * @file Measurement.h
 * @brief Definicja podstawowej struktury danych pomiarowych.
 *
 * Plik naglowkowy zawierajacy strukture Measurement, ktora jest fundamentem
 * calego modelu danych. Reprezentuje ona pojedynczy odczyt z licznika
 * w konkretnym punkcie czasu.
 */

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <iostream>
#include <fstream>
#include <ctime>

 /**
  * @struct Measurement
  * @brief Struktura reprezentujaca pojedynczy rekord pomiarowy.
  *
  * Przechowuje znaczniki czasu oraz zestaw pieciu wartosci energetycznych.
  * Zawiera rowniez metody umozliwiajace porownywanie instancji (na potrzeby
  * sortowania i unikalnosci) oraz serializacje do formatu binarnego.
  */
struct Measurement {
    std::tm timestamp;      /**< Data i czas wykonania pomiaru. */
    double autoconsumption; /**< Autokonsumpcja energii [W]. */
    double exportEnergy;    /**< Energia wyeksportowana do sieci [W]. */
    double importEnergy;    /**< Energia pobrana z sieci [W]. */
    double consumption;     /**< Calkowite zuzycie domu [W]. */
    double production;      /**< Calkowita produkcja z instalacji PV [W]. */

    /**
     * @brief Konstruktor domyslny.
     *
     * Inicjalizuje wszystkie wartosci liczbowe zerami, a strukture czasu
     * ustawia na pusta.
     */
    Measurement() : autoconsumption(0), exportEnergy(0), importEnergy(0), consumption(0), production(0) {
        timestamp = {};
    }

    /**
     * @brief Operator mniejszosci.
     *
     * Porownuje dwa pomiary na podstawie ich czasu. Umozliwia sortowanie
     * chronologiczne w kontenerach STL.
     *
     * @param other Inny obiekt Measurement do porownania.
     * @return bool True, jesli biezacy obiekt jest starszy (wczesniejszy) niz other.
     */
    bool operator<(const Measurement& other) const {
        return tmToTime() < other.tmToTime();
    }

    /**
     * @brief Operator rownosci.
     *
     * Sprawdza, czy dwa pomiary dotycza tego samego momentu w czasie.
     * Wykorzystywany do wykrywania duplikatow.
     *
     * @param other Inny obiekt Measurement do porownania.
     * @return bool True, jesli znaczniki czasu sa identyczne.
     */
    bool operator==(const Measurement& other) const {
        return tmToTime() == other.tmToTime();
    }

    /**
     * @brief Konwertuje strukture std::tm na czas liniowy (time_t).
     *
     * Metoda pomocnicza ulatwiajaca porownywanie dat poprzez sprowadzenie
     * ich do pojedynczej liczby calkowitej (liczba sekund od epoki).
     * Uzywa kopii lokalnej, aby funkcja mktime nie modyfikowala oryginalu.
     *
     * @return time_t Czas w formacie liniowym.
     */
    time_t tmToTime() const {
        std::tm temp = timestamp;
        return std::mktime(&temp);
    }

    /**
     * @brief Serializuje obiekt do strumienia binarnego.
     *
     * Zapisuje "surowa" zawartosc pamieci struktury (wszystkie pola)
     * bezposrednio do pliku. Pozwala to na szybki zapis i odczyt.
     *
     * @param ofs Strumien wyjsciowy pliku (otwarty w trybie binarnym).
     */
    void serialize(std::ofstream& ofs) const {
        ofs.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
        ofs.write(reinterpret_cast<const char*>(&autoconsumption), sizeof(autoconsumption));
        ofs.write(reinterpret_cast<const char*>(&exportEnergy), sizeof(exportEnergy));
        ofs.write(reinterpret_cast<const char*>(&importEnergy), sizeof(importEnergy));
        ofs.write(reinterpret_cast<const char*>(&consumption), sizeof(consumption));
        ofs.write(reinterpret_cast<const char*>(&production), sizeof(production));
    }

    /**
     * @brief Deserializuje obiekt ze strumienia binarnego.
     *
     * Wczytuje dane bezposrednio do pol struktury, odtwarzajac stan
     * zapisany metoda serialize.
     *
     * @param ifs Strumien wejsciowy pliku (otwarty w trybie binarnym).
     */
    void deserialize(std::ifstream& ifs) {
        ifs.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
        ifs.read(reinterpret_cast<char*>(&autoconsumption), sizeof(autoconsumption));
        ifs.read(reinterpret_cast<char*>(&exportEnergy), sizeof(exportEnergy));
        ifs.read(reinterpret_cast<char*>(&importEnergy), sizeof(importEnergy));
        ifs.read(reinterpret_cast<char*>(&consumption), sizeof(consumption));
        ifs.read(reinterpret_cast<char*>(&production), sizeof(production));
    }
};

#endif