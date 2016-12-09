/* программа должна записать в файл в бинарном и текстовом виде (а читать из бинарного) данные о багаже (кол-во предметов и общий вес)
*  так же должна определить имеется ли среди пассажиров пара таких, вес багажа которых отличается не более чем на 0.5 единиц веса.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

#define SSIZE 256

struct Baggage
{
	float summaryWeight;
	unsigned itemsCount; // в структурах малые типы всегда должны идти вслед за большими, если это не сильно ухудшит читаемость кода.
	// но практически на всех современных архитектурах float == unsigned == 4 байта. А нужно же это, чтобы итоговый размер струтуры был меньше, т.к. существует "выравнивание" по большему из членов.
};


int writeBaggageBin(const char * data, FILE * filePtr)
{
	// запись данных в бинарном виде. По сути в промышленном, а не учебном программировании такой подход используется
	// только тогда, когда не предполагается обмен такими фалами между ЭВМ различных архитектур. Подробнее почему так,
	// можно найти в поисковиках по запросу "си выравнивание структур", "сериализация данных".

	struct Baggage b;

	if ( 2 != sscanf(data, "%d %f", &b.itemsCount, &b.summaryWeight) )
	{
		return -1;
	}
	// Хорошим тоном в программировании является проверка того что ф-я отработала успешно. frite отработал успешно если кол-во записей которые он сделал
	// равно тому кол-ву которое мы попросили сделать, это 1.
	return ( 1 == fwrite(&b, sizeof b, 1, filePtr) ) ? 1 : 0;
}

int writeBaggageText(const char * data, FILE * filePtr)
{
	int i; float f;

	if ( 2 != sscanf(data, "%d %f", &i, &f) )
	{
		return -1;
	}
	// здесь же, в отличие от fwrite - при ощибке возвращается отрицательное число
	// (if <a>)?<b>:<c> - является единственным в Си и Сиподобных языках тренарным оператором. Описан здесь http://www.cplusplus.com/doc/tutorial/operators/ ctrl+f "conditional operator"
	return ( 0 > fprintf(filePtr,"%d %f\n", i, f) ) ? 0 : 1;
}



int main (int argc, char * argv[])
{
	int i, j, s;
	printf("Program usig rules:\n\tinput item count and (with space) summary weight %s",
       "(for example 12 3.5) and press enter\n\tfor exit from input process input \"exit\" and press enter\n");
	
	// статическая строка размещается в сегменте BSS и изначальное зануление не нужно
	// в данной задаче выгоднее использовать статический массив, нежели терять время выполнения на обращение к malloc
	char string[SSIZE];
	
	FILE *fBin;
	FILE *fText;
	
	fBin  =fopen("data.bin","w"); // для бинарных чтения и записи чатенько пишут 'b', но это вовсе не нужно, т.к. это просто напоминание программисту что он хочет работать с файлом бинарно
                                  // никакой другой пользы не даёт, поэтому не будет использовать лишние символы. И просто откроем файл на запись
	fText =fopen("data.txt","w");

	// проверяем открылись ли на запись оба файла	
	if (!fBin || !fText)
	{
		printf("Unable to open files to write!\n");
		return (1);
	}
	

	while (1)
	{
		fgets(string, SSIZE-1, stdin);
		
		
		if ( !strcmp("exit\n", string) )
		{
			break;
		}
		
		i = writeBaggageBin(string, fBin);

		if ( 1 == i )
		{
			// наиболее частое условие принято размещать первым, так программа будет работать быстрее в виду особенностей предсказателя перехода процессора
			writeBaggageText(string, fText);
		}
		else if ( -1 == i )
		{
			printf("Not Correct Input! Try Aggain\n");
			continue;
		}
		else
		{
			printf("Unable to write on files!\n");
			return 1;
		}
	}
	
	fclose(fBin);
	fclose(fText);
	
	// читаем записанное данное, 'b' здесь не обязательно
	
	fBin = fopen("data.bin","r");
	
	if (!fBin)
	{
		printf("Unable to open files to write!\n");
		return (1);
	}
	
	// узнаем размер файла
	fseek(fBin, 0L, SEEK_END);
	s = ftell(fBin);
	fseek(fBin, 0L, SEEK_SET);
	
	
	//выделяем память для получения записей, т.к. пишем на си, то явно приводить void* к чему-то нет необходимости.
	struct Baggage * data = malloc (s);
	if (!data)
	{
		printf("Unable to memmory allocation!\n");
	}
	
	// узнаём количество записей в файле
	s /= sizeof (struct Baggage);
	

	// читаем
	for (i = 0; i < s; ++i)
	{
		fread ( data+i, sizeof(struct Baggage), 1, fBin );
	}
	
	
	// определяем
	for (i = 0; i < s - 1; ++i)
	{
		for (j = i + 1; j < s; ++j)
		{
			if (fabs((data+i)->summaryWeight - (data+j)->summaryWeight) <= 0.5f)
			{   // не будем печатать лишние нули, большая точность здесь мешает читать вывод .3 - усекает вывод до 3 чисел после точки.
				printf("YES: %.3f and %.3f from passanger %d and %d \n", (data+i)->summaryWeight, (data+j)->summaryWeight, 1+i, 1+j);
				j = -1;
				break;
			}
		}
		if (j < 0) break;
	}
	
	
	if (j >= 0) printf("NO\n");
	
	
	free(data);
	fclose(fBin);
	
	return 0;
}

