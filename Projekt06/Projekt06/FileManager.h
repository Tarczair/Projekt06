/**
 * @file FileManager.h
 * @brief Definicja klasy narzedziowej do obslugi plikow.
 *
 * Plik naglowkowy zawierajacy deklaracje klasy FileManager, ktora
 * dostarcza statyczne metody do importowania danych z plikow CSV
 * oraz zarzadzania trwaloscia danych poprzez serializacje do formatu binarnego.
 */

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "EnergyTree.h"
#include <string>

 /**
  * @class FileManager
  * @brief Klasa statyczna obslugujaca operacje wejscia/wyjscia (I/O).
  *
  * Klasa ta nie przechowuje stanu i sluzy wylacznie jako zbior metod pomocniczych
  * do komunikacji z systemem plikow. Odpowiada za parsowanie formatow tekstowych
  * oraz szybki zapis/odczyt binarny struktury EnergyTree.
  */
class FileManager {
public:
    /**
     * @brief Wczytuje dane z pliku CSV i dodaje je do drzewa.
     *
     * Metoda parsuje plik tekstowy w formacie CSV (rozdzielany srednikami),
     * konwertuje dane tekstowe na typy liczbowe oraz tworzy obiekty Measurement.
     * Podczas operacji tworzone sa logi (zapisywane w osobnych plikach txt),
     * ktore raportuja sukcesy oraz bledy parsowania dla kazdej linii.
     *
     * @param tree Referencja do obiektu drzewa, do ktorego zostana dodane dane.
     * @param filename Sciezka do pliku zrodlowego CSV.
     */
    static void loadCSV(EnergyTree& tree, const std::string& filename);

    /**
     * @brief Zapisuje (serializuje) zawartosc drzewa do pliku binarnego.
     *
     * Zrzuca cala strukture danych do pliku w formacie binarnym.
     * Jest to metoda znacznie szybsza niz zapis tekstowy i pozwala na
     * zachowanie precyzji danych zmiennoprzecinkowych.
     *
     * @param tree Referencja do drzewa, ktorego stan ma zostac zapisany.
     * @param filename Sciezka do pliku docelowego.
     */
    static void saveBinary(EnergyTree& tree, const std::string& filename);

    /**
     * @brief Wczytuje (deserializuje) dane z pliku binarnego.
     *
     * Odtwarza stan drzewa na podstawie wczesniej zapisanego pliku binarnego.
     * Przed wczytaniem obecna zawartosc drzewa jest czyszczona.
     *
     * @param tree Referencja do drzewa, ktore zostanie wypelnione danymi.
     * @param filename Sciezka do pliku z danymi binarnymi.
     */
    static void loadBinary(EnergyTree& tree, const std::string& filename);

    /**
     * @brief Generuje znacznik czasowy dla nazw plikow logow.
     *
     * Pomocnicza metoda tworzaca unikalny ciag znakow na podstawie
     * aktualnego czasu systemowego. Format: "RRRRMMDD_GGMMSS".
     *
     * @return std::string Znacznik czasu jako napis.
     */
    static std::string getTimestampStr();
};

#endif