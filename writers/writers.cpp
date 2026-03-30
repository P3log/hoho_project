#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <mutex>
#include "../include/logger.hpp"

#define DB_FILE "../db/measures.csv"
#define LOG_FILE "../logs/journal.log"

using namespace std;
namespace fs = std::filesystem;


class CsvWriter {
private:
    string dbPath;
    Logger& logger;
    mutex mtx;

public:
    CsvWriter(const string& filePath, Logger& log)
        : dbPath(filePath), logger(log) {}

    void writeHeaderIfNeeded() {
        if (!fs::exists(dbPath)) {
            ofstream file(dbPath);
            if (!file) {
                logger.log("ERROR: Cannot create CSV file");
                return;
            }

            file << "DATE;TIME;HUMIDITY (%);TEMPERATURE (°C)\n";
        }
    }

    void write(float humidity, float temperature) {
        lock_guard<mutex> lock(mtx);

        writeHeaderIfNeeded();

        ofstream file(dbPath, ios::app);
        if (!file) {
            logger.log("ERROR: Cannot open CSV file");
            return;
        }

        auto [date, time] = getDateTime();

        file << date << ";"
             << time << ";"
             << fixed << setprecision(2) // display 2 digits after 0
             << humidity << ";"
             << temperature << "\n";
    }

private:
    pair<string, string> getDateTime() {
        time_t now = time(nullptr);
        tm *t = localtime(&now);

        stringstream date, time;

        date << put_time(t, "%Y-%m-%d");
        time << put_time(t, "%H:%M:%S");

        return {date.str(), time.str()};
    }
};


// ---------------- MAIN ----------------
int main(int argc, char* argv[]) {

    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <humidity> <temperature>\n";
        return EXIT_FAILURE;
    }

    float humidity = stof(argv[1]);
    float temperature = stof(argv[2]);

    string log_dest = LOG_FILE;
    Logger logger(log_dest);
    CsvWriter writer(DB_FILE, logger);

    writer.write(humidity, temperature);

    cout << "Hdt: " << humidity << " %, Tmp: " << temperature << " °C" << endl; // TODO REMOVE ?

    return EXIT_SUCCESS;
}