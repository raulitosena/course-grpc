#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sum.grpc.pb.h>


class SumServiceImpl : public sum::SumService::CallbackService
{
public:
	::grpc::ServerUnaryReactor* ComputeSum(::grpc::CallbackServerContext* context, const sum::SumOperand* request, sum::SumResult* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(request->op1() + request->op2());
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}
};

class SumServer
{
public:
	SumServer(unsigned short port)
	{
		this->host = absl::StrFormat("localhost:%d", port);
	}

	void Run()
	{
		this->builder.AddListeningPort(this->host, ::grpc::InsecureServerCredentials());
		this->builder.RegisterService(&this->sum_service);
		std::shared_ptr<::grpc::Server> server = this->builder.BuildAndStart();

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

private:
	std::string host;
	::grpc::ServerBuilder builder;
	SumServiceImpl sum_service;
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
		SumServer server(port);
		server.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}
	
	return 0;
}