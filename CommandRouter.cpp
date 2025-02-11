#include "CommandRouter.h"


CommandRouter::CommandRouter(EventsGenerator* eventGen, StartLogging* log) : logger(log), eventGenerator(eventGen)
{
    createMap();
    printListCommands();
}

CommandRouter::~CommandRouter()
{
}

void CommandRouter::userInputHandler(const std::string& command)
{
    //разделяем команду и параметры
    size_t spacePos = command.find(' ');
    std::string cmd = command.substr(0, spacePos);
    std::string paramsStr = (spacePos != std::string::npos) ? command.substr(spacePos + 1) : "";

    if (cmd == "level") {
        if (!paramsStr.empty()) {
            cmd += " " + std::string(1, paramsStr[0]);      //добавляем первый символ после пробела в cmd
            paramsStr = paramsStr.substr(1);                //удаляем первый символ и все последующие пробелы
        }
        else {      //если пробела нет   
            paramsStr.clear();
        }
    }

    //удаляем пробелы в строкe parameters
    std::string parameters;
    std::remove_copy_if(paramsStr.begin(), paramsStr.end(), std::back_inserter(parameters),
        [](unsigned char ch) { return std::isspace(ch); });
    const char* params = parameters.empty() ? nullptr : parameters.c_str();

    //ищем команду в контейнере
    auto it = commandMap.find(cmd);
    if (it != commandMap.end()) {
        it->second(params);         //вызываем обработчик команды
    }
    else {
        std::cerr << "Ошибка: Неизвестная команда '" << cmd << "'." << std::endl;
    }
}

bool CommandRouter::setExitFlag()
{
    return exitFlag;
}

void CommandRouter::createMap()
{
    commandMap["date"] = [this](const char* params) { handlerDate(params); };
    commandMap["time"] = [this](const char* params) { handlerTime(params); };
    commandMap["exit"] = [this](const char* params) { handlerExit(params); };
    commandMap["faster"] = [this](const char* params) { handlerFaster(params); };
    commandMap["slower"] = [this](const char* params) { handlerSlower(params); };
    commandMap["pause"] = [this](const char* params) { handlerPause(params); };
    commandMap["resume"] = [this](const char* params) { handlerContinue(params); };
    commandMap["level 0"] = [this](const char* params) { handlerZiroLevel(params); };
    commandMap["level 1"] = [this](const char* params) { handlerFirstLevel(params); };
    commandMap["level 2"] = [this](const char* params) { handlerSecondLevel(params); };
    commandMap["stat"] = [this](const char* params) { handlerStat(params); };
}

void CommandRouter::printListCommands()
{
    std::cout << "Список комманд:\n"
        "date – печать текущей даты (введите дату в формате: DD-MM-YYYY, DD.MM.YYYY или DD,MM,YYYY)\n"
        "time – печать текущего времени (введите время в формате: HH:MM:SS, HH.MM.SS или HH,MM,SS)\n"
        "exit – завершить работу программы\n"
        "faster – уменьшить интервал между порождениями событий\n"
        "slower – увеличить интервал между порождением событий\n"
        "pause – приостановить генерацию событий\n"
        "resume – продолжить генерацию событий\n"
        "level 0 – установить нулевой уровень протоколирования\n"
        "level 1 – установить первый уровень протоколирования\n"
        "level 2 – установить второй уровень протоколирования\n"
        "stat – вывести количество произошедших событий\n";
}

void CommandRouter::handlerDate(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        std::cerr << "Ошибка: Не указан параметр для команды 'date'. Введите корректную дату в формате: DD-MM-YYYY, DD.MM.YYYY или DD,MM,YYYY" << std::endl;
        //если строка не содержит параметров - ввыводим в консоль текущую дату
        auto now = std::chrono::system_clock::now();                    //получаем текущее время
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);  //преобразуем текущее время в удобный для использования тип
        std::tm* local_tm = std::localtime(&now_c);                     //преобразуем в строку времени

        std::cout << "Текущая дата: ";
        std::cout << std::put_time(local_tm, "%d-%m-%Y\n") << std::endl;
    }
    else {
        //проверяем введенную дату на корректность
        const std::string str(params);
        if (isValidDate(params)) {
            std::cout << "Дата '" << params << "' корректна." << std::endl;
        }
        else {
            std::cout << "Дата '" << params << "' некорректна. Введите корректную дату в формате: DD-MM-YYYY, DD.MM.YYYY или DD,MM,YYYY" << std::endl;
        }
    }
}

void CommandRouter::handlerTime(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        std::cerr << "Ошибка: Не указан параметр для команды 'time'. Введите корректное время в формате: HH:MM:SS, HH.MM.SS или HH,MM,SS" << std::endl;
        //если строка не содержит параметров - ввыводим в консоль текущее время
        auto now = std::chrono::system_clock::now();                    //получаем текущее время
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);  //преобразуем текущее время в удобный для использования тип

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::cout << "Текущее время: ";
        std::cout << std::put_time(std::localtime(&now_c), "%H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << milliseconds.count() << std::endl;
    }
    else {
        //проверяем введенное время на корректность
        const std::string str(params);
        if (isValidTime(params)) {
            std::cout << "Время '" << params << "' корректно." << std::endl;
        }
        else {
            std::cout << "Время '" << params << "' некорректно. Введите корректное время в формате: HH:MM:SS, HH.MM.SS или HH,MM,SS" << std::endl;
        }
    }
}

