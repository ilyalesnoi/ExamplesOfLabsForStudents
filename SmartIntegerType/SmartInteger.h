//
// Created by user on 06.12.16.
//

#ifndef SMARTINTEGER_H
#define SMARTINTEGER_H


#if _WIN32 || _WIN64
	#include "boost_1_60_0\boost\multiprecision\cpp_int.hpp"
	// буст потребуется для символьных вычислений
#elif __GNUC__
	#include <boost/multiprecision/cpp_int.hpp>
#endif
using boost::multiprecision::cpp_int;

// пользовательский класс числа
// основная задача которого производить элементарные арифметические действия над собой, при этом изменять своё
// внутреннее состояние, в случае если на очередном действии может случиться переполение, и в этом случае обращаться
// к символьным вычислениям, реализованным в библиотеке буст.

// класс использует перегрузку методов для обеспечения над собой арифметических операций, это делает его удобным
// в использовании.
class SmartInteger
{
public:
	SmartInteger(long long v = 0);
	
	SmartInteger(const SmartInteger & rhs);
	
	SmartInteger(const cpp_int & val);
	
	~SmartInteger();
	
	SmartInteger& operator=(long long n);
	
	SmartInteger& operator=(const SmartInteger& rhs);
	
	SmartInteger operator^(long long n);
	
	SmartInteger operator*(long long n);
	
	SmartInteger operator*(const SmartInteger& rhs);
	
	SmartInteger operator/(long long n);
	
	SmartInteger operator+(long long n);
	
	SmartInteger operator+(const SmartInteger& rhs);
	
	SmartInteger operator-(long long n);
	
	SmartInteger operator-(const SmartInteger& rhs);

private:
	friend std::ostream& operator << (std::ostream& os, const SmartInteger& sd);
	friend std::istream& operator >> (std::istream& os, SmartInteger& sd);
	
	SmartInteger& mutate_to_symbolic(void);

	// поле хранения нативного типа
	long long stack_val;
	
	// поле хранения символного типа (см. в поисковике "символьные вычиления" или "длинная арифметика"
	// вкратце компьютер (аппаратно) умеет считать только величины конечного размера. Если значение перерастает этот
	// размер, случается "переполнение" и результат вычилений будет ошибочен. Для вычислений очень больших чисел,
	// принято использовать метод символьных вычислений, грубо говоря это метод в котором мы учим компьютер считать
	// числа не аппаратно а программно, причем почти по тем же правилам, которые учат младшеклассника в школе, например
	// умножение в столбик, умножая мы не представляем себе величину а работаем с мнеманическими символьными
	// преобразованиями. Так же и здесь	но лично нам учить компьютер этому не нужно, т.к. он уже обучен этому
	// библиотекой boost. (хотя иногда бывают лаболаторные именно на тему сделать символьные вычисления самому, но это
	// уже весьма сложная лаболатораная).
	//
	// теперь подход ооп в реалиях си++:
	// т.к. о внутренностях типа cpp_int нам ничего не известно, но предположительно он большой, и его создание требует
	// время, а хренения ресурсов причем физически никак не меньших чем размер указателя, то естественно в нашем типе мы
	// будем хранить просто указатель, а сам cpp_int создавать не будем.
	// Возможно он вообще не потребуется. А вот когда потребуется, он будет создан автоматически.
	cpp_int * heap_symbolic_val;
};



#endif //SMARTINTEGER_H
