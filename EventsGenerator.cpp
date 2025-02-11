#include "EventsGenerator.h"

EventsGenerator::EventsGenerator(/*HANDLE writePipe*/PipeType writePipe) : pipeWriteFd(writePipe), countEvents(0)
{
}

EventsGenerator::~EventsGenerator(void)
{
}

void EventsGenerator::generation()
{
	while (true) {

		//�������� ����� � ������ ����� ��� ������
		if (flagExit.load()) {
			break;		//������� �� �����, ���� ���� ���������� � true
		}

		//���� ������ (���������) �����, � ���������� �������, ���� ��������� ������� �� ��������������
		int randomPart = (rand() % 3);                  //c�������� ����� �� 0 �� 2
		int sleepTime = static_cast<int>((randomPart * 1000) + (intervalAddition * 1000));
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));

		if (flagEventGenerator.load()) {
			countEvents += 1;                               //����������� ������� ������� (� ������� �������: countEvents = 1)
			
			Event event;
			auto now = std::chrono::system_clock::now();
			std::time_t now_c = std::chrono::system_clock::to_time_t(now);

			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

			//������� ��������� ����� ��� �������������� �������
			std::ostringstream timeStream;
			timeStream << std::put_time(std::localtime(&now_c), "%d-%m-%Y %H:%M:%S")
				<< '.' << std::setw(3) << std::setfill('0') << milliseconds.count();

			std::strcpy(event.eventTime, timeStream.str().c_str());		//����������� ����� � ������

			event.eventId = countEvents;
			event.param1 = rand() % 10;
			event.param2 = rand() % 10;
			event.param3 = rand() % 10;
			//������ ������� � ����
#ifdef _WIN32
			DWORD bytesWritten;
			if (!WriteFile((HANDLE)pipeWriteFd, &event, sizeof(event), &bytesWritten, NULL)) {
				std::cerr << "WriteFile failed: " << GetLastError() << std::endl;
			}
			else {
				//std::cout << "Wrote " << bytesWritten << " bytes to pipe" << std::endl;
			};
#else
            ssize_t bytesWritten = write(pipeWriteFd, &event, sizeof(event));
			if (bytesWritten < 0) {
				std::cerr << "Write error: " << strerror(errno) << std::endl;
			}
			else {
				//std::cout << "Wrote " << bytesWritten << " bytes to pipe" << std::endl;
			}
#endif
		}
		else
		{
			//������������� ���������� ����� �� ��� ���, ���� ���� �� ����� ������� �� true
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); //����� �������, ����� �� ����������� ���������
		}
	}
}

void EventsGenerator::setFlagEventGenerator(bool flag)
{
	std::lock_guard<std::mutex> lock(mtx);
	flagEventGenerator.store(flag);
}

void EventsGenerator::setFlagExit(bool flag)
{
	std::lock_guard<std::mutex> lock(mtx);
	flagExit.store(flag);
}

double EventsGenerator::getIntervalAddition()
{
	return intervalAddition;
}

void EventsGenerator::setIntervalAddition(double new_interval)
{
	std::lock_guard<std::mutex> lock(mtx);
	intervalAddition = new_interval;
}

int EventsGenerator::getCountEvent()
{
	return countEvents;
}
