// MPI_lab.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"

#include "mpi.h"
#include <iostream>
#include <windows.h>
#include <ctime>
#include <queue>

#define NO_EDGE 0

using namespace std;

struct Too_Int
{
	int value;
	int proc;
};

//19. Поиск кратчайших путей из одной вершины (алгоритм Дейкстры).
int main(int argc, char **argv)
{
	srand(time(0));

	int ProcNum, ProcRank;
	MPI_Init(&argc, &argv);
	MPI_Status status;

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	
	//Начало
		//Чтение данных		
		//Матрицы M x N (M - строк, N - столбцов)
	    int SizeVertex = 1000;

		int min = 0;
		int max = 1;

		if (argc > 1)
	    	SizeVertex = atoi(argv[1]);
		if (argc > 2)
			max = atoi(argv[2]);
		if (argc > 3)
			min = atoi(argv[3]);
		
		
		////////////////////////////////////////
		int  MAX_PROCCES = SizeVertex;


		MPI_Group UsingProcces;
		MPI_Comm_group(MPI_COMM_WORLD, &UsingProcces);

		int usingProcces;
		if (ProcNum > MAX_PROCCES)
			usingProcces = MAX_PROCCES;
		else usingProcces = ProcNum;

		int* ArrUsingProcces = new int[usingProcces];
		for (int i = 0; i < usingProcces; i++)
		{
			ArrUsingProcces[i] = i;
		}
		MPI_Group_incl(UsingProcces, usingProcces, ArrUsingProcces, &UsingProcces);
		delete[] ArrUsingProcces;


		MPI_Comm Using;
		MPI_Comm_create(MPI_COMM_WORLD, UsingProcces, &Using);

		////////////////////////////////

		if (ProcRank < usingProcces)
		{

		int* loadProcces = new int[usingProcces];

		for (int i = 0; i < usingProcces; i++)
		{
			loadProcces[i] = SizeVertex / usingProcces;
		}
		//нагрузить процессы, отличные от 0 сильнее
		for (int i = 1; i < (SizeVertex % usingProcces); i++)
		{
			loadProcces[i]++;
		}

		int* indexWork = new int[usingProcces];
		indexWork[0] = 0;
		for (int i = 1; i < usingProcces; i++)
		{
			indexWork[i] = indexWork[i - 1] + loadProcces[i - 1];
		}

			int* ArrResult = new int[loadProcces[ProcRank]];
			//int* ArrBuff = new int[loadProcces[ProcRank]];
			bool* Mark = new bool[loadProcces[ProcRank]];

			Too_Int local_min;

			//НУЛЕВОЙ
			if (ProcRank == 0)
			{
				//Инициализация
				int** MatrixAdjacency = new int*[SizeVertex];
				for (int i = 0; i < SizeVertex; i++)
				{ 
					MatrixAdjacency[i] = new int[SizeVertex];
				}

				for (int i = 0; i < SizeVertex; i++)
				{
					for (int j = i; j < SizeVertex; j++)
					{
						if (i != j) 
							MatrixAdjacency[i][j] = MatrixAdjacency[j][i] = (rand() % (max - min + 1) + min) ;
						else MatrixAdjacency[i][j] = NO_EDGE;
					}
				}

			 // 0 - нет ребра
				 
				//print Matrix incedent

				//for (int i = 0; i < SizeVertex; i++)
				//{
				//	for (int j = 0; j < SizeVertex; j++)
				//	{
				//		cout << MatrixAdjacency[0][j] << " ";
				//	}
				//	cout << "\n";
				//}
				//cout << "\n";

				//////////////////////////////////////////////////////паралельн алгоритм ///////////////////////////////////////////////////
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				unsigned int start_paralel_time = clock();

				int* Dist = new int[SizeVertex];
				
		     	for (int i = 0; i < loadProcces[ProcRank]; i++)
				{
					//ArrResult[i] = INT_MAX;
					Mark[i] = false;
				}

				bool* passedVertex = new bool[SizeVertex];
				for (int i = 0; i < SizeVertex; i++)
				{
					Dist[i] = INT_MAX;
					passedVertex[i] = false;
				}
				Dist[0] = 0;
		
				
				bool flag = true;
				Too_Int global_min;

				while (flag)
				{
					MPI_Bcast(&flag, 1, MPI_BYTE, 0, Using);
					//поиск минимального не помеченного
					//нужно передать 2 массива (метки и dist)   метки обновляются при поиске непомеченных


					local_min.value = INT_MAX;
					local_min.proc = INT_MAX;

					MPI_Scatterv(Dist, loadProcces, indexWork, MPI_INT, ArrResult, loadProcces[ProcRank], MPI_INT, 0, Using);
					for (int i = 0; i < loadProcces[ProcRank]; i++)    //для 0                                                     /////////////////???????????????????????
					{
						if (Mark[i] == false)
							if (ArrResult[i] <= local_min.value)
							{
								local_min.value = ArrResult[i];
								local_min.proc = i + indexWork[ProcRank];
							}
					}
					MPI_Reduce(&local_min, &global_min, 1, MPI_2INT, MPI_MINLOC, 0, Using);

					passedVertex[global_min.proc] = true;



					//пересчёт дистанций
					for (int i = 0; i < SizeVertex; i++)                                                          /////////////////???????????????????????
					{
						if (!passedVertex[i])
							if (MatrixAdjacency[global_min.proc][i] != NO_EDGE)
								Dist[i] = min(Dist[i], Dist[global_min.proc] + MatrixAdjacency[global_min.proc][i]);
					}

					// обновление дистанций
					//MPI_Scatterv(Dist, loadProcces, indexWork, MPI_INT, ArrResult, loadProcces[ProcRank], MPI_INT, 0, Using);

				
					// проверка на работу
					
					bool temp_flag = false;
					//oбновление меток
					MPI_Scatterv(passedVertex, loadProcces, indexWork, MPI_BYTE, Mark, loadProcces[ProcRank], MPI_BYTE, 0, Using);
					for (int i = 0; i < loadProcces[ProcRank]; i++)                                          /////////////////???????????????????????
					{
						if (Mark[i] == false)
							temp_flag = true;
					}
					MPI_Reduce(&temp_flag, &flag, 1, MPI_BYTE, MPI_BOR, 0, Using);
				}

				MPI_Bcast(&flag, 1, MPI_BYTE, 0, Using);

				unsigned int end_paralel_time = clock();
				
				delete[] passedVertex;

				/*cout << "parralel algorithm " << "\n";
				for (int i = 0; i < SizeVertex; i++)
				{
					cout << Dist[i] << " ";
				}
				cout << endl;

*/



				cout << "\ntime paralel = " << (end_paralel_time - start_paralel_time) << endl;

				////////////////////////////////////////////////однопоточный алгоритм/////////////////////////////////////////////////////


				unsigned int start_sequential_time = clock();
				

				int* Dist2 = new int[SizeVertex];
				bool* passedVertex2 = new bool[SizeVertex];
				for (int i = 1; i < SizeVertex; i++)
				{
					passedVertex2[i] = false;
					Dist2[i] = INT_MAX;
				}
				passedVertex2[0] = false;
				Dist2[0] = 0;

				int temp;
				int index2;
				int min_2;
				
				bool flagWork = true;
				while (flagWork)
				{
					min_2 = INT_MAX;
					for (int i = 0; i < SizeVertex; i++)                                                         /////////////////???????????????????????
					{
						if (passedVertex2[i] == false)
							if (Dist2[i] <= min_2)
							{
								min_2 = Dist2[i];
								index2 = i;
							}
					}
					passedVertex2[index2] = true;

					for (int i = 0; i < SizeVertex; i++)                                                          /////////////////???????????????????????
					{
						if (!passedVertex2[i])
							if(MatrixAdjacency[index2][i] != NO_EDGE)
							Dist2[i] = min(Dist2[i], Dist2[index2] + MatrixAdjacency[index2][i]);
					}

					flagWork = false;
					for (int i = 0; i < SizeVertex; i++)                                          /////////////////???????????????????????
					{
						if (passedVertex2[i] == false)
							flagWork = true;
					}
				}





				delete[] passedVertex2;
               
				unsigned int end_sequential_time = clock();

				/*cout << "sequential algorithm " << "\n";
				for (int i = 0; i < SizeVertex; i++)
				{
					cout << Dist2[i] << " ";
				}
				cout << endl;
*/
				int k = 0;
				for (int i = 0; i < SizeVertex; i++)
				{
					k+= Dist2[i] - Dist[i];
				}

				cout << "\n\nA1-A2 = " << k << endl;
				cout << "\ntime sequential = " << (end_sequential_time - start_sequential_time) << endl;

				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				delete[] Dist;
				delete[] Dist2;

				for (int i = 0; i < SizeVertex; i++)
				{
					delete[] MatrixAdjacency[i];
				}
				delete[] MatrixAdjacency;

			}
			//работа других процессов
			else
			{
				for (int i = 0; i < loadProcces[ProcRank]; i++)
				{
					//ArrResult[i] = INT_MAX;
					Mark[i] = false;
				}
				bool vertex = true;

				do
				{
					MPI_Bcast(&vertex, 1, MPI_BYTE, 0, Using);
					if (vertex == true)
					{
						local_min.value = INT_MAX;
						local_min.proc = INT_MAX;
						MPI_Scatterv(NULL, loadProcces, indexWork, MPI_INT, ArrResult, loadProcces[ProcRank], MPI_INT, 0, Using);
						for (int i = 0; i < loadProcces[ProcRank]; i++)                                                       /////////////////???????????????????????
						{
							if (Mark[i] == false)
								if (ArrResult[i] <= local_min.value)
								{
									local_min.value = ArrResult[i];
									local_min.proc = i + indexWork[ProcRank];
								}
						}
						MPI_Reduce(&local_min, NULL, 1, MPI_2INT, MPI_MINLOC, 0, Using);

						
						/////////////////////////////////////////////////////////////////
						//можно раздвть всем результат редуцирования стр 347             ,  в  результате 0 кинет всем строку Матрицы
						//MPI_Bcast(&index, 1, MPI_INT, 0, Using);
						//MPI_Scatterv(NULL, loadProcces, NULL, MPI_INT, ArrBuff, loadProcces[ProcRank], MPI_INT, 0, Using);

						//for (int i = 0; i < loadProcces[ProcRank]; i++)
						//{
			            //          if(ArrBuff[i] != NO_EDGE)
						//	ArrResult[i] = min(ArrResult[i], vertex + ArrBuff[i]);
						//}
						//MPI_Gatherv(ArrResult, loadProcces[ProcRank], MPI_INT, NULL, loadProcces, indexWork, MPI_INT, 0, Using);
						/////////////////////////////////////////////////////


						bool temp_flag = false;
						MPI_Scatterv(NULL, loadProcces, NULL, MPI_BYTE, Mark, loadProcces[ProcRank], MPI_BYTE, 0, Using);
						for (int i = 0; i < loadProcces[ProcRank]; i++)                                          /////////////////???????????????????????
						{
							if (Mark[i] == false)
								temp_flag = true;
						}
						MPI_Reduce(&temp_flag, NULL, 1, MPI_BYTE, MPI_BOR, 0, Using);
					}
				} while (vertex == true);
			}

			//общая часть


			delete[] loadProcces;
			delete[] indexWork;
			delete[] ArrResult;
			delete[] Mark;

		}
	MPI_Group_free(&UsingProcces);
	MPI_Comm_free(&Using);
	MPI_Finalize();


	return 0;
}
