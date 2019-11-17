#include <mpi.h>
#include <iostream>
#include <stdlib.h>
//#include <vector>
#include <stdio.h>
//#include <math.h>
//#include <cstring>
//#include <ctime>
//#include <algorithm>
//#include <stdexcept>
#include <random>

void test_add(void* a, void* b, int n);////
void test_add_double(void* a, void* b, int n);
void PROD_int(void* a, void* b, int n);
void PROD_double(void* a, void* b, int n);
void MAX_int(void* a, void* b, int n);
void MAX_double(void* a, void* b, int n);
void MIN_int(void* a, void* b, int n);
void MIN_double(void* a, void* b, int n);
void LAND_int(void* a, void* b, int n);
void LAND_double(void* a, void* b, int n);
void LOR_int(void* a, void* b, int n);
void LOR_double(void* a, void* b, int n);
void LXOR_int(void* a, void* b, int n);
void LXOR_double(void* a, void* b, int n);
void BOR_int(void* a, void* b, int n);
void BXOR_int(void* a, void* b, int n);
void BAND_int(void* a, void* b, int n);
void MAXLOC_double(void* a, void* b, int n, MPI_Comm comm);
void MINLOC_double(void* a, void* b, int n, MPI_Comm fcomm);

struct A {

	double value; int proc;

}local_max[3], global_max[3];



void Reduce(void* where_to_send_from, void* where_to_send_to, int how_much, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	MPI_Status status;

	int rank_came, num_of_proc;

	MPI_Comm_rank(comm, &rank_came);//� ����� ������ ��������

	MPI_Comm_size(comm, &num_of_proc);



	int rank = rank_came, num = num_of_proc;////////////////////////////////����� �������� ��� ����

	int ost = 0, type_size, step = 1;//�������,



	while (num > 1)//���������� ���������
	{

		if (rank < num)//���� �������, � ������� ������ < ���������� ���������(����� �������)
		{

			ost = num % 2;

			if (rank % 2 != 0)//���� ������� ��������
			{

				MPI_Send(where_to_send_from, how_much, datatype, (rank - 1) * step, 5, comm);

				rank *= num;

				break;

			}
			else
			{

				if (rank != (num - 1))//���� ������� ������ � �� ���������
				{

					MPI_Recv(where_to_send_to, how_much, datatype, (rank + 1) * step, 5, comm, &status);
					//op(where_to_send_to, where_to_send_from, how_much);
					if (datatype == MPI_INT)
					{
						if (op == MPI_SUM)
						{
							test_add(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_PROD)
						{
							PROD_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_MAX)
						{
							MAX_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_MIN)
						{
							MIN_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LAND)
						{
							LAND_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LOR)
						{
							LOR_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LXOR)
						{
							LXOR_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_BOR)
						{
							BOR_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_BXOR)
						{
							BXOR_int(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_BAND)
						{
							BAND_int(where_to_send_to, where_to_send_from, how_much);
						}
					}
					else if (datatype == MPI_DOUBLE)
					{
						if (op == MPI_SUM)
						{
							test_add_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_PROD)
						{
							PROD_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_MAX)
						{
							MAX_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_MIN)
						{
							MIN_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LAND)
						{
							LAND_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LOR)
						{
							LOR_double(where_to_send_to, where_to_send_from, how_much);
						}
						else if (op == MPI_LXOR)
						{
							LXOR_double(where_to_send_to, where_to_send_from, how_much);
						}
					}
					else if (datatype == MPI_DOUBLE_INT)
					{
						if (op == MPI_MAXLOC)
						{
							MAXLOC_double(where_to_send_to, where_to_send_from, how_much, comm);
						}
						else if (op == MPI_MINLOC)
						{
							MINLOC_double(where_to_send_to, where_to_send_from, how_much, comm);
						}
					}

					//op(where_to_send_to, where_to_send_from, how_much);

				}

				rank /= 2;/////////////////����� � 2 ���� ������ ����������

			}

			step *= 2;

		}

		num = num / 2 + ost;

	}

	if (rank_came == 0)
	{

		MPI_Type_size(datatype, &type_size);

		for (int i = 0; i < how_much; i++)
			std::cout << "POSMOTRI " << ((A*)(double*)where_to_send_from)[i].value << std::endl;
		if (datatype != MPI_DOUBLE_INT)
			memcpy(where_to_send_to, where_to_send_from, how_much * type_size);
		else
			memcpy(where_to_send_to, where_to_send_from, how_much * sizeof(struct A));

	}

}


int SumOfMatrixElementsPartly(std::vector<int> matrix) {

	int sum = 0;

	int size = matrix.size();

	for (int i = 0; i < size; i++)

		sum += matrix[i];

	return sum;

}



int MultiplicationOfMatrixElementsPartly(std::vector<int> matrix) {

	int sum = 0;

	int size = matrix.size();

	for (int i = 0; i < size; i++)

		sum *= matrix[i];

	return sum;

}

void test_add(void* a, void* b, int n)
{

	//*b += *a;


	for (int i = 0; i < n; i++)
		((int*)b)[i] += ((int*)a)[i];

}

void test_add_double(void* a, void* b, int n)
{

	//*b += *a;


	for (int i = 0; i < n; i++)
		((double*)b)[i] += ((double*)a)[i];

}

void PROD_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
		((int*)b)[i] *= ((int*)a)[i];

}

void PROD_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
		((double*)b)[i] *= ((double*)a)[i];

}

