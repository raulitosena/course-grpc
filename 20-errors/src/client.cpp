#include <iostream>
#include <grpcpp/grpcpp.h>
#include "sqrt-callback.hpp"
#include "sqrt-reactor.hpp"


int main(int argc, char** argv) 
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./client <port> <number>" << std::endl;
		return 1001;
	}

	int port, number;
	std::shared_ptr<grpc::Channel> channel;

	try
	{
		port = std::stoi(argv[1]);
		number = std::stoi(argv[2]);
		std::string host = absl::StrFormat("localhost:%d", port);
		channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1002;
	}

	try
	{
		SqrtClientWithReactor client1(channel);
		double result = client1.Calculate(number);	
		std::cout << "By reactor  - Sqrt: " << result << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "By reactor  - Exception: " << e.what() << '\n';
	}

	try
	{
		SqrtClientWithCallback client2(channel);
		double result = client2.Calculate(number);	
		std::cout << "By callback - Sqrt: " << result << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "By callback - Exception: " << e.what() << '\n';
	}

	return 0;
}