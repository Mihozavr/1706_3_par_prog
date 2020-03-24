#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

typedef double type;

using namespace std;

class Matrix
{
protected:
	int row = 0;
	int col = 0;
public:
	vector<type> vec;
	Matrix()
	{
		vec = vector<double>();
	}
	Matrix(int col, int row)
	{
		this->row = row;
		this->col = col;
		vec = vector<type>(row * col);//�� ��������
	}
	Matrix(int col, int row, type val)
	{
		this->row = row;
		this->col = col;
		vec = vector<type>(row * col, val);
	}

	int getRow() const
	{
		return row;
	}
	int getCol() const
	{
		return col;
	}
	type* getP()
	{
		return &vec[0];
	}
	type* operator [] (int j)
	{
		return &vec[j * col];
	}
	Matrix operator * (Matrix& m)
	{
		if (getCol() != m.getRow()) return Matrix();
		Matrix newMatr(getRow(), m.getCol());
		for (int j = 0; j < m.getRow(); j++)
			for (int i = 0; i < getCol(); i++)
				for (int z = 0; z < getCol(); z++)
					newMatr[j][i] += (*this)[z][i] * m[j][z];
		return newMatr;
	}
	void transposition()
	{
		Matrix A(row, col);
		for (int i = 0; i < col; i++)
			for (int j = 0; j < row; j++)
			{
				A[i][j] = (*this)[j][i];
			}
		swap(row, col);
		vec = A.vec;
	}

	bool operator == (const Matrix& m)
	{
		bool res = ((row == m.row) & (col == m.col));
		if (res == false) return false;
		for (int i = 0; i < vec.size(); i++)
			if (vec[i] != m.vec[i]) return false;
		return true;
	}

	static void readMatrix(Matrix& A, Matrix& B, int N)
	{
		ifstream input("source.txt");
		for (int i = 0; i < A.vec.size() + B.vec.size(); i++)
		{
			if (i < A.vec.size())
				input >> A.vec[i];
			else
			{
				for (int j = 0; j < B.vec.size(); j++)
					input >> B.vec[j];
			}
		}
		input.close();
		A.transposition();
		B.transposition();
	}


	static void writeMatrix(Matrix& A, int N)
	{
		A.transposition();
		ofstream input("result.txt");
		for (int i = 0; i < A.vec.size(); i++)
		{
			if (i < A.vec.size())
				input << A.vec[i] << "\t ";
			if ((i + 1) % N == 0)
				input << endl;
		}
		input.close();
	}

};

class CCSMatrix
{
protected:
	vector<type> values;
	vector<int> rows;
	vector<int> pointer;
	int N;
public:

	CCSMatrix(int n) :N(n) {}
	CCSMatrix(Matrix& M) :N(M.getRow())
	{
		pointer = vector<int>(N + 1);
		int lastCount = 0;
		for (int j = 0; j < N; j++)
		{
			int curCount = 0;
			for (int i = 0; i < N; i++)
			{
				type el = M[j][i];//����� ������� �� ����������� �������
				if (el != 0.0)
				{
					curCount++;
					rows.push_back(i);//��������� � ����� ������
					values.push_back(el);
				}
			}
			if (j != 0)
			{
				pointer[j] = pointer[j - 1] + lastCount;//������ � values, ������ � �������
			}
			lastCount = curCount;
		}
		pointer[N] = (int)values.size();
	}

