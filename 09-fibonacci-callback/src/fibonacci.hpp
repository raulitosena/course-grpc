#include <vector>


std::vector<unsigned int> GenerateFibonacci(unsigned int limit) 
{
	std::vector<unsigned int> sequence;

	// Handle the case where n is 0
	if (limit == 0)
		return sequence;

	// The first Fibonacci number is 0
	sequence.push_back(0);

	// Handle the case where n is 1
	if (limit == 1)
		return sequence;

	// The second Fibonacci number is 1
	sequence.push_back(1);

	// Generate the remaining Fibonacci numbers
	for (unsigned int i = 2; i < limit; ++i)
	{
		unsigned int next_value = sequence[i - 1] + sequence[i - 2];
		sequence.push_back(next_value);
	}

	return sequence;
}

