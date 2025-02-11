// taskPeleng.cpp: определяет точку входа для приложения.
//
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "CommandRouter.h"
#include "EventsGenerator.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include "Logger.h"



using namespace std;

int main()
{
	setlocale(LC_ALL, "Russian");
	std::mutex sharedMutex;

	//создаем анонимный пайп
#ifdef _WIN32
	HANDLE readPipe;
	HANDLE writePipe;
	if (!CreatePipe(&readPipe, &writePipe, NULL, 0)) {
		readPipe = INVALID_HANDLE_VALUE;
		writePipe = INVALID_HANDLE_VALUE;
		std::cerr << "CreatePipe failed: " << GetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}
    EventsGenerator eventGenerator(writePipe);
#else
    int pipeFd[2];              //pipeFd[0] — чтение, pipeFd[1] — запись
	if (pipe(pipeFd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}
    EventsGenerator eventGenerator(pipeFd[1]); //передаем запись
#endif


//	EventsGenerator eventGenerator(writePipe);
    std::thread eventGenerationThread([&eventGenerator]() { eventGenerator.generation(); });
    //std::thread eventGenerationThread(&EventsGenerator::generation, &eventGenerator);

	//имя файла с текущей датой и временем
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::ostringstream logFileStream;
	logFileStream << "log" << std::put_time(std::localtime(&now_c), "%d%m%Y_%H%M%S") << ".txt";
	std::string logFile = logFileStream.str();
	
	const char* nameLogFile = logFile.c_str();

#ifdef _WIN32
	StartLogging logging(readPipe, nameLogFile);
#else
    StartLogging logging(pipeFd[0], nameLogFile);
#endif

	logging.setLogLevel(0);
	std::thread loggingThread(&StartLogging::startLogging, &logging);


	CommandRouter commandRouter(&eventGenerator, &logging);

	while (true) {

		std::cout << "\nВведите комманду: ";

		std::string input;
		std::getline(std::cin, input);		//считываем всю строку

		commandRouter.userInputHandler(input);

		if (commandRouter.setExitFlag()) {
			eventGenerator.setFlagExit(true);
			std::cout << "Завершение работы программы." << std::endl;
			break;
		}

	}

	eventGenerator.setFlagEventGenerator(false);

	if (eventGenerationThread.joinable()) {
		eventGenerationThread.join();
	}
	if (loggingThread.joinable()) {
		loggingThread.join();
	}
	
	return 0;
}
