#include "InputWorkerFileStream.h"

InputWorkerFileStream & InputWorkerFileStream::operator<<(std::ostream&(*modificator)(std::ostream&))
{
	return *this;
}

InputWorkerFileStream & InputWorkerFileStream::operator<<(const char * str)
{
	return *this;
}

InputWorkerFileStream & InputWorkerFileStream::operator >> (std::string & str)
{
	bool appened { false };
	int ch { 0 };
	
	str.clear();
	
	// лямбда проверки на разделитель
	bool(*is_spaser)(int) = [](int ch)
	{
		switch (ch)
		{
			case '\n':
			case '\t':
			case ' ': return true;
			default:  return false;
		}
	};
	
	// чтение ведем если истины два утверждения: значение ch не соответствует концу файла
	// а так же не является пробелом, табуляцией или переводом строки (т.е. является любым другим символом)
	while (!is_spaser(ch) && ch != EOF)
	{
		ch = f_.get();
		if (is_spaser(ch) && !appened)
		{
			// если разделитель, но числое еще не было прочтено, то отменяем условие выхода из цикла
			// т.к. 0 внесенный в левые части строки не влияет на её отображение в целочисленный тип.
			ch = 0;
		}
		if (isdigit(ch) || ('-' == ch && isdigit(f_.peek())))
		{
			appened = true;
			// преобразуем число в символ и прибавляем к строке. изначально с числом работает, т.к.
			// метод get возвращает число (вернее символ но в типе числа, сделано это видимо т.к.
			// не везде стандартизовано что один символ это 1 байт, хотя и общепринято)
			str += static_cast<char>(ch);
		}
		else continue;
	}
	
	return *this;
}