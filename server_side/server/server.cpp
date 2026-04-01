#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <mutex>
#include <signal.h>
#include "../include/logger.hpp"

#define PORT 4531
#define BUFFER_SIZE 4096
#define DB_DIRECTORY "../db"
#define LOG_FILE "../logs/journal.log"

using namespace std;
namespace fs = std::filesystem;

Logger logger(LOG_FILE);
static pid_t tracer_pid = -1;
static std::mutex tracer_mutex;

enum Command {
    CMD_GETLIST = 0,
    CMD_GETFILE,
    CMD_ENABLE,
    CMD_STOP,
    CMD_GETLOGS,
    CMD_INVALID
};

struct CommandEntry {
    string name;
    Command cmd;
};

struct ExecEntry {
    Command cmd;
    const char* path;
    const char* arg0;
    const char* arg1;
};

struct BodyRequest {
    string command;
    string value;
};


// 🔒 WHITELIST
const vector<CommandEntry> commands = {
    {"getlist", CMD_GETLIST},
    {"getfile", CMD_GETFILE},
    {"starttracer", CMD_ENABLE},
    {"getlogs", CMD_GETLOGS},
    {"stoptracer", CMD_STOP}
};


// ---------------- COMMANDS ----------------
string list_files() {
    try {
        string result = "{\"data\" : [";
        for (const auto &entry : fs::directory_iterator(DB_DIRECTORY)) {
            result += "\"" + entry.path().filename().string() + "\",";
        }
        if (result.back() == ',') result.pop_back();
        result += "]}";
        return result;

    } catch (...) {
        logger.log("ERROR: Could not open directory after command to list files");
        return "{\"error\":\"cannot open directory\"}";
    }
}


string escape_json(const string& input) {
    string out;
    for (char c : input) {
        switch (c) {
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            default: out += c;
        }
    }
    return out;
}


string get_file(const string &filename) {
    ostringstream error_log;
    try{
        // 🔒 basic securization against path traversal
        fs::path p = fs::weakly_canonical(fs::path(DB_DIRECTORY) / filename);
    
        if (p.string().find(fs::canonical(DB_DIRECTORY).string()) != 0) {
            logger.log("ERROR: Invalid path to reach the database");
            return "{\"error\":\"invalid path\"}";
        }
    
        string path = string(DB_DIRECTORY) + "/" + filename;
        ifstream file(path);
        
        if (!file) {
            error_log << "ERROR: Could not find the file " << filename + "]";
            logger.log(error_log.str());
            return "{\"error\":\"file not found\"}";
        }
    
        string content((istreambuf_iterator<char>(file)),
                        istreambuf_iterator<char>());
    
        return "{\"data\":\"" + escape_json(content) + "\"}"; // escape " from file content

    } catch (...) {
        error_log << "ERROR: Filesystem error when trying to reach the file " << filename;
        logger.log(error_log.str());
        return "{\"error\":\"filesystem error\"}";
    }
}

string getlogs() {
 try{
        // 🔒 basic securization against path traversal
        fs::path p = fs::weakly_canonical(fs::path(LOG_FILE));
    
        string path = string(LOG_FILE);
        ifstream file(path);
    
        if (!file) {
            logger.log("ERROR: Log file not found");
            return "{\"error\":\"No log file found\"}";
        }
    
        string content((istreambuf_iterator<char>(file)),
                        istreambuf_iterator<char>());
    
        return "{\"data\":\"" + escape_json(content) + "\"}"; // escape " from file content

    } catch (...) {
        logger.log("ERROR: Filesystem error when trying to reach the log file");
        return "{\"error\":\"filesystem error\"}";
    }
}

Command get_command(const string& cmd) {
    for (const CommandEntry& entry : commands) {
        if (entry.name == cmd)
            return entry.cmd;
    }
    return CMD_INVALID;
}


// ---------------- JSON PARSER ----------------
void extract_json_fields(const string &body, BodyRequest& req) {
    size_t i = 0;

    while (i < body.size()) {
        if (body[i] == '"') {
            i++;
            size_t start = i;

            // KEY
            while (i < body.size() && body[i] != '"') i++;
            if (i >= body.size()) break;

            string key = body.substr(start, i - start);
            i++;

            // skip spaces + colon
            while (i < body.size() && (body[i] == ' ' || body[i] == ':')) i++;

            if (i >= body.size() || body[i] != '"') continue;
            i++;

            // VALUE
            start = i;
            while (i < body.size() && body[i] != '"') i++;
            if (i >= body.size()) break;

            string value = body.substr(start, i - start);
            i++;

            if (key == "command") {
                req.command = value;
            } else if (key == "value") {
                req.value = value;
            }
        } else {
            i++;
        }
    }
}


