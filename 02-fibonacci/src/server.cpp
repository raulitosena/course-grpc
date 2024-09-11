#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.grpc.pb.h>
#include <proto/fibonacci.pb.h>
#include <vector>


std::vector<uint32_t> GenerateFibonacci(uint32_t n) 
{
	std::vector<uint32_t> fibonacci_sequence;

	// Handle the case where n is 0
	if (n == 0) {
		return fibonacci_sequence;
	}

	// The first Fibonacci number is 0
	fibonacci_sequence.push_back(0);

	// Handle the case where n is 1
	if (n == 1) {
		return fibonacci_sequence;
	}

	// The second Fibonacci number is 1
	fibonacci_sequence.push_back(1);

	// Generate the remaining Fibonacci numbers
	for (uint32_t i = 2; i < n; ++i) {
		uint32_t next_value = fibonacci_sequence[i - 1] + fibonacci_sequence[i - 2];
		fibonacci_sequence.push_back(next_value);
	}

	return fibonacci_sequence;
}


class FibonacciServicesImpl : public fibonacci::FibonacciServices::Service
{
 	::grpc::Status GetFibonacciSequence(::grpc::ServerContext* context, const ::fibonacci::FibonacciRequest* request, ::fibonacci::FibonacciResponse* response)
 	{
		unsigned int number = request->number();

		//std::vector<uint32_t> fibonacci = GenerateFibonacci(number);
		std::vector<uint32_t> fibonacci = { 1, 2 , 3 };

		for (uint32_t i = 0; i < fibonacci.size(); ++i) 
		{
			response->set_number(fibonacci.at(i));
		}
		
		return grpc::Status::OK;
 	}
};

int main()
{
	printf("Fibonacci server running... \n");
	FibonacciServicesImpl service;
	grpc::ServerBuilder builder;
	builder.AddListeningPort("localhost:5000", grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	auto server(builder.BuildAndStart());
	server->Wait();

	for (size_t i = 0; i < 10; i++)
	{
		printf("ok\n");
		/* code */
	}
	

	return 0;
}