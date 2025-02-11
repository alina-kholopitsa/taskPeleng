#pragma once
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

#ifdef _WIN32
typedef int ssize_t;
#include <windows.h>
using PipeType = HANDLE; //Windows
#else
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
using PipeType = int; //Unix
#endif

struct Event {
    char eventTime[26];
    int eventId = 0;
    int param1 = 0;
    int param2 = 0;
    int param3 = 0;
};

//абстрактный класс Logger
class Logger {
protected:
    std::ofstream logFile;

    Logger(const char* fileName);
    std::mutex writeMutex;

public:
    ~Logger();
    //фабричный метод с учетом уровня логирования и имени файла
    static Logger* GetLogger(int level, const char* fileName);
    virtual void Write(const Event& event) = 0; //виртуальная функция
};

class LoggerLevel0 : public Logger {
public:
    LoggerLevel0(const char* fileName) : Logger(fileName) {}

    void Write(const Event& event) override;
};

class LoggerLevel1 : public Logger {
public:
    LoggerLevel1(const char* fileName) : Logger(fileName) {}

    void Write(const Event& event) override;
};

class LoggerLevel2 : public Logger {
public:
    LoggerLevel2(const char* fileName) : Logger(fileName) {}

    void Write(const Event& event) override;
};

class StartLogging {
public:
    StartLogging(/*HANDLE readPipe*/PipeType readPipe, const char* fileName);
    ~StartLogging();

    void setLogLevel(int newLogLevel);
    void startLogging();
    const char* outFileName;
    
private:
    std::atomic<bool> running{ true };
    PipeType pipeReadFd;
    //HANDLE pipeReadFdWin;           //read file descriptors
    Logger* _logger = nullptr;      //указатель на логгер
    std::mutex mtx;
};
