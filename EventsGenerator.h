#pragma once
#include <iostream>
#include <iomanip>
#include <atomic>
#include <thread>
#include <sstream>   //для std::ostringstream
#include <mutex>
#include <variant>
#include <cstring>



#ifdef _WIN32
#include <windows.h>
using PipeType = HANDLE; //Windows
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
using PipeType = int; //Unix
#endif

class EventsGenerator {
public:

    EventsGenerator(PipeType writePipe);
    //EventsGenerator(HANDLE writePipe);

    ~EventsGenerator(void);

    void generation();
    void setFlagEventGenerator(bool flag);
    void setFlagExit(bool flag);
    double getIntervalAddition();
    void setIntervalAddition(double new_interval);
    int getCountEvent();

private:
    struct Event {
        char eventTime[26];
        int eventId = 0;
        int param1 = 0;
        int param2 = 0;
        int param3 = 0;
    };

    PipeType pipeWriteFd; // ?????????? PipeType ??? ???????? ????????????
//    HANDLE pipeWriteFdWin;

    std::atomic<bool> flagEventGenerator{ true };       //флаг генерации событий, true-генерация, false-остановить генерацию
    std::atomic<bool> flagExit{ false };
    
    int countEvents = 0;
    double intervalAddition = 3.0;
    std::mutex mtx;
};
