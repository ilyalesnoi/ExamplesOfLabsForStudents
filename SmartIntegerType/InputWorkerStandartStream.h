#ifndef INPUTWORKERSTANDARTSTREAM_H
#define INPUTWORKERSTANDARTSTREAM_H

#include <iostream>
#include <string>

class InputWorkerStandartStream
{
public:
	InputWorkerStandartStream& operator<<(std::ostream&(*)(std::ostream&));
	
	InputWorkerStandartStream& operator<<(const char * );
	
	InputWorkerStandartStream& operator >> (std::string &);
};


#endif