void MAX_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (((int*)b)[i] < ((int*)a)[i])
			((int*)b)[i] = ((int*)a)[i];
	}

}

void MAX_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (((double*)b)[i] < ((double*)a)[i])
			((double*)b)[i] = ((double*)a)[i];
	}

}

void MIN_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (((int*)b)[i] > ((int*)a)[i])
			((int*)b)[i] = ((int*)a)[i];
	}

}

void MIN_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (((double*)b)[i] > ((double*)a)[i])
			((double*)b)[i] = ((double*)a)[i];
	}

}

void LAND_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] && ((int*)a)[i];
	}

}

void LAND_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((double*)b)[i] = ((double*)b)[i] && ((double*)a)[i];
	}

}

void LOR_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] || ((int*)a)[i];
	}

}

void LOR_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((double*)b)[i] = ((double*)b)[i] || ((double*)a)[i];
	}

}

void LXOR_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] != ((int*)a)[i];
	}

}

void LXOR_double(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((double*)b)[i] = ((double*)b)[i] != ((double*)a)[i];
	}

}

void BOR_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] | ((int*)a)[i];
	}

}

void BXOR_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] ^ ((int*)a)[i];
	}

}

void BAND_int(void* a, void* b, int n)
{
	for (int i = 0; i < n; i++)
	{
		((int*)b)[i] = ((int*)b)[i] & ((int*)a)[i];
	}

}

void MAXLOC_double(void* a, void* b, int n, MPI_Comm fcomm)
{
	int rank;
	MPI_Comm_rank(fcomm, &rank);

	for (int i = 0; i < n; i++)
	{
		std::cout << rank << "BEFORE" << ((A*)(double*)b)[i].value << std::endl;
		std::cout << rank << "SRAVNI" << ((A*)(double*)a)[i].value << std::endl;
		if (((A*)(double*)b)[i].value < ((A*)(double*)a)[i].value)
		{
			((A*)(double*)b)[i].value = ((A*)(double*)a)[i].value;
			((A*)(int*)b)[i].proc = ((A*)(int*)a)[i].proc;
		}
		else if (((A*)(double*)b)[i].value == ((A*)(double*)a)[i].value)
			if (((A*)(int*)b)[i].proc > ((A*)(int*)a)[i].proc)
				((A*)(int*)b)[i].proc = ((A*)(int*)a)[i].proc;
		std::cout << "AFTER" << ((A*)(double*)b)[i].value << std::endl;
	}
	std::cout << std::endl << std::endl;
}

void MINLOC_double(void* a, void* b, int n, MPI_Comm fcomm)
{
	int rank;
	MPI_Comm_rank(fcomm, &rank);
	for (int i = 0; i < n; i++)
	{
		std::cout << rank << "PROCESSSSSSSS " << ((A*)(double*)b)[i].proc << std::endl;
		std::cout << rank << "VALUE_B " << ((A*)(double*)b)[i].value << std::endl;
		std::cout << rank << "VALUE_A " << ((A*)(double*)a)[i].value << std::endl;
		if (((A*)(double*)b)[i].value > ((A*)(double*)a)[i].value)
		{
			((A*)(double*)b)[i].value = ((A*)(double*)a)[i].value;
			((A*)(int*)b)[i].proc = ((A*)(int*)a)[i].proc;
		}
		else if (((A*)(double*)b)[i].value == ((A*)(double*)a)[i].value)
			if (((A*)(int*)b)[i].proc > ((A*)(int*)a)[i].proc)
				((A*)(int*)b)[i].proc = ((A*)(int*)a)[i].proc;
		std::cout << rank << "VALUE_B_AFTER " << ((A*)(double*)b)[i].value << std::endl;
		std::cout << i << " INDEX " << rank << "PROCESSSSSSSS" << ((A*)(double*)b)[i].proc << std::endl;

	}
	std::cout << std::endl << std::endl;
}


