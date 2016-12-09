//
// Created by user on 06.12.16.
//

#ifndef INPUTWORKERFILESTREAM_H
#define INPUTWORKERFILESTREAM_H


#include <fstream>

class InputWorkerFileStream
{
public:
	InputWorkerFileStream( )  = default;
	InputWorkerFileStream(const char * name) : f_(name, std::ios_base::in) // конструирование в списке инициализации
	{
		// конструирование в списке инициализации является правильным подходом, т.к. в таком случае используется
		// конструирование на прямую. иначе объект файла создался бы с конструктором по умолчанию, далее нужно
		// было бы вызывать необходимые методы, это не так оптимально. Хотя в данном случае мы всерано должны
		// прибегнуть к вызову методов.
		if (true == f_.is_open())
		{
			f_.exceptions(std::fstream::badbit);
		}
		else
		{
			std::perror("Error open file");
		}
	}
	
	// данные два оператора просто заглушки, для обеспечения схожести интерфейсов классов.
	// Хотя они и не являются родственными на уровне наследования,
	// но обеспечение схожести интерфейсов - хорошая идея проектирования.
	InputWorkerFileStream& operator<<(std::ostream&(*)(std::ostream&));
	
	InputWorkerFileStream& operator<<(const char*);
	
	// данный же оператор будет широко использоваться
	InputWorkerFileStream& operator >> (std::string&);

private:
	std::fstream f_;
};



#endif //INPUTWORKERFILESTREAM_H
