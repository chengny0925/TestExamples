#include <thread>
#include <atomic>
#include <math.h>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#define FILE_FULL_PATH "input.txt"

int** g_arr;
std::mutex g_lock;
condition_variable g_cv;
atomic_int g_cal_count = 0;
int g_max_phase;

bool splitIntString(char spCharacter, const string& objString, vector<int>& intVector);
bool splitString(char spCharacter, const string& objString, vector<string>& stringVector);

int initIntVals(const string& filePath, int& elementNum)
{
	ifstream f(filePath);
	if (!f)
	{
		cout << "file donot existed" << endl;
		return -1;
	}
	string line;
	if (!getline(f, line))
	{
		cout << "read file error " << endl;
		f.close();
		return -1;
	}
	f.close();

	vector<int> vals;
	//int arr[4*4] = { 3,11,6,16,8,1,5,10,14,7,12,2,4,13,9,15 };
	splitIntString(',', line, vals);
	int num = vals.size();
	int size = sqrt(num);
	if (num != size * size)
	{
		cout << "input elements number error num=" << num << endl;
		return -1;
	}

	elementNum = num;
	g_arr = new int*[size];
	for (int i = 0; i < size; i++)
		g_arr[i] = new int[size];

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			g_arr[i][j] = vals[i * size + j];
		}
	}

	return 0;
}

void print(int** arr, int n)
{
	for (int row = 0; row < n; row++)
	{
		for (int col = 0; col < n; col++)
		{
			printf("%3d ", arr[row][col]);
			//std::cout << arr[row * n + col] << " ";
		}
		std::cout << endl;
	}
}



void thread_func(int id, int** arr, int size, condition_variable* pcv)
{
	int phase = 1;
	while (phase <= g_max_phase)
	{
		if (phase % 2 == 1)
		{
			//sort row at id, bubble sort
			for (int i = size - 1; i > 0; i--)
			{
				for (int col = 0; col < i; col++)
				{
					if (id % 2 == 0)
					{
						if (arr[id][col] > arr[id][col + 1])
						{
							swap(arr[id][col], arr[id][col + 1]);
						}
					}
					else
					{
						if (arr[id][col] < arr[id][col + 1])
						{
							swap(arr[id][col], arr[id][col + 1]);
						}
					}

				}
			}

		}
		else
		{
			//sort col at id
			for (int i = size - 1; i > 0; i--)
			{
				for (int row = 0; row < i; row++)
				{
					if (arr[row][id] > arr[row+1][id])
					{
						swap(arr[row][id], arr[row + 1][id]);
					}
				}
			}
		}

		g_cal_count++;

		//send signal
		pcv->notify_one();
		{
			std::unique_lock<std::mutex> lock{ g_lock };
			//wait signal
			g_cv.wait(lock);
		}
		phase++;
	}
}

int main()
{
	int elementNum;
	int ret = initIntVals(FILE_FULL_PATH, elementNum);
	if (ret != 0)
	{
		return ret;
	}


	int size = sqrt(elementNum);
	g_max_phase = log(elementNum) / log(2) + 1;


	cout << "initial" << endl;
	print(g_arr, size);
	cout << endl;

	thread ** pthread_arr = new thread*[size];
	condition_variable cv;
	for (int i = 0; i < size; i++)
	{
		pthread_arr[i] = new thread(thread_func, i, g_arr, size, &cv);
	}


	for (int i = 0; i < g_max_phase; i++)
	{
		{
			std::unique_lock<std::mutex> lock{ g_lock };
			//wait signal
			while (g_cal_count < (i + 1) * size)
			{
				cv.wait(lock);
			}

		}

		cout << "after phase " << i+ 1 << endl;
		print(g_arr, size);
		cout << endl;

		//send signal;
		g_cv.notify_all();
	}

	for (int i = 0; i < size; i++)
	{
		pthread_arr[i]->join();
		delete pthread_arr[i];
	}

	delete[] pthread_arr;

	for (int i = 0; i < size; i++)
	{
		delete[] g_arr[i];
	}
	delete[] g_arr;

	return 0;
}

bool splitIntString(char spCharacter, const string& objString, vector<int>& intVector)
{
	vector<string> strVector;
	splitString(spCharacter, objString, strVector);
	vector<string>::iterator iter = strVector.begin();
	while (iter != strVector.end())
	{
		intVector.push_back(atoi(iter->c_str()));
		iter++;
	}
	return true;
}

bool splitString(char spCharacter, const string& objString, vector<string>& stringVector)
{
	if (objString.length() == 0)
	{
		return true;
	}

	size_t posBegin = 0;
	size_t posEnd = 0;
	bool lastObjStore = true;

	while (posEnd != string::npos)
	{
		posBegin = posEnd;
		posEnd = objString.find(spCharacter, posBegin);

		if (posBegin == posEnd)
		{
			posEnd += 1;
			continue;
		}

		if (posEnd == string::npos)
		{
			stringVector.push_back(objString.substr(posBegin, objString.length() - posBegin));
			break;
		}

		stringVector.push_back(objString.substr(posBegin, posEnd - posBegin));
		posEnd += 1;
	}
	return true;
}