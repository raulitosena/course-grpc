#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "FibonacciServiceImpl.hpp"
#include "FibonacciSlowServiceImpl.hpp"
#include <iostream>


int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1001;
	}

	grpc::reflection::InitProtoReflectionServerBuilderPlugin();

	try
	{
		int port = std::stoi(argv[1]);
		std::string server_address = absl::StrFormat("0.0.0.0:%d", port);

		// Create instances of both services
		FibonacciServiceImpl fibonacciService;
		FibonacciSlowServiceImpl fibonacciSlowService;

		// Create and configure server builder
		grpc::ServerBuilder builder;
		builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

		// Register both services to the same server builder
		builder.RegisterService(&fibonacciService);
		builder.RegisterService(&fibonacciSlowService);

		// Start the server
		std::shared_ptr<grpc::Server> server = builder.BuildAndStart();

		if (server)
		{
			std::cout << "Server running on " << server_address << " ..." << std::endl;
			server->Wait();
		}
		else
		{
			throw std::runtime_error("Failed to start server on " + server_address);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}

	return 0;
}