	void transposition()
	{
		vector<vector<pair<type, int>>> tmp(N, vector<pair<type, int>>());
		int pCount = 0;//����� �������
		int numElementsInCol = 0;//������� ��������� ��������� � �������
		for (int i = 0; i < values.size(); i += numElementsInCol)
		{
			numElementsInCol = pointer[pCount + 1] - pointer[pCount];//���������� ���������� ��������� � �������
			for (int z = i; z < i + numElementsInCol; z++)//���� ��� ��������� ������ �������
			{
				int row = rows[z];//� ����� ������ ���������
				int col = pCount;
				type el = values[z];
				tmp[row].push_back(make_pair(el, col));
			}
			pCount++;
		}
		pCount = 0;//��� pointer
		vector<int>* cols = &rows;
		int lastCount = 0;
		int vCount = 0;
		for (int i = 0; i < N; i++)
		{
			int numElementInCol = (int)tmp[i].size();//������ ������ � �������
			if (numElementInCol > 0)//���� � ������ ������ ���� ��������
			{
				for (int j = 0; j < numElementInCol; j++)//���������� �������� ��� CCS
				{
					values[vCount] = tmp[i][j].first;//����� �������
					(*cols)[vCount] = tmp[i][j].second;
					vCount++;
				}
			}
			if (pCount != 0)
			{
				pointer[pCount] = pointer[pCount - 1] + lastCount;
			}
			lastCount = numElementInCol;
			pCount++;

		}
	}
	CCSMatrix operator * (const CCSMatrix& m)
	{
		CCSMatrix res(N);//�������������� �������
		res.pointer.push_back(0);
		transposition();
		vector<int>* cols = &rows;
		int elCountM = 0;
		for (int j = 0; j < m.N; j++)
		{
			int numElInResCol = 0;
			const int numElementInCol = m.pointer[j + 1] - m.pointer[j];//����������� ���������� ��������� � �������
			if (numElementInCol == 0)
			{
				int size = res.pointer.size();//������ pointer � �������������� �������
				res.pointer.push_back(res.pointer[size - 1]);//��������� ����������(���� � ������ ������� ����)
				continue;
			}
			int elCountThis = 0;
			for (int i = 0; i < N; i++)
			{
				const int numElementInRow = pointer[i + 1] - pointer[i];//������� ��������� � ������
				if (numElementInRow == 0)//���� ���� ���������
				{
					continue;
				}
				int tmpNumElCol = numElementInCol;//���������� ��������� � ������� ������� m
				int tmpNumElRow = numElementInRow;//���������� ��������� � ������ ������� this

				type sum = 0;
				int tmpElCountM = elCountM;
				for (int z = 0; z < min(tmpNumElCol, tmpNumElRow);)//�������� min, �.�. ��������������� �������� ����������, � ������ ����� ����
				{
					int colThis = (*cols)[elCountThis];//�������� ������� ����������
					int rowM = m.rows[tmpElCountM];//�������� ������� ���������� � M
					if (colThis == rowM)//������������ �� �����������
					{
						sum += values[elCountThis] * m.values[tmpElCountM];
						tmpNumElCol--;
						tmpNumElRow--;
						tmpElCountM++;
						elCountThis++;
					}
					else if (colThis < rowM)//this
					{
						tmpNumElRow--;//���������� ���������� ���������
						elCountThis++;//�������
					}
					else//��� m
					{
						tmpNumElCol--;
						tmpElCountM++;
					}
				}
				for (int z = 0; z < tmpNumElRow; z++)
					elCountThis++;

				if (sum != 0)
				{
					res.values.push_back(sum);
					res.rows.push_back(i);
					numElInResCol++;//������� ��������� � �������
				}
			}
			const int size = res.pointer.size();
			res.pointer.push_back(res.pointer[size - 1] + numElInResCol);
			elCountM += numElementInCol;
		}
		transposition();
		return res;
	}

	Matrix CCStoMatrix()
	{
		Matrix res(N, N);

		int Elem = 0;
		int count = 0;
		int resCount = 0;

		for (int i = 0; i < N; i++)
		{
			int numOfElem = pointer[i + 1] - pointer[i];

			if (numOfElem != 0)
				for (int j = 0; j < N; j++)
				{
					if (numOfElem != 0 && rows[resCount] == j)
					{
						res.vec[count] = values[Elem];
						Elem++;
						resCount++;
						numOfElem--;
					}
					else
						res.vec[count] = 0;
					count++;
				}
			else
				for (int j = 0; j < N; j++)
				{
					res.vec[count] = 0;
					count++;
				}
		}

		return res;
	}

};



int main()
{
	Matrix a(4, 4), b(4, 4), c(4, 4);

	Matrix::readMatrix(a, b, 4);

	CCSMatrix a1(a), b1(b), c1(c);
	c1 = a1 * b1;
	c = c1.CCStoMatrix();

	Matrix::writeMatrix(c, 4);

	return 0;
}