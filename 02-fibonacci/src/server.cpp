#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.pb.h>
#include <proto/fibonacci.grpc.pb.h>
#include <vector>


std::vector<uint32_t> GenerateFibonacci(uint32_t limit) 
{
	std::vector<uint32_t> sequence;

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
	for (uint32_t i = 2; i < limit; ++i)
	{
		uint32_t next_value = sequence[i - 1] + sequence[i - 2];
		sequence.push_back(next_value);
	}

	return sequence;
}


class FibonacciServiceImpl : public fibonacci::FibonacciService::Service
{
	grpc::Status GetFibonacciSequence(grpc::ServerContext* context, const fibonacci::FibonacciRequest* request, grpc::ServerWriter<fibonacci::FibonacciResponse>* writer) override
	{
		unsigned int value = request->value();
		std::cout << "Received on server: " << value << std::endl;

		std::vector<uint32_t> fibonacci = GenerateFibonacci(value);
		for (uint32_t i = 0; i < fibonacci.size(); i++) 
		{
			fibonacci::FibonacciResponse response;
			response.set_value(fibonacci.at(i));
			writer->Write(response);
		}

		return grpc::Status::OK;
	}
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Missing parameters!" << std::endl;
		return 1001;
	}

	std::string host = argv[1];
	FibonacciServiceImpl service;
	grpc::ServerBuilder builder;
	builder.AddListeningPort(host, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	auto server(builder.BuildAndStart());
	printf("Fibonacci server running on %s ... \n", host.c_str());
	server->Wait();
	return 0;
}