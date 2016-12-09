#include <memory>
#include "SmartInteger.h"
#include "OverflowChecks.h"

// нативные типы принято передавать по значению, хотя в некоторых архитектурах long long превышает размер указателя, а
// следовательно передача по константной ссылке была бы предочтительнее.
SmartInteger::SmartInteger(long long v) : stack_val(v), heap_symbolic_val(nullptr)
{
	//конструктор преобразования из нативного типа. Пишем входящее значение во внутренний тип, указатель на символьное
	// число зануляем
}

// в конструкторе копирования передача уже по ссылке
SmartInteger::SmartInteger(const SmartInteger & rhs) : stack_val(rhs.stack_val), heap_symbolic_val(nullptr)
{
	// в конструкторе копирования необходимо учесть явялеется ли rhs уже длинным числом или внутри него нативный тип
	// хранит значение.
	if (rhs.heap_symbolic_val)
	{
		try
		{
			heap_symbolic_val = new cpp_int(*rhs.heap_symbolic_val);
		}
		catch (std::exception& wtf)
		{
			std::cerr << wtf.what();
		}
	}
}

// в списке инициализации выгодняя использовать запрет исключений оператора new (std::nothrow), чтобы иметь возможность
// обработать исключительную ситуацию (не исключение а ситуацию) в теле конструктора
SmartInteger::SmartInteger(const cpp_int & val) : heap_symbolic_val(new (std::nothrow) cpp_int(val))
{
	// конструктор преобразования. Здесь уже потребуется создать объект cpp_int.
	if (nullptr == heap_symbolic_val)
	{
		std::cerr << "Can't allocate memory\n";
		abort();
	}
}

SmartInteger::~SmartInteger()
{
	delete heap_symbolic_val;
}

SmartInteger & SmartInteger::operator=(long long n)
{
	stack_val = n;
	if (heap_symbolic_val)
	{
		delete heap_symbolic_val;
		heap_symbolic_val = nullptr;
	}
	return *this;
}

SmartInteger & SmartInteger::operator=(const SmartInteger & rhs)
{
	// если я не rhs то присвою себе его значения
	if (this != &rhs)
	{
		// если rhs хранит в cpp_int
		if (rhs.heap_symbolic_val)
		{
			// и я храню в cpp_int
			if (heap_symbolic_val)
			{
				// то присвою его значение
				*heap_symbolic_val = *rhs.heap_symbolic_val;
			}
			else
			{
				// но если до этого момента не хранил, то выделю память и сконструрую новый cpp_int со значением rhs
				heap_symbolic_val = new cpp_int(*rhs.heap_symbolic_val);
			}
		}
		else //если rhs хранит в лонг лонге
		{
			// а я в cpp_int
			if (heap_symbolic_val)
			{
				// то и я могу в лонг лонге, и удаляю тогда cpp_int
				delete heap_symbolic_val;
				heap_symbolic_val = nullptr;
			}
		}
		// присваиваю rhs значение.
		stack_val = rhs.stack_val;
	}
	return *this; // вернем себя
}

// по подобным рассуждениям будут работаь и арифетические операторы ниже. Их существенным отличием является то что они
// будут возвращать не себя а копию, созданную из себя + прибавленное значение. Если бы вычисления делались на себе а не
// на копии, это было б черевато ошибками при повторном использовании переменных.

