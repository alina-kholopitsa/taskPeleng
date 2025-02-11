#include "Logger.h"

void LoggerLevel0::Write(const Event& event)
{
    std::lock_guard<std::mutex> lock(writeMutex);
    std::string timeEvent = event.eventTime;
    logFile << "Time: " << timeEvent.substr(11,22)
        << " Event ID: " << event.eventId
        << std::endl;
}

void LoggerLevel1::Write(const Event& event)
{
    std::lock_guard<std::mutex> lock(writeMutex);
    logFile << "Time: " << event.eventTime
        << " Event ID: " << event.eventId
        << " Params: " << event.param1
        << std::endl;
}

void LoggerLevel2::Write(const Event& event)
{
    std::lock_guard<std::mutex> lock(writeMutex);
    logFile << "Time: " << event.eventTime
        << " Event ID: " << event.eventId
        << " Params: " << event.param1 << ", " << event.param2 << ", " << event.param3
        << std::endl;
}

StartLogging::StartLogging(/*HANDLE readPipe*/PipeType readPipe, const char* fileName) : outFileName(fileName), pipeReadFd(readPipe)
{
}

StartLogging::~StartLogging()
{
}

void StartLogging::startLogging()
{
    std::thread([this]() {
        Event event;
        while (running) {
            ssize_t bytesRead = 0;

#if defined(_WIN32)
            DWORD bytesAvailable;
            if (PeekNamedPipe((HANDLE)pipeReadFd, NULL, 0, NULL, &bytesAvailable, NULL)) {
                if (bytesAvailable > 0) {
                    DWORD bytesReadWin;
                    if (ReadFile((HANDLE)pipeReadFd, &event, sizeof(event), &bytesReadWin, NULL)) {
                        bytesRead = bytesReadWin;
                        if (_logger) {
                            _logger->Write(event); //записываем событие
                        }
                    }
                    else {
                        std::cerr << "Ошибка чтения из пайпа" << std::endl;
                        running = false;
                    }
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
            else {
                std::cerr << "PeekNamedPipe error" << std::endl;
                running = false;
            }
#else
            bytesRead = read(pipeReadFd, &event, sizeof(event));
            if (bytesRead < 0) {
                std::cerr << "Ошибка чтения из пайпа" << std::endl;
                running = false;
            }
            else if (bytesRead == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            else {
                if (_logger) {
                    _logger->Write(event); //записываем событие
                }
            }
#endif
        }
        }).detach();
}

void StartLogging::setLogLevel(int newLogLevel)
{
    std::lock_guard<std::mutex> lock(mtx);
    Logger* newLogger = Logger::GetLogger(newLogLevel, outFileName);
    delete _logger;         //освобождаем старый логгер
    _logger = newLogger;    //устанавливаем новый логгер
}

Logger::Logger(const char* fileName)
{
    logFile.open(fileName, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Ошибка открытия лог-файла: " << fileName << std::endl;
    }
}

Logger::~Logger()
{
    if (logFile.is_open()) {
        logFile.close();
    }
}

Logger* Logger::GetLogger(int level, const char* fileName)
{
    switch (level) {
    case 0: return new LoggerLevel0(fileName);
    case 1: return new LoggerLevel1(fileName);
    case 2: return new LoggerLevel2(fileName);
    default: return nullptr;
    }
}
