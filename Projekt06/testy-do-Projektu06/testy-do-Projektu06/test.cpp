#include <gtest/gtest.h>
#include <memory>
#include "./../../Projekt06/EnergyTree.h"
#include "./../../Projekt06/Analyzer.h"

// --- TESTY ENERGY TREE ---

// 1. Sprawdzenie zapobiegania duplikatom
TEST(EnergyTreeTest, DuplicatePrevention) {
    EnergyTree tree;
    auto m1 = std::make_unique<Measurement>();
    m1->timestamp.tm_year = 121; m1->timestamp.tm_mon = 5; m1->timestamp.tm_mday = 10; m1->timestamp.tm_hour = 12;

    auto m2 = std::make_unique<Measurement>();
    m2->timestamp = m1->timestamp;

    EXPECT_TRUE(tree.addMeasurement(std::move(m1)));
    EXPECT_FALSE(tree.addMeasurement(std::move(m2)));
}

// 2. Test czyszczenia drzewa (u¿ywamy != zamiast ==)
TEST(EnergyTreeTest, ClearTree) {
    EnergyTree tree;
    auto m = std::make_unique<Measurement>();
    tree.addMeasurement(std::move(m));
    tree.clear();

    // Skoro nie masz operatora ==, sprawdzamy czy begin != end jest FA£SZEM
    EXPECT_FALSE(tree.begin() != tree.end());
}

// 3. Test przypisania do odpowiedniej æwiartki
TEST(EnergyTreeTest, QuarterAssignment) {
    EnergyTree tree;
    auto m = std::make_unique<Measurement>();
    m->timestamp.tm_hour = 3; // Æwiartka 0
    tree.addMeasurement(std::move(m));

    auto it = tree.begin();
    EXPECT_EQ(it->timestamp.tm_hour / 6, 0);
}

// --- TESTY ANALYZER ---

// 4. Test obliczania sumy produkcji
TEST(AnalyzerTest, SumProduction) {
    EnergyTree tree;
    auto m1 = std::make_unique<Measurement>();
    m1->timestamp.tm_year = 121; m1->timestamp.tm_mon = 1; m1->timestamp.tm_mday = 1;
    m1->production = 100.0;

    auto m2 = std::make_unique<Measurement>();
    m2->timestamp = m1->timestamp;
    m2->timestamp.tm_min = 15;
    m2->production = 250.5;

    std::tm s = m1->timestamp, e = m2->timestamp;
    tree.addMeasurement(std::move(m1));
    tree.addMeasurement(std::move(m2));

    Analyzer an(tree);
    EXPECT_DOUBLE_EQ(an.getSum(s, e, DataType::PROD), 350.5);
}

// 5. Test obliczania œredniej
TEST(AnalyzerTest, AverageImport) {
    EnergyTree tree;
    auto m1 = std::make_unique<Measurement>();
    m1->timestamp.tm_year = 121; m1->timestamp.tm_mon = 1; m1->timestamp.tm_mday = 1;
    m1->importEnergy = 10.0;

    auto m2 = std::make_unique<Measurement>();
    m2->timestamp = m1->timestamp;
    m2->timestamp.tm_min = 15;
    m2->importEnergy = 20.0;

    std::tm s = m1->timestamp, e = m2->timestamp;
    tree.addMeasurement(std::move(m1));
    tree.addMeasurement(std::move(m2));

    Analyzer an(tree);
    EXPECT_DOUBLE_EQ(an.getAvg(s, e, DataType::IMPORT), 15.0);
}

// 6. Test zakresu dat
TEST(AnalyzerTest, DateRangeFiltering) {
    EnergyTree tree;
    auto m = std::make_unique<Measurement>();
    m->timestamp.tm_year = 100; // 2000 rok
    m->production = 1000.0;
    tree.addMeasurement(std::move(m));

    std::tm s = {}, e = {};
    s.tm_year = 121; s.tm_mon = 0; s.tm_mday = 1; // 2021 rok
    e.tm_year = 121; e.tm_mon = 11; e.tm_mday = 31;

    Analyzer an(tree);
    EXPECT_DOUBLE_EQ(an.getSum(s, e, DataType::PROD), 0.0);
}

// 7. Test selektora eksportu
TEST(AnalyzerTest, ExportSelector) {
    Measurement m;
    m.exportEnergy = 55.5;
    auto selector = Analyzer::getSelector(DataType::EXPORT);
    EXPECT_DOUBLE_EQ(selector(m), 55.5);
}

// 8. Test selektora poboru (CONS)
TEST(AnalyzerTest, ConsumptionSelector) {
    Measurement m;
    m.consumption = 100.0;
    auto selector = Analyzer::getSelector(DataType::CONS);
    EXPECT_DOUBLE_EQ(selector(m), 100.0);
}

// 9. Test œredniej dla pustego drzewa
TEST(AnalyzerTest, AverageEmptyTree) {
    EnergyTree tree;
    Analyzer an(tree);
    std::tm s = {}, e = {};
    // W Twoim kodzie mktime na pustym tm mo¿e daæ -1, ale pêtla w getAvg siê nie wykona
    EXPECT_DOUBLE_EQ(an.getAvg(s, e, DataType::PROD), 0.0);
}

// 10. Test poprawnoœci metody tmToTime w Measurement
TEST(MeasurementTest, TimeConversion) {
    Measurement m;
    m.timestamp.tm_year = 121; // 2021
    m.timestamp.tm_mon = 0;   // styczeñ
    m.timestamp.tm_mday = 1;
    m.timestamp.tm_hour = 12;
    m.timestamp.tm_min = 0;
    m.timestamp.tm_sec = 0;
    m.timestamp.tm_isdst = -1;

    time_t expected = mktime(&m.timestamp);
    EXPECT_EQ(m.tmToTime(), expected);
}