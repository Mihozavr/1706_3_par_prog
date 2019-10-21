#include "mpi.h"
#include <iostream> 
#include "stdlib.h"
#include <random>

using namespace::std;

int procs; //����� ���������
int rankprocs; //����� �����
int sizes = 0; //������ ����� ��� ������� ��������
char* FillRand(int n)//���������� ������
{
	char* str;
	str = new char[n];
	char arrr[33];
	for (int i = 0, j = 97; i < 26; i++)
	{
		arrr[i] = (char)j;
		j++;
	}
	arrr[26] = ' ';
	arrr[27] = ' ';
	arrr[28] = ' ';
	arrr[29] = ' ';
	arrr[30] = ' ';
	arrr[31] = ' ';
	arrr[32] = ' ';
	for (int i = 0; i < n; i++)
		str[i] = arrr[(rand() % 33)];
	str[n] = '\0';
	return str;
}


int main(int argc, char* argv[])//���������� ��������� � ������ �� exe
{
	double t1 = 0.0, t2, prtime;

	char* str;
	str = new char[1000];
	char* LocalArr;
	int n = 1000;
	double N = 0;
	MPI_Status stat;
	MPI_Init(&argc, &argv);//������������� ����� ���������� MPI
	MPI_Comm_size(MPI_COMM_WORLD, &procs);//���������� ���������
	MPI_Comm_rank(MPI_COMM_WORLD, &rankprocs);// ���� ��������, ������� ������ ��� �������

	if (rankprocs == 0)
	{
		bool flag = false;

		while (flag == false)
		{
			cout << "Size str " << endl;
			cin >> n;
			if (n > 0)
				flag = true;
			else {
				cout << "Size str > 0" << endl;

			}
		}

		t1 = MPI_Wtime();
		str = FillRand(n);
		if (procs < 1)
		{
			cout << "procs < 1" << endl;
			return 0;
		}
		else
			cout << "Number procs : " << procs << endl;

		sizes = n / procs;
		if ((n % procs) != 0)
			sizes++;
		sizes += 2;
	}

	MPI_Bcast(&sizes, 1, MPI_INT, 0, MPI_COMM_WORLD); //�������� ������� �����

	LocalArr = new char[sizes];
	LocalArr[sizes - 1] = '\0';

	MPI_Barrier(MPI_COMM_WORLD);

	if (rankprocs == 0)
	{
		for (int i = 1; i < procs; i++)
		{

			MPI_Send(&str[((sizes - 2) * i)], sizes - 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);

		}
		for (int i = 0; i < sizes - 1; i++)
			LocalArr[i] = str[i];

	}
	else
	{
		MPI_Recv(LocalArr, sizes - 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);

	}

	double summ = 0;//���������� ����, ����������� � ��������
	cout << rankprocs << " proc start work" << endl;
	for (int i = 0; i < sizes - 2; i++)
	{
		if (LocalArr[i] == ' ')
			summ++;
		if (LocalArr[i] == ' ' && LocalArr[i + 1] == ' ')
			summ--;
	}

	double sums = 0;//����� ����, ����������� ����� ����������
	MPI_Reduce(&summ, &sums, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rankprocs == 0)
	{
		N = sums;

		N++;//���� ��� ��������
		if (str[0] == ' ')//�������� �� ������ �������
			N--;
		if (str[n - 1] == ' ')
			N--;
		t2 = MPI_Wtime();
		prtime = t2 - t1;
		for (int i = 0; i < n; i++)
			cout << str[i];
		cout << endl;
		cout << "Time= " << prtime << endl;
		cout << "Words: " << N << endl;

	}

	MPI_Finalize();
	return 0;
}




