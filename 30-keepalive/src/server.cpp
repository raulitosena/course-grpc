#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sum.grpc.pb.h>
#include <fstream>
#include <thread>


class SumServiceImpl : public sum::SumService::CallbackService
{
public:
	SumServiceImpl()
	{
	}

	grpc::ServerUnaryReactor* ComputeSum(grpc::CallbackServerContext* context, const sum::SumOperand* request, sum::SumResult* response) 
	{
		grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(request->op1() + request->op2());
		reactor->Finish(grpc::Status::OK);
		return reactor;
	}
};

class SumServer
{
public:
	SumServer(unsigned short port)
	{
		this->host = absl::StrFormat("0.0.0.0:%d", port);
	}

	virtual ~SumServer()
	{
		this->Stop();
	}

	void Start()
	{
		if (this->server)
			return;

		this->builder.AddListeningPort(this->host, grpc::InsecureServerCredentials());
		this->builder.RegisterService(&this->service);
		builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, 1 * 60 * 1000 /*1 min*/);
		builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 20 * 1000 /*20 sec*/);
		builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);
		this->server = this->builder.BuildAndStart();
		std::cout << "Server running on " << this->host << " ..." << std::endl;
	}

	void Stop()
	{
		if (!this->server)
			return;

		this->server->Shutdown();
	}

private:
	std::string host;
	grpc::ServerBuilder builder;
	SumServiceImpl service;
	std::unique_ptr<grpc::Server> server;
};

int main(int argc, char** argv)
{
	std::unique_ptr<SumServer> server;

	if (argc != 2)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		server = std::make_unique<SumServer>(port);
		server->Start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1002;
	}

	std::thread server_thread([&server]()
	{
		std::cout << "Commands: q=quit" << std::endl;
		std::string command;
		do
		{
			std::cout << "> " << std::flush;
			std::cin >> command;
		}
		while (command != "q");

		server->Stop();		
	});

	server_thread.join();
	
	return 0;
}