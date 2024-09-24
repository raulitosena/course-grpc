#include <vector>


unsigned long long getFibonacci(unsigned long long n)
{
	if (n <= 1)
	{
		return n;
	}
	
	return getFibonacci(n - 1) + getFibonacci(n - 2);
}

std::vector<unsigned long long> getFibonacciSequence(unsigned long long limit)
{
	std::vector<unsigned long long> fibonacci_list;

	for (unsigned long long i = 0; i < limit; ++i)
	{
		fibonacci_list.push_back(getFibonacci(i));
	}

	return fibonacci_list;
}
