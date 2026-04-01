#include <iostream>
#include <fstream>
#include <mutex>
#include <string>

using namespace std;

class Logger {
private:
    bool enabled;
    mutex mtx;
    string log_dest;

public:
    Logger(const string& path);
    void enable();

    void disable();

    void log(const string& message);

    static string timestamp();
};
