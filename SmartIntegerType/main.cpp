#include <string>
#include <iostream>
#include "InputWorkerStandartStream.h"
#include "InputWorkerFileStream.h"
#include "SmartInteger.h"

int main ( int argc, char ** argv )
{
	// объявляем переменные, участвующие в вычислении. можно было бы обойтись всего двумя переменными типа SmartInteger на все вычисление
	// но в таком случае код получился бы не таким удобным.
	
	// переменные нативного типа потребуются для удобной проверки коррекнтности ввода данных
	long long   lb, ub, coef5, coef4, coef3, coef2, coef1, k;
	
	// переменные этого типа защищены от переполнения. Плата за это больший вес, и отсутствие плавающей точки
	// на выходе получаем не округление а отбрасывание дробной части, но это разумное допущение для вычисления больших величин
	// при необходимости класс SmartInteger можно усложнить до поддержки плавающей точки.
	SmartInteger  res1, res2, x5, x4, x3, x2, x1, kx;
	std::string buffer;
	
	// отключаем синхронизацию с Си потоками, тем самым ускорям Си++ потоки.
	// подробнее https://code-live.ru/post/cpp-input-output/
	std::ios_base::sync_with_stdio(false);
	
	try // орагнизуем ввод в трай блоке, позволяющем отлавливать ошибки.
	{
		if (argc > 1)
		{
			// оранизуем чтение данных из файла
			
			InputWorkerFileStream* inputPtr(new InputWorkerFileStream(argv[1]));
			*inputPtr >> buffer;
			lb = std::stoll(buffer);
			*inputPtr >> buffer;
			ub = std::stoll(buffer);
			*inputPtr >> buffer;
			coef5 = std::stoll(buffer);
			*inputPtr >> buffer;
			coef4 = std::stoll(buffer);
			*inputPtr >> buffer;
			coef3 = std::stoll(buffer);
			*inputPtr >> buffer;
			coef2 = std::stoll(buffer);
			*inputPtr >> buffer;
			coef1 = std::stoll(buffer);
			*inputPtr >> buffer;
			k = std::stoll(buffer);
			delete inputPtr;
		}
		else
		{
			// если программе не указан файл, организуем вво данных из самой программы
			InputWorkerStandartStream* inputPtr(new InputWorkerStandartStream);
			*inputPtr << "Input lower integral border" << std::endl;
			*inputPtr >> buffer;
			lb = std::stoll(buffer);
			*inputPtr << "Input upper integral border" << std::endl;
			*inputPtr >> buffer;
			ub = std::stoll(buffer);
			*inputPtr << "[]x^5+x^4+x^3+x^2+x+k\n";
			*inputPtr >> buffer;
			coef5 = std::stoll(buffer);
			*inputPtr << "x^5+[]x^4+x^3+x^2+x+k\n";
			*inputPtr >> buffer;
			coef4 = std::stoll(buffer);
			*inputPtr << "x^5+x^4+[]x^3+x^2+x+k\n";
			*inputPtr >> buffer;
			coef3 = std::stoll(buffer);
			*inputPtr << "x^5+x^4+x^3+[]x^2+x+k\n";
			*inputPtr >> buffer;
			coef2 = std::stoll(buffer);
			*inputPtr << "x^5+x^4+x^3+x^2+[]x+k\n";
			*inputPtr >> buffer;
			coef1 = std::stoll(buffer);
			*inputPtr << "x^5+x^4+x^3+x^2+x+[k]\n";
			*inputPtr >> buffer;
			k = std::stoll(buffer);
			delete inputPtr;
		}
	}
	catch (std::exception& wtf)
	{
		// во время выделения памяти, или работы со стандартными потоками, по причинами, не завищащим от программы
		// могут произойти ошибки (например не хватка памяти в системе, или потеря прав на файл, или физические проблемы с диском)
		// в этом блоке мы отлавлиаем любую ошибку, и в случае таковой прекращаем работу.
		std::cerr << wtf.what();
		return 1;
	}
	
	kx = k;
	x5 = x4 = x3 = x2 = x1 = ub;
	
	res1 = (((x5 ^ 6)*coef5) / 6) + (((x4 ^ 5)*coef4) / 5) + (((x3 ^ 4)*coef3) / 4)
	       + (((x2 ^ 3)*coef2) / 3) + (((x1 ^ 2)*coef1) / 2) + (kx*ub);
	
	x5 = x4 = x3 = x2 = x1 = lb;
	
	res2 = (((x5 ^ 6)*coef5) / 6) + (((x4 ^ 5)*coef4) / 5) + (((x3 ^ 4)*coef3) / 4)
	       + (((x2 ^ 3)*coef2) / 3) + (((x1 ^ 2)*coef1) / 2) + (kx*lb);
	
	std::cout << "Result: " << (res1 - res2) << std::endl;
	
	
	// в Windows тяжело с нормальной консолью, и обычно пускают кликом. Поэтому задерживаем окно по завершении работы программы
	#if _WIN32 || _WIN64
		// один гет опустошит буффер, в котором будет \n, т.к. он не входит при чтении строки оператором >> и остаётся в потоке
		// другой гет будет ожидать реакции пользователя.
		std::cin.get(); std::cin.get();
	#endif
	
	return (0);
}
