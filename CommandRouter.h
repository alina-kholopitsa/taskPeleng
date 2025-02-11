#pragma once
#include <functional>
#include <map>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <regex>        //дл€ работы с регул€рными выражени€ми
#include "EventsGenerator.h"
#include "Logger.h"


class CommandRouter {
public:
	CommandRouter(EventsGenerator* eventGen, StartLogging* log);
	~CommandRouter();

	void userInputHandler(const std::string& command);			//ф-ци€ обработки пользовательского клавиатурного ввода
	bool setExitFlag();

private:
	void createMap();
	void printListCommands();

	void handlerDate(const char* params);
	void handlerTime(const char* params);
	void handlerExit(const char* params);
	void handlerFaster(const char* params);
	void handlerSlower(const char* params);
	void handlerPause(const char* params);
	void handlerContinue(const char* params);
	void handlerZiroLevel(const char* params);
	void handlerFirstLevel(const char* params);
	void handlerSecondLevel(const char* params);
	void handlerStat(const char* params);

	bool isValidDate(const std::string date);
	bool isValidTime(const std::string time);       //функци€ проверки даты на валидность
	bool isLeapYear(int year);                      //проверка високосного года

	bool exitFlag = false;										//флаг выхода (дл€ завершени€ работы программы) true - завержить работу
	using CommandHandler = std::function<void(const char*)>;    //определ€ем тип обработчика
	std::map<std::string, CommandHandler> commandMap;           //контейнер команд и их обработчиков
	EventsGenerator* eventGenerator;
	StartLogging* logger;


};