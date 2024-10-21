#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.grpc.pb.h>
#include <vector>
#include <chrono>
#include <string>


void printMetadata(const std::multimap<grpc::string_ref, grpc::string_ref> metadata)
{
	for (auto iter = metadata.begin(); iter != metadata.end(); ++iter)
	{
		std::cout << " - Header key: " << iter->first << ", value: ";
		size_t isbin = iter->first.find("-bin");
		if ((isbin != std::string::npos) && (isbin + 4 == iter->first.size()))
		{
			std::cout << std::hex;
			for (auto c : iter->second)
			{
				std::cout << static_cast<unsigned int>(c);
			}
			std::cout << std::dec;
		}
		else
		{
			std::cout << iter->second;
		}
		std::cout << std::endl;
	}
}

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
	FibonacciServerSlowReactor(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest& request, fibonacci::FibonacciListResponse* response) 
	{
		// Get the client's initial metadata
		std::cout << "Client user-defined metadata: " << std::endl;
		const std::multimap<grpc::string_ref, grpc::string_ref> metadata = context->client_metadata();

		printMetadata(metadata);

		auto request_id = metadata.find("request-id");

		// Process Fibonacci request
		uint64_t number = request.number();
		std::vector<unsigned long long> fibonacci_list;

		for (unsigned long long i = 0; i < number; ++i)
		{
			fibonacci_list.push_back(getFibonacci(i));
		}
		
		for (auto&& num : fibonacci_list)
		{
			response->add_number(num);
		}

		// Add server response metadata
		auto now = std::chrono::system_clock::now();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		context->AddInitialMetadata("response-timestamp", std::to_string(millis));
		context->AddInitialMetadata("server-id", "server-12345");
		context->AddInitialMetadata("request-id", std::string(request_id->second.data(), request_id->second.size()) );

		this->Finish(grpc::Status::OK);
	}

private:
	void OnDone() override 
	{
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
		this->host = absl::StrFormat("0.0.0.0:%d", port);
	}

	void Run()
	{
		grpc::ServerBuilder builder;
		// Metadata SOFT limit		
		// builder.AddChannelArgument(GRPC_ARG_MAX_METADATA_SIZE, 787);
		// Metadata HARD limit
		// builder.AddChannelArgument(GRPC_ARG_ABSOLUTE_MAX_METADATA_SIZE, 789);
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
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}
	
	return 0;
}