bool is_process_alive(pid_t pid) {
    if (pid <= 0) return false;
    return kill(pid, 0) == 0;
}


bool is_valid_interval(const string& s) {
    if (s.empty()) return false;

    for (char c : s) {
        if (!isdigit(c)) return false;
    }

    try {
        int val = stoi(s);
        return val > 0 && val <= 2880; // ex: max 48h
    } catch (...) {
        return false;
    }
}


string start_tracer(string interval) {
    lock_guard<mutex> lock(tracer_mutex);

    if (tracer_pid > 0 && is_process_alive(tracer_pid)) {
        logger.log("ERROR: Tracer is already running");
        return "{\"error\":\"already running\"}";
    }

    pid_t pid = fork();
    if (pid == -1) {
        logger.log("ERROR: Fork failure");
        return "{\"error\":\"fork failed\"}";
    }
    if (pid == 0) {
        execl("./dht22_tracer", "dht22_tracer", interval.c_str(), NULL);
        logger.log("ERROR: Tracer interrupted");
        _exit(EXIT_FAILURE);
    }

    tracer_pid = pid;
    ostringstream ret;
    ret << "{\"status\":\"Tracer started with an interval of " << interval << " minutes\"}";
    
    return ret.str();
}


string stop_tracer() {
    lock_guard<mutex> lock(tracer_mutex);

    if (tracer_pid == -1) {
        return "{\"error\":\"not running\"}";
    }
    if (kill(tracer_pid, SIGTERM) != 0) {
        return "{\"error\":\"kill failed\"}";
    }

    tracer_pid = -1;
    return "{\"status\":\"stopped\"}";
}


// ---------------- CLIENT ----------------
void handle_client(int client_sock) {

    char buffer[BUFFER_SIZE] = {0};
    int bytes = read(client_sock, buffer, BUFFER_SIZE - 1);
    if (bytes <= 0) {
        close(client_sock);
        return;
    }
    buffer[bytes] = '\0';

    string request(buffer);

    if (request.find("OPTIONS") == 0) {
        string response =
            "HTTP/1.1 204 No Content\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Access-Control-Max-Age: 86400\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

        send(client_sock, response.c_str(), response.size(), 0);
        close(client_sock);
        return;
    }

    size_t pos = request.find("\r\n\r\n");
    if (pos == string::npos) {
        close(client_sock);
        return;
    }

    string body = request.substr(pos + 4);
    // cout << "[+] Body is :\n" << body << endl;    // TODO REMOVE

    BodyRequest req; // store command and file name
    extract_json_fields(body, req);
    // cout << "[+] Command : '" << req.command << "'; Value : '" << req.value << "'" << endl;    // TODO REMOVE
    
    ostringstream req_log;
    req_log << "INFO: command=" << req.command << "; value=" << req.value;
    
    Command cmd = get_command(req.command);
    
    string response_body;
    // do not trace calls to logs
    switch (cmd) {
        case CMD_GETLIST:
            logger.log(req_log.str());
            response_body = list_files();
            break;

        case CMD_GETFILE: {
            logger.log(req_log.str());
            if (req.value.empty()) {
                logger.log("ERROR: missing filename in the get file comment");
                response_body = "{\"error\":\"missing filename\"}";
            } else {
                response_body = get_file(req.value);
            }
            break;
        }

        case CMD_ENABLE:
            logger.log(req_log.str());
            if (!is_valid_interval(req.value)) {
                logger.log("ERROR: invalid interval");
                response_body = "{\"error\":\"invalid interval\"}";
            } else {
                response_body = start_tracer(req.value);
            }
            break;

        case CMD_STOP:
            logger.log(req_log.str());
            response_body = stop_tracer();
            break;
        
        case CMD_GETLOGS: // not traced
            response_body = getlogs();
            break;
            
        default: // CMD_INVALID
            logger.log(req_log.str());
            logger.log("ERROR: invalid command");
            response_body = "{\"error\":\"invalid command\"}";
            break;

    }

    string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + to_string(response_body.size()) + "\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "\r\n" +
        response_body;

    send(client_sock, response.c_str(), response.size(), 0);
    close(client_sock);
}


// ---------------- SERVER ----------------
int main() {

    int server_fd;
    sockaddr_in address{};
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);

    cout << "[+] Secure HTTP server on port " << PORT << endl;
    logger.log("INFO: server running");

    while (true) {
        int client = accept(server_fd, (sockaddr*)&address, (socklen_t*)&addrlen);
        thread(handle_client, client).detach();
    }
}