SmartInteger SmartInteger::operator^(long long n) // поэтому здесь возвращаем не ссылку а объект SmartInteger.
{   //это нетипичная перегрузка оператора ^, для возведения в степень,
	// остальные перегузки соответствую своим отбычным ф-ям.
	long long i = 1;
	long long ret = stack_val;
	// здесь мы вынуждены использовать умный указатель, т.к. мы культурные люди
	// и не хотим чтобы память утекла после ретурна.
	std::unique_ptr<cpp_int> tmp_cpp_int = nullptr;
	try
	{
		for (; i < n; i++)
		{
			// если цпп_инт не испольщуется, и нет переполнения
			if (!heap_symbolic_val && !overflow_multiplication(stack_val, n))
			{
				ret *= stack_val;
			}
			else if (!heap_symbolic_val) // если же при следующем умножении будет переполнение то множем уже здесь:
			{
				tmp_cpp_int.reset(new cpp_int(stack_val));
				break;
			}
			else // сюда мы попадём только  тогда когда возмодимое в степень число уже хранится в cpp_int и это первая
				 // итерация цикла
			{
				tmp_cpp_int.reset(new cpp_int(*heap_symbolic_val));
				break;
			}
		}
	}
	catch (std::exception& wtf)
	{
		std::cerr << wtf.what();
	}
	if (i != n)
		// здесь мы либо домножим оставшее после перевода в cpp_int или же будем множить с 1 до n если
		// перевода не было, но было сразу в cpp_int.
	{
		for (; i < n; i++)
		{
			*tmp_cpp_int *= stack_val;
		}
	}
	if (tmp_cpp_int != nullptr)
	{
		return SmartInteger(*tmp_cpp_int.get()); // возвращаем копию значения
		// вот здесь и пригодится умный указатель, т.к.  отличие от нативного (или еще говорят сырого)
		// он обладает деструктором. Деструктор же вызывается при "выходе переменной из области видимости". Выход этот
		// происходит при выходе вычилительного потока из ф-ии(метода). Деструктор уничтожает объект хранящийся где-то в
		// памяти, и вызывает освобождение памяти нативный же указатель уничтожается сам, но объект не уничтожает и
		// не вызывает очистку памяти. Поэтому с ним была бы "утечка".
		
		// впрочем, мы бы могли обойтись и без умного указателя, причем создавая объект cpp_int именно когда нужно
		// на стеке (используя области видимости) но код тогда бы был менее компактен и красив.
	}
	else
	{
		return SmartInteger(ret); // возвращаем копию значения
	}

}

SmartInteger SmartInteger::operator*(long long n)
{
	if (!heap_symbolic_val && !overflow_multiplication(stack_val, n))
	{
		return SmartInteger(stack_val*n);
	}
	else
	{
		if (nullptr == heap_symbolic_val)
		{
			return (SmartInteger(*this).mutate_to_symbolic() * n);
		}
		else
		{
			// здесь же мы вызываем конструктор SmartInteger(const cpp_int & val)
			return (static_cast<cpp_int>(*heap_symbolic_val * n));
			// при этом применяя явное преобразование типов, т.к. cpp_int на самом деле является
			// конкретизацией некоторых шаблонов boost, при перемножении тип итогового выражения cpp_int * some_type
			// возможно будет отличаться от типа cpp_int, но при этом данный новый тип является приводимым к cpp_int
			// но не все компиляторы готовы произвести компиляцию без указания явного преобразования. Вероятно это
			// связано с механикой дедукции шаблонных типов компиляторами, а не умением компиляторов преобразовывать
			// что-то к чему-то т.к. это они всегда делают хорошо, причем на столько, что лучше бы делали хуже :)
			// подробнее можно читать "проблемы неявного приведения типов в си++" в поисковике.
			
		}
	}
}

SmartInteger SmartInteger::operator*(const SmartInteger & rhs)
{
	if (!rhs.heap_symbolic_val)
	{
		if (!heap_symbolic_val && !overflow_multiplication(stack_val, rhs.stack_val))
		{
			return SmartInteger(stack_val * rhs.stack_val);
		}
		else if (!heap_symbolic_val)
		{
			cpp_int tmp(stack_val);
			return SmartInteger(static_cast<cpp_int>(tmp * rhs.stack_val));
		}
		else
		{
			return SmartInteger(static_cast<cpp_int>(*heap_symbolic_val * rhs.stack_val));
		}
	}
	else
	{
		if (!heap_symbolic_val)
		{
			return SmartInteger(static_cast<cpp_int>(*rhs.heap_symbolic_val * stack_val));
		}
		else
		{
			return SmartInteger(static_cast<cpp_int>(*rhs.heap_symbolic_val * *heap_symbolic_val));
		}
	}
	
}

SmartInteger SmartInteger::operator/(long long n)
{
	if (!heap_symbolic_val)
	{
		return SmartInteger(stack_val / n);
		
	}
	else
	{
		cpp_int tmp(stack_val);
		return SmartInteger(static_cast<cpp_int>(tmp / n));
	}
}

