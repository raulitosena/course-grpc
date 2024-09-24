#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.grpc.pb.h>
#include <vector>


unsigned long long getFibonacci(unsigned long long n)
{
	if (n <= 1)
	{
		return n;
	}
	
	return getFibonacci(n - 1) + getFibonacci(n - 2);
}

class FibonacciServerSlowReactor : public grpc::ServerUnaryReactor 
{
public:
	FibonacciServerSlowReactor(grpc::CallbackServerContext* context, const ::fibonacci::FibonacciRequest& request, ::fibonacci::FibonacciListResponse* response) 
	{
		uint64_t number = request.number();
		std::vector<unsigned long long> fibonacci_list;

		for (unsigned long long i = 0; i < number; ++i)
		{
			std::cerr << i << " " << std::flush;
			if (context->IsCancelled())
			{
				this->Finish(grpc::Status(grpc::StatusCode::CANCELLED, "Client cancelled, abandoning."));
				return;
			}

			fibonacci_list.push_back(getFibonacci(i));
		}
		
		for (auto &&num : fibonacci_list)
		{
			response->add_number(num);
		}

		this->Finish(grpc::Status::OK);
	}

private:
	void OnDone() override 
	{
		std::cerr << "RPC done!" << std::endl;
		delete this;
	}

	void OnCancel() override 
	{
		std::cerr << "RPC cancelled!" << std::endl;
	}
};

class FibonacciSlowServiceRpc : public ::fibonacci::FibonacciSlowService::CallbackService
{
public:
	FibonacciSlowServiceRpc(unsigned short port)
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

	grpc::ServerUnaryReactor* GetFibonacciList(grpc::CallbackServerContext* context, const ::fibonacci::FibonacciRequest* request, ::fibonacci::FibonacciListResponse* response) override
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
		FibonacciSlowServiceRpc service(port);
		service.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}
	
	return 0;
}
