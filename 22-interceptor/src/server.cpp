#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.grpc.pb.h>
#include <vector>
#include "server_interceptor.hpp"


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

class FibonacciServerSlowReactor : public grpc::ServerUnaryReactor 
{
public:
	FibonacciServerSlowReactor(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest& request, fibonacci::FibonacciListResponse* response) 
	{
		uint64_t number = request.number();
		std::vector<unsigned long long> fibonacci_list = getFibonacciSequence(number);
		
		for (auto &&num : fibonacci_list)
		{
			response->add_number(num);
		}

		this->Finish(grpc::Status::OK);
	}

private:
	void OnDone() override 
	{
		std::cout << "RPC done!" << std::endl;
		delete this;
	}

	void OnCancel() override 
	{
		std::cerr << "RPC cancelled!" << std::endl;
	}
};

class FibonacciSlowServiceImpl : public fibonacci::FibonacciSlowService::CallbackService
{
public:
	FibonacciSlowServiceImpl(unsigned short port)
	{
		this->host = absl::StrFormat("localhost:%d", port);
	}

	void Run()
	{
		grpc::ServerBuilder builder;
		builder.AddListeningPort(this->host, grpc::InsecureServerCredentials());

		// Bind interceptor
        std::vector<std::unique_ptr<grpc::experimental::ServerInterceptorFactoryInterface>> interceptor_creators;
        interceptor_creators.push_back(std::make_unique<ServerLogInterceptorFactory>());
        builder.experimental().SetInterceptorCreators(std::move(interceptor_creators));

		// Register RCP server & start
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

	grpc::ServerUnaryReactor* GetFibonacciList(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest* request, fibonacci::FibonacciListResponse* response) override
	{
		return new FibonacciServerSlowReactor(context, *request, response);
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
		FibonacciSlowServiceImpl service(port);
		service.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1002;
	}
	
	return 0;
}