int main(int argc, char** argv) {
	int n = atoi(argv[1]);//������ �������
	int root = atoi(argv[2]);//�������, ������� ����� root
	//std::cout << root << std::endl << std::endl << std::endl;
	MPI_Init(&argc, &argv);
	int np, rank;//���������� ��������� � ����
	//int chislo = 1, chislo1 = 1;
	//int sum, sum1;
	//int n = 3;//������ �������
	double tic1, ticTree1, ticMPI1, tic2, ticTree2, ticMPI2;/////////////////////////
	int size = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//chislo *= rank;//���������� ������������� ����, ����� � ������ �������� ���� ������ �����
	//std::cout << "Chislo: " << chislo << " Rank: " << rank << std::endl;
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	//int root = 3;

	int* mas = new int[n];
	int* mas_sum = new int[n];
	double* mas_double = new double[n];
	double* mas_double_sum = new double[n];

	for (int i = 0; i < n; i++)
	{
		//mas[i] = i;
		//mas_double[i] = i + 0.1;
		//mas_double[i] = i + rank;
		if (rank == 0)
		{
			mas_double[0] = 1.1;
			mas_double[1] = 2.1;
			mas_double[2] = 2;
			for (int i = 0; i < n; i++)
			{
				local_max[i].value = mas_double[i];
				local_max[i].proc = rank;
			}
		}
		else if (rank == 1)
		{
			mas_double[0] = -1;
			mas_double[1] = 1.1;
			mas_double[2] = -3;
			for (int i = 0; i < n; i++)
			{
				local_max[i].value = mas_double[i];
				local_max[i].proc = rank;
			}
		}
		else if (rank == 2)
		{
			mas_double[0] = 1;
			mas_double[1] = -6;
			mas_double[2] = 3;
			for (int i = 0; i < n; i++)
			{
				local_max[i].value = mas_double[i];
				local_max[i].proc = rank;
			}
		}
		else if (rank == 3)
		{
			mas_double[0] = -1;
			mas_double[1] = 7;
			mas_double[2] = -32.1;
			for (int i = 0; i < n; i++)
			{
				local_max[i].value = mas_double[i];
				local_max[i].proc = rank;
			}
		}

		if (rank == 0)
		{
			mas[0] = 0;
			mas[1] = 1;
			mas[2] = 0;
		}
		else if (rank == 1)
		{
			mas[0] = 0;
			mas[1] = 1;
			mas[2] = 0;
		}
		else if (rank == 2)
		{
			mas[0] = 0;
			mas[1] = 1;
			mas[2] = 1;
		}
		else if (rank == 3)
		{
			mas[0] = 0;
			mas[1] = 1;
			mas[2] = 1;
		}
	}
	/////////////////////////////////////////////////////
	MPI_Group MPI_GROUP_WORLD;
	MPI_Group group;//����� ������
	MPI_Comm fcomm;//����� ������������

	int processov = np, q, proc;
	int* process_ranks;//������ ���������
	int rank_in_group;
	MPI_Status status;
	//MPI_Comm_size(MPI_COMM_WORLD, &size);//������
	//MPI_Comm_rank(MPI_COMM_WORLD, &rank);//����

	//printf("New group contains processes:");
	//q = size - 1;
	process_ranks = new int[np];
	//process_ranks = (int*)malloc(q * sizeof(int));
	for (proc = 0; proc < np; proc++)
	{
		if (proc == 0)
			process_ranks[proc] = root;
		else if (proc == root)
			process_ranks[proc] = 0;
		else
			process_ranks[proc] = proc;
		//printf("%i ", process_ranks[proc]);
	}
	if (rank == 0)
		for (int i = 0; i < np; i++)
		{
			std::cout << "process_ranks  " << process_ranks[i] << std::endl;
		}

	int ch = 0;
	if (rank == 0)
		ch = 4;
	if (rank == 2)
		ch = 1;
	printf("\n");
	MPI_Comm_group(MPI_COMM_WORLD, &MPI_GROUP_WORLD);
	MPI_Group_incl(MPI_GROUP_WORLD, np, process_ranks, &group);
	MPI_Comm_create(MPI_COMM_WORLD, group, &fcomm);

	//if (fcomm != MPI_COMM_NULL)
	//{
	int newprocs, newrank;
	MPI_Comm_size(fcomm, &newprocs);
	MPI_Comm_rank(fcomm, &newrank);

	/*if (newrank == 0)
		std::cout <<"DATAAAAAAAAAAAAAAAAAAAAAAAAA "<< chislo << std::endl;*/

	if (newrank == 0)
	{
		tic1 = MPI_Wtime();
	}
	//Reduce(&chislo, &sum, 1, MPI_INT, (func)test_add, 0, fcomm);
	//Reduce(mas, mas_sum, n, MPI_INT, (func)test_add, 0, fcomm);
	//Reduce(mas_double, mas_double_sum, n, MPI_DOUBLE, (func)test_add_double, 0, fcomm);
//Reduce(mas_double, mas_double_sum, n, MPI_DOUBLE, MPI_SUM, 0, fcomm);//��������
//Reduce(mas_double, mas_double_sum, n, MPI_DOUBLE, MPI_PROD, 0, fcomm);//��������
//Reduce(mas_double, mas_double_sum, n, MPI_DOUBLE, MPI_MAX, 0, fcomm);//��������
//Reduce(mas_double, mas_double_sum, n, MPI_DOUBLE, MPI_MIN, 0, fcomm);//��������
//Reduce(mas, mas_sum, n, MPI_INT, MPI_LAND, 0, fcomm);//��������
//Reduce(mas, mas_sum, n, MPI_INT, MPI_LOR, 0, fcomm);//��������
//Reduce(mas, mas_sum, n, MPI_INT, MPI_LXOR, 0, fcomm);//��������, ������ ����������
//Reduce(mas, mas_sum, n, MPI_INT, MPI_BOR, 0, fcomm);//��������, ������ ����������, ������ int
//Reduce(mas, mas_sum, n, MPI_INT, MPI_BXOR, 0, fcomm);//��������
//Reduce(mas, mas_sum, n, MPI_INT, MPI_BAND, 0, fcomm);//��������
	Reduce(local_max, global_max, n, MPI_DOUBLE_INT, MPI_MAXLOC, 0, fcomm);
	//Reduce(local_max, global_max, n, MPI_DOUBLE_INT, MPI_MINLOC, 0, fcomm);

		//Reduce(mas_double, mas_double_sum, n, MPI_DOUBLE, MPI_BAND, 0, fcomm);
		//Reduce(mas_double, mas_double_sum, n, MPI_DOUBLE, MPI_MAXLOC, 0, fcomm);
		//Reduce(&chislo, &sum, 1, MPI_INT, (func)test_multiplication, 0, MPI_COMM_WORLD);
	tic2 = MPI_Wtime();

	if (newrank == 0)
	{
		ticMPI1 = MPI_Wtime();
	}
	//MPI_Reduce(&chislo1, &sum1, 1, MPI_INT, MPI_SUM, 0, fcomm);
//MPI_Reduce(mas, mas_sum, n, MPI_INT, MPI_SUM, 0, fcomm);
//MPI_Reduce(mas_double, mas_double_sum, n, MPI_DOUBLE, MPI_PROD, 0, fcomm);
//MPI_Reduce(mas_double, mas_double_sum, n, MPI_DOUBLE, MPI_MAX, 0, fcomm);
//MPI_Reduce(mas_double, mas_double_sum, n, MPI_DOUBLE, MPI_MIN, 0, fcomm);
//MPI_Reduce(mas, mas_sum, n, MPI_INT, MPI_LAND, 0, fcomm);//������ ����������
//MPI_Reduce(mas, mas_sum, n, MPI_INT, MPI_LOR, 0, fcomm);//������ ����������
//MPI_Reduce(mas, mas_sum, n, MPI_INT, MPI_LXOR, 0, fcomm);//������ ����������
//MPI_Reduce(mas, mas_sum, n, MPI_INT, MPI_BOR, 0, fcomm);//������ ����������
//MPI_Reduce(mas, mas_sum, n, MPI_INT, MPI_BXOR, 0, fcomm);//������ ����������
//MPI_Reduce(mas, mas_sum, n, MPI_INT, MPI_BAND, 0, fcomm);//������ ����������
	//MPI_Reduce(local_max, global_max, n, MPI_DOUBLE_INT, MPI_MAXLOC, 0, fcomm);
	//MPI_Reduce(local_max, global_max, n, MPI_DOUBLE_INT, MPI_MINLOC, 0, fcomm);
	if (newrank == 0) {
		ticMPI2 = MPI_Wtime();
		std::cout << "reduce realisation MPI: t =  " << ticMPI2 - ticMPI1 << std::endl;
		std::cout << "reduce realisation my: t =  " << tic2 - tic1 << std::endl;
		if ((ticMPI2 - ticMPI1) > (tic2 - tic1))
			std::cout << "YES";
		std::cout << std::endl;
		for (int i = 0; i < 3; i++)
		{
			//std::cout << "SUM =  " << mas_double_sum[i];
			//std::cout << "SUM =  " << global_max.proc;
			std::cout << "SUM =  " << global_max[i].proc;

		}
		//std::cout << "SUM =  " << sum << std::endl;
		//std::cout << "SUM1 =  " << sum1 << std::endl;
	}

	MPI_Finalize();
	return 0;


}