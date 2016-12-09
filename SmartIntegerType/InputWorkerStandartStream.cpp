#include "InputWorkerStandartStream.h"

InputWorkerStandartStream & InputWorkerStandartStream::operator<<(std::ostream &(*modificator)(std::ostream &))
{
	std::cout << modificator;
	return *this;
}

InputWorkerStandartStream & InputWorkerStandartStream::operator<<(const char * str)
{
	std::cout << str;
	return *this;
}

InputWorkerStandartStream & InputWorkerStandartStream::operator>>(std::string & str)
{
	std::cin >> str;
	return *this;
}