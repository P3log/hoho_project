#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <mutex>
#include <iomanip>
#include "../include/logger.hpp"

namespace fs = std::filesystem;

Logger::Logger(const string& path) : enabled(true), log_dest(path) {
    fs::create_directories(fs::path(path).parent_path()); // create directory if it does not exist
}

void Logger::enable() { enabled = true; }


void Logger::disable() { enabled = false; }


void Logger::log(const string& message) {
    if (!enabled) return;

    lock_guard<mutex> lock(mtx);

    ofstream file(log_dest, ios::app);
    if (!file) return;

    file << timestamp() << " " << message << endl;
}


string Logger::timestamp() {
    time_t now = time(nullptr);
    tm t;
    localtime_r(&now, &t);

    stringstream ss;
    ss << put_time(&t, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
