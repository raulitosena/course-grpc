#include <iostream>
#include <grpcpp/grpcpp.h>
#include "sqrt-callback.hpp"
#include "sqrt-reactor.hpp"


int main(int argc, char** argv) 
{
	if (argc < 2 || argc > 12)
	{
		std::cerr << "Usage: ./client <port> [numbers... (0 to 10)]" << std::endl;
		return 1001;
	}

	int port, number;
	std::shared_ptr<grpc::Channel> channel;
	std::unique_ptr<SqrtClientWithReactor> client_reactor;
	std::unique_ptr<SqrtClientWithCallback> client_callback;

	try
	{
		port = std::stoi(argv[1]);
		std::string host = absl::StrFormat("localhost:%d", port);
		channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		client_reactor = std::make_unique<SqrtClientWithReactor>(channel);
		client_callback = std::make_unique<SqrtClientWithCallback>(channel);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1002;
	}

	for (int i = 2; i < argc; ++i)
	{
		try
		{
			number = std::stoi(argv[i]);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			continue;
		}

		try
		{
			double result = client_reactor->Calculate(number);
			std::cout << "By reactor  - Sqrt: " << result << std::endl;
		}
		catch(const std::exception& e)
		{
			std::cerr << "By reactor  - Exception: " << e.what() << '\n';
		}

		try
		{
			double result = client_callback->Calculate(number);
			std::cout << "By callback - Sqrt: " << result << std::endl;
		}
		catch(const std::exception& e)
		{
			std::cerr << "By callback - Exception: " << e.what() << '\n';
		}
	}
	
	return 0;
}