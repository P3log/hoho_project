#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <chrono>
#include <thread>
#include <iomanip>
#include "../include/dht22.h"

#define TRUE 1

using namespace std;

// ---------------- UTILS ----------------
void usage(const string& program) {
    cout << "Usage:\n\t" << program << " DELAY_IN_MINUTES\n";
}


string float_to_string(float value) {
    ostringstream oss;
    oss << fixed << setprecision(2) << value;
    return oss.str();
}


// ---------------- MAIN ----------------
int main(int argc, char* argv[]) {

    if (argc != 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int delay = stoi(argv[1]); // plus sûr que atoi

    float humidity = 0.0f;
    float temperature = 0.0f;

    const string csvProgram = "./writers";

    while (TRUE) {

        int funcStatus = getMeasures(&humidity, &temperature);

        if (funcStatus == 1) {
            perror("Error while trying to get a measure");
            this_thread::sleep_for(chrono::seconds(2));
            continue;
        }

        string str_humidity = float_to_string(humidity);
        string str_temperature = float_to_string(temperature);

        pid_t pid = fork();
        if (pid == -1) {
            perror("Fork failure");
            return EXIT_FAILURE;
        }
        else if (pid == 0) {
            // Child process
            execlp(csvProgram.c_str(),
                   "writers",
                   str_humidity.c_str(),
                   str_temperature.c_str(),
                   NULL);

            perror("Exec failure");
            _exit(EXIT_FAILURE); // important en cas d'échec
        }

        // Parent process
        int status;
        wait(&status);

        this_thread::sleep_for(chrono::minutes(delay));
    }

    return EXIT_SUCCESS;
}