// PP_lab_Dikstra.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include "dikstra.h"
#include <ctime>

#define SIZE 20000
#define SIZEFOR 10


int main(int argc, char **argv)
{
	//creature
	dataType** Graph;
	dataType* Result;
	dataType* ResultMP;
	dataType* ResultTBB;
	dataType* ResultStdThread;

	int min = 0;
	int max = 3;
	int SizeVertex = SIZE;

	srand(time(0));

	if (argc > 1)
		SizeVertex = atoi(argv[1]);
	if (argc > 2)
		max = atoi(argv[2]);
	if (argc > 3)
		min = atoi(argv[3]);

	////initialization

	//Graph[0][0] = 0;  Graph[0][1] = 7;  Graph[0][2] = 9;  Graph[0][3] = 0;  Graph[0][4] = 0; Graph[0][5] = 14;
	//Graph[1][0] = 7;  Graph[1][1] = 0;  Graph[1][2] = 10; Graph[1][3] = 15; Graph[1][4] = 0; Graph[1][5] = 0;
	//Graph[2][0] = 9;  Graph[2][1] = 10; Graph[2][2] = 0;  Graph[2][3] = 11; Graph[2][4] = 0; Graph[2][5] = 2;
	//Graph[3][0] = 0;  Graph[3][1] = 15; Graph[3][2] = 11; Graph[3][3] = 0;  Graph[3][4] = 6; Graph[3][5] = 0;
	//Graph[4][0] = 0;  Graph[4][1] = 0;  Graph[4][2] = 0;  Graph[4][3] = 6;  Graph[4][4] = 0; Graph[4][5] = 9;
	//Graph[5][0] = 14; Graph[5][1] = 0;  Graph[5][2] = 2;  Graph[5][3] = 0;  Graph[5][4] = 9; Graph[5][5] = 0;



	// 0 7 9 20 20 11
	double time_start, time_end, time_sequential = 0;
	double time_start_MP, time_end_MP, time_parallel_MP = 0;
	double time_start_TBB, time_end_TBB, time_parallel_TBB = 0;
	double time_start_Std_Thread, time_end_Std_Thread, time_parallel_Std_Thread = 0;

	//work

	for (int i = 0; i < SIZEFOR; i++) {

		Result = new dataType[SizeVertex];
		ResultMP = new dataType[SizeVertex];
		ResultTBB = new dataType[SizeVertex];
		ResultStdThread = new dataType[SizeVertex];

		Graph = new dataType*[SizeVertex];
		for (int i = 0; i < SizeVertex; i++)
		{
			Graph[i] = new dataType[SizeVertex];
		}


		for (int i = 0; i < SizeVertex; i++)
		{
			for (int j = i; j < SizeVertex; j++)
			{
				if (i != j)
					Graph[i][j] = Graph[j][i] = (rand() % (max - min + 1) + min);
				else Graph[i][j] = NO_EBGE;
			}
		}

		time_start = omp_get_wtime();
		dikstra(Graph, Result, SizeVertex);
		time_end = omp_get_wtime();

		//Graph = new dataType*[SizeVertex];
		//for (int i = 0; i < SizeVertex; i++)
		//{
		//	Graph[i] = new dataType[SizeVertex];
		//}


		//for (int i = 0; i < SizeVertex; i++)
		//{
		//	for (int j = i; j < SizeVertex; j++)
		//	{
		//		if (i != j)
		//			Graph[i][j] = Graph[j][i] = (rand() % (max - min + 1) + min);
		//		else Graph[i][j] = NO_EBGE;
		//	}
		//}

		time_start_MP = omp_get_wtime();
		dikstraMP(Graph, ResultMP, SizeVertex);
		time_end_MP = omp_get_wtime();

		/*for (int i = 0; i < SizeVertex; i++)
		{
			delete[] Graph[i];
		}
		delete[] Graph;

		Graph = new dataType*[SizeVertex];
		for (int i = 0; i < SizeVertex; i++)
		{
			Graph[i] = new dataType[SizeVertex];
		}
*/

		/*for (int i = 0; i < SizeVertex; i++)
		{
			for (int j = i; j < SizeVertex; j++)
			{
				if (i != j)
					Graph[i][j] = Graph[j][i] = (rand() % (max - min + 1) + min);
				else Graph[i][j] = NO_EBGE;
			}
		}*/

		time_start_TBB = omp_get_wtime();
		dikstraTBB(Graph, ResultTBB, SizeVertex);
		time_end_TBB = omp_get_wtime();

		/*for (int i = 0; i < SizeVertex; i++)
		{
			delete[] Graph[i];
		}
		delete[] Graph;
*/
		
		time_start_Std_Thread = omp_get_wtime();
		dikstraStdThread(Graph, ResultStdThread, SizeVertex);
		time_end_Std_Thread = omp_get_wtime();

		for (int i = 0; i < SizeVertex; i++)
		{
			delete[] Graph[i];
		}
		delete[] Graph;

		//cout << "Graph" << endl;

		//for (int i = 0; i < SizeVertex; i++)
		//{
		//	for (int j = 0; j < SizeVertex; j++)
		//	{
		//		cout << Graph[i][j] << " ";
		//	}
		//	cout << endl;
		//}
		//cout << endl;

		//cout << "sequential algorithm" << endl;
		//for (int i = 0; i < SizeVertex; i++)
		//{
		//	cout << Result[i] << endl;
		//}
		//cout << endl;

		//cout << "parallel MP algorithm" << endl;
		//for (int i = 0; i < SizeVertex; i++)
		//{
		//	cout << ResultMP[i] << endl;
		//}
		//cout << endl;


		time_sequential += time_end - time_start;
		
		time_parallel_MP += time_end_MP - time_start_MP;

		time_parallel_TBB += time_end_TBB - time_start_TBB;
		
		time_parallel_Std_Thread += time_end_Std_Thread - time_start_Std_Thread;
		

		cout << "time sequential algorithm " << time_end - time_start << " " << endl;
		cout << "time parallel MP algorithm " << time_end_MP - time_start_MP << " " << endl;
		cout << "time parallel TBB algorithm " << time_end_TBB - time_start_TBB << " " << endl;
		cout << "time parallel Std Thread algorithm " << time_end_Std_Thread - time_start_Std_Thread << " " << endl;


		dataType rez_sequential_and_MP = 0;
		for (int i = 0; i < SizeVertex; i++)
		{
			rez_sequential_and_MP += abs(Result[i] - ResultMP[i]);
		}

		cout << "control rezult sequential and MP " << rez_sequential_and_MP << endl;


		dataType rez_sequential_and_TBB = 0;
		for (int i = 0; i < SizeVertex; i++)
		{
			rez_sequential_and_TBB += abs(Result[i] - ResultTBB[i]);
		}

		cout << "control rezult sequential and TBB " << rez_sequential_and_TBB << endl;

		dataType rez_sequential_and_StdThread = 0;
		for (int i = 0; i < SizeVertex; i++)
		{
			rez_sequential_and_StdThread += abs(Result[i] - ResultStdThread[i]);
		}

		cout << "control rezult sequential and Std Thread " << rez_sequential_and_StdThread << endl;



		//dell
		delete[] Result;
		delete[] ResultMP;
		delete[] ResultTBB;
		delete[] ResultStdThread;


	}

	cout << "time sequential algorithm " << time_sequential / SIZEFOR << " " << endl;
	cout << "time parallel MP algorithm " << time_parallel_MP / SIZEFOR << " " << endl;
	cout << "time parallel TBB algorithm " << time_parallel_TBB / SIZEFOR << " " << endl;
	cout << "time parallel Std Thread algorithm " << time_parallel_Std_Thread / SIZEFOR << " " << endl;

}