void CommandRouter::handlerExit(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        exitFlag = true;
    }
    else {
        std::cout << "Для завершения работы программы введите команду 'exit' без параметров." << std::endl;
    }
}

void CommandRouter::handlerFaster(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        double currentInterv = eventGenerator->getIntervalAddition();
        double newInterv = currentInterv / 2.0;
        eventGenerator->setIntervalAddition(newInterv);
        std::cout << "Интервал между порождениями событий уменьшен со значений (min = " << currentInterv << " c, max = " << 2.0 + currentInterv << " c) до значений (min = " <<
            newInterv << " c, max = " << 2.0 + newInterv << " c)." << std::endl;
    }
    else {
        std::cout << "Для уменьшения интервала между порождениями событий введите команду 'faster' без параметров." << std::endl;
    }
}

void CommandRouter::handlerSlower(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        double currentInterv = eventGenerator->getIntervalAddition();
        double newInterv = currentInterv * 2.0;
        eventGenerator->setIntervalAddition(newInterv);
        std::cout << "Интервал между порождениями событий увеличен со значений (min = " << currentInterv << " c, max = " << 2.0 + currentInterv << " c) до значений (min = " <<
            newInterv << " c, max = " << 2.0 + newInterv << " c)." << std::endl;
    }
    else {
        std::cout << "Для увеличения интервала между порождениями событий введите команду 'slower' без параметров." << std::endl;
    }
}

void CommandRouter::handlerPause(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        eventGenerator->setFlagEventGenerator(false);
        std::cout << "Генерация событий приостановлена." << std::endl;
    }
    else {
        std::cout << "Для приостановки генерации событий введите команду 'pause' без параметров." << std::endl;
    }
}

void CommandRouter::handlerContinue(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        eventGenerator->setFlagEventGenerator(true);
        std::cout << "Генерация событий продолжена." << std::endl;
    }
    else {
        std::cout << "Для продолжения генерации событий введите команду 'resume' без параметров." << std::endl;
    }
}

void CommandRouter::handlerZiroLevel(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        logger->setLogLevel(0);
        std::cout << "Установлен нулевой уровень протоколирования." << std::endl;
    }
    else {
        std::cout << "Для установки нулевого уровня протоколирования введите команду 'level 0' без параметров." << std::endl;
    }
}

void CommandRouter::handlerFirstLevel(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        //нужен mtx
        logger->setLogLevel(1);
        std::cout << "Установлен первый уровень протоколирования." << std::endl;
    }
    else {
        std::cout << "Для установки первого уровня протоколирования введите команду 'level 1' без параметров." << std::endl;
    }
}

void CommandRouter::handlerSecondLevel(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        //нужен mtx
        logger->setLogLevel(2);
        std::cout << "Установлен второй уровень протоколирования." << std::endl;
    }
    else {
        std::cout << "Для установки второго уровня протоколирования введите команду 'level 2' без параметров." << std::endl;
    }
}

void CommandRouter::handlerStat(const char* params)
{
    if (params == nullptr || strlen(params) == 0) {
        std::cout << "Колличество произошедших событий: " << eventGenerator->getCountEvent() << std::endl;  
    }
    else {
        std::cout << "Для вывода колличества произошедших событий введите команду 'stat' без параметров." << std::endl;
    }
}

bool CommandRouter::isValidDate(const std::string date)
{
    int day, month, year;
    char delimiter1, delimiter2;

    //регулярное выражение для формата DD-MM-YYYY, DD.MM.YYYY, DD,MM,YYYY
    std::regex dateRegex(R"((\d{2})([-.,])(\d{2})\2(\d{4}))");
    std::smatch match;

    //проверка соответствия формату
    if (!std::regex_match(date, match, dateRegex)) {
        return false;       //неверный формат
    }

    //извлечение значений
    day = std::stoi(match[1]);
    month = std::stoi(match[3]);
    year = std::stoi(match[4]);

    //проверка диапазонов
    if (day < 1 || day > 31 || month < 1 || month > 12) {
        return false;
    }

    //проверка количества дней в месяце
    if (month == 2) {       //февраль
        return (isLeapYear(year) && day <= 29) || (!isLeapYear(year) && day <= 28);
    }

    //месяцы с 30 днями
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return day <= 30;
    }

    //месяцы с 31 днем
    return day <= 31;
}

bool CommandRouter::isValidTime(const std::string time)
{
    int hour, minute, second;
    char delimiter1, delimiter2;

    //регулярное выражение для формата HH:MM:SS, HH.MM.SS, HH,MM,SS
    std::regex timeRegex(R"((\d{2})([:.,])(\d{2})\2(\d{2}))");
    std::smatch match;

    //проверка соответствия формату
    if (!std::regex_match(time, match, timeRegex)) {
        return false;       //неверный формат
    }

    //извлечение значений
    hour = std::stoi(match[1]);
    minute = std::stoi(match[3]);
    second = std::stoi(match[4]);

    //проверка диапазонов
    if (hour < 0 || hour > 12 || minute < 0 || minute > 60 || second < 0 || second > 60) {
        return false;
    }

    return true;
    return false;
}

bool CommandRouter::isLeapYear(int year)
{
    return  (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
