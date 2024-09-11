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
	::grpc::Status GetFibonacciSequence(::grpc::ServerContext* context, const ::fibonacci::FibonacciRequest* request, grpc::ServerWriter<fibonacci::FibonacciResponse>* writer)
	{
		unsigned int number = request->number();
		std::cout << "Received on server: " << number << std::endl;

		std::vector<uint32_t> fibonacci = GenerateFibonacci(number);
		for (uint32_t i = 0; i < fibonacci.size(); ++i) 
		{
			::fibonacci::FibonacciResponse response;
			response.set_number(fibonacci.at(i));
			writer->Write(response);
		}

		return grpc::Status::OK;
	}
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Invalid parameter!" << std::endl;
		return 1001;
	}

	printf("Fibonacci server running on %s ... \n", argv[1]);
	FibonacciServicesImpl service;
	grpc::ServerBuilder builder;
	builder.AddListeningPort(argv[1], grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	auto server(builder.BuildAndStart());
	server->Wait();
	return 0;
}