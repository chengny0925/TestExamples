#include <algorithm>
#include <ctime>
#include <iostream>

void test_benchmark()
{
	// Generate data
	const unsigned arraySize = 32768;
	int data[arraySize];

	for (unsigned c = 0; c < arraySize; ++c)
		data[c] = std::rand() % 256;

	// !!! With this, the next loop runs faster
	std::sort(data, data + arraySize);

	// Test
	clock_t start = clock();
	long long sum = 0;

	for (unsigned i = 0; i < 100000; ++i)
	{
		// Primary loop
		for (unsigned c = 0; c < arraySize; ++c)
		{
			if (data[c] >= 128)
				sum += data[c];
		}
	}

	double elapsedTime = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;

	std::cout << elapsedTime << std::endl;
	std::cout << "sum = " << sum << std::endl;
}

//优化后性能与排序后数据性能差不多
void test_optimization()
{
	// Generate data
	const unsigned arraySize = 32768;
	int data[arraySize];

	for (unsigned c = 0; c < arraySize; ++c)
		data[c] = std::rand() % 256;

	// !!! With this, the next loop runs faster
	//std::sort(data, data + arraySize);

	int lookup[256];

	for (int c = 0; c < 256; ++c)
	{
		lookup[c] = (c >= 128) ? c : 0;
	}

	// Test
	clock_t start = clock();
	long long sum = 0;

	for (unsigned i = 0; i < 100000; ++i)
	{
		// Primary loop
		for (unsigned c = 0; c < arraySize; ++c)
		{
			sum += lookup[data[c]];
		}
	}

	double elapsedTime = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;

	std::cout << elapsedTime << std::endl;
	std::cout << "sum = " << sum << std::endl;
}

int main()
{
	test_benchmark();
	test_optimization();

	return 0;
}