#ifndef SMARTINTEGER_OVERFLOWCHECKS_H
#define SMARTINTEGER_OVERFLOWCHECKS_H

#include <limits>

// шаблоны удобны, т.к. могут в последствии использоваться не только с нашим классов, т.е. отвязываемся от
// конкретных типов

// проверка на переполнение сложения
template <typename T> bool overflow_addition(T a, T b)
{
	if ((b > 0) && (a > std::numeric_limits<T>::max() - b))
	{
		// если не понятна арифметика проверки, можно представить чиловую прямую...
		// если b больше нуля, и при этом в крайний правый промежуток
		// a влезти не может (т.к. больше, правее этой границы)
		// то очевидно что при сложении а и b значение буде правее чем граница max().
		return true;
	}
	if ((b < 0) && (a < std::numeric_limits<T>::min() - b))
	{
		// а тут можно представить левые границы
		return true;
	}
	return false;
}

// проверка на переполнение вычитания
template <typename T> bool overflow_substration(T a, T b)
{
	if ((b < 0) && (a > std::numeric_limits<T>::max() + b))
	{
		return true;
	}
	if ((b > 0) && (a < std::numeric_limits<T>::min() + b))
	{
		// если b>0, но при этом самый левый край + b формируют такую границу, что а меньше (левее) этой границы
		// то это говорит о том при вычитании из а - b, результирующая граница переместиться еще левее чем min.
		return true;
	}
	return false;
}

// проверка на переполнение умножения
template <typename T> bool overflow_multiplication(T a, T b)
{
	// с проверкой переполнения при умножении все тривиально:
	// если t/a != b, где t==a*b - то при вычислении случилось переполнение
	T t = a * b;
	if ((0 != a) && (b != t / a))
	{
		return true;
	}
	return false;
}

// данные проверки тратят вычислительное время. но являются универсальными, если бы проверки производились на беззнаковых
// типах то можно было бы использовать свойства битов чисел, и делать её более эффеткивно, не не универсально.


#endif //SMARTINTEGER_OVERFLOWCHECKS_H
