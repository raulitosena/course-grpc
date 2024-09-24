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

class FibonacciServiceRpc : public fibonacci::FibonacciService::Service
{
public:
	FibonacciServiceRpc(unsigned short port)
	{
		this->host = absl::StrFormat("localhost:%d", port);
	}

	void Run()
	{
		grpc::ServerBuilder builder;
		builder.AddListeningPort(this->host, grpc::InsecureServerCredentials());
		builder.RegisterService(this);
		std::shared_ptr<grpc::Server> server = builder.BuildAndStart();

		if (server)
		{
			std::cout << "Server running on " << this->host << " ..." << std::endl;
			server->Wait();
		}
		else
		{
			throw std::runtime_error("Failed to start server on " + this->host);
		}
	}

	grpc::Status GetFibonaccisStream(grpc::ServerContext* context, const fibonacci::FibonacciRequest* request, grpc::ServerWriter<fibonacci::FibonacciResponse>* writer) override
	{
		unsigned int value = request->number();
		std::cout << "Received on server: " << value << std::endl;

		std::vector<uint32_t> fibonacci = GenerateFibonacci(value);
		for (uint32_t i = 0; i < fibonacci.size(); i++) 
		{
			fibonacci::FibonacciResponse response;
			response.set_number(fibonacci.at(i));
			writer->Write(response);
		}

		return grpc::Status::OK;
	}

private:
	std::string host;
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		FibonacciServiceRpc service(port);
		service.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}

	return 0;
}