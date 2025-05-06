#pragma once

#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    
    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex);
        if (logFile.is_open()) {
            logFile << message << std::endl;
        }
    }
    
    void setLogFile(const std::string& filename) {
        logFile.open(filename);
    }
    
    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
private:
    Logger() = default;
    std::ofstream logFile;
    std::mutex mutex;
}; 