SmartInteger SmartInteger::operator+(long long n)
{
	if (!heap_symbolic_val && !overflow_addition(stack_val, n))
	{
		return SmartInteger(stack_val + n);
	}
	else
	{
		if (nullptr == heap_symbolic_val)
		{
			return (SmartInteger(*this).mutate_to_symbolic() + n);
		}
		else
		{
			return (static_cast<cpp_int>(*heap_symbolic_val + n));
		}
	}
}

SmartInteger SmartInteger::operator+(const SmartInteger & rhs)
{
	// если прибавляемое число хранится на стеке в нативном типе
	if (!rhs.heap_symbolic_val)
	{
		// если текущее число + прибавляемое помеящается в нативный тип на стек
		if (!heap_symbolic_val && !overflow_addition(stack_val, rhs.stack_val))
		{
			return SmartInteger(stack_val + rhs.stack_val);
		}
			// случай когда сумма не может поместиться в стек
		else if (!heap_symbolic_val)
		{
			cpp_int tmp(stack_val);
			return SmartInteger(static_cast<cpp_int>(tmp + rhs.stack_val));
		}
			// случай, когда текущее число хранится не в стеке
		else
		{
			return SmartInteger(static_cast<cpp_int>(*heap_symbolic_val + rhs.stack_val));
		}
	}
		// прибавляемое число есть больше (символы в куче)
	else
	{
		// текущее хранится в нативном типе
		if (!heap_symbolic_val)
		{
			return SmartInteger(static_cast<cpp_int>(*rhs.heap_symbolic_val + stack_val));
		}
			// текущее число так же большое
		else
		{
			return SmartInteger(static_cast<cpp_int>(*rhs.heap_symbolic_val + *heap_symbolic_val));
		}
	}
}

SmartInteger SmartInteger::operator-(long long n)
{
	if (!heap_symbolic_val && !overflow_substration(stack_val, n))
	{
		return SmartInteger(stack_val - n);
	}
	else
	{
		if (nullptr == heap_symbolic_val)
		{
			return (SmartInteger(*this).mutate_to_symbolic() - n);
		}
		else
		{
			return (static_cast<cpp_int>(*heap_symbolic_val - n));
		}
	}
}

SmartInteger SmartInteger::operator-(const SmartInteger & rhs)
{
	if (!rhs.heap_symbolic_val)
	{
		if (!heap_symbolic_val && !overflow_substration(stack_val, rhs.stack_val))
		{
			return SmartInteger(stack_val - rhs.stack_val);
		}
		else if (!heap_symbolic_val)
		{
			cpp_int tmp(stack_val);
			return SmartInteger(static_cast<cpp_int>(tmp - rhs.stack_val));
		}
		else
		{
			return SmartInteger(static_cast<cpp_int>(*heap_symbolic_val - rhs.stack_val));
		}
	}
	else
	{
		if (!heap_symbolic_val)
		{
			return SmartInteger(static_cast<cpp_int>(*rhs.heap_symbolic_val - stack_val));
		}
		else
		{
			return SmartInteger(static_cast<cpp_int>(*rhs.heap_symbolic_val - *heap_symbolic_val));
		}
	}
	
}

SmartInteger& SmartInteger::mutate_to_symbolic(void)
{
	try
	{
		heap_symbolic_val = new cpp_int(stack_val);
	}
	catch (std::exception &wtf)
	{
		std::cerr << wtf.what();
	}
	return *this;
}

std::ostream & operator<<(std::ostream & os, const SmartInteger & sd)
{
	if (!sd.heap_symbolic_val)
	{
		os << sd.stack_val;
	}
	else
	{
		os << *sd.heap_symbolic_val;
	}
	return os;
}

std::istream & operator >> (std::istream & os, SmartInteger & sd)
{
	long long buff;
	os >> buff;
	if (sd.heap_symbolic_val)
	{
		*sd.heap_symbolic_val = buff;
	}
	else
	{
		sd.stack_val = buff;
	}
	return os;
}