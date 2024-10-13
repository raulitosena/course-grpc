#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sum.grpc.pb.h>
#include <fstream>
#include "json.hpp"
#include <thread>


class SumServiceImpl : public sum::SumService::CallbackService
{
public:
	SumServiceImpl() : is_serving(true)
	{
		grpc::EnableDefaultHealthCheckService(true);
	}

	grpc::ServerUnaryReactor* ComputeSum(grpc::CallbackServerContext* context, const sum::SumOperand* request, sum::SumResult* response) 
	{
		grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(request->op1() + request->op2());
		reactor->Finish(grpc::Status::OK);
		return reactor;
	}

	void set_health_check_service(grpc::HealthCheckServiceInterface* health_check_service) 
	{
		this->health_check_service_ = health_check_service;
	}

	void SetHealth(bool active)
	{
		if (this->health_check_service_ == nullptr)
			return;
		if (this->is_serving == active)
			return;

		this->health_check_service_->SetServingStatus(active);
		this->is_serving = active;

		if (this->is_serving)
			std::cout << "service resumed." << std::endl;
		else
			std::cout << "service suspended." << std::endl;

	}

private:
	grpc::HealthCheckServiceInterface* health_check_service_ = nullptr;
	bool is_serving;
};

class SumServer
{
public:
	SumServer(unsigned short port)
	{
		this->host = absl::StrFormat("localhost:%d", port);
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
		this->server = this->builder.BuildAndStart();
		this->service.set_health_check_service(this->server->GetHealthCheckService());
		std::cout << "Server running on " << this->host << " ..." << std::endl;
	}

	void Stop()
	{
		if (!this->server)
			return;

		this->server->Shutdown();
	}

	void Suspend()
	{
		this->service.SetHealth(false);
	}

	void Resume()
	{
		this->service.SetHealth(true);
	}

private:
	std::string host;
	grpc::ServerBuilder builder;
	SumServiceImpl service;
	bool service_active;
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
		std::cout << "Commands: q=quit, s=suspend, r=resume" << std::endl;
		std::string command;
		do
		{
			std::cout << "> " << std::flush;
			std::cin >> command;

			if (command == "s")
				server->Suspend();
			else if (command == "r")
				server->Resume();

		} while (command != "q");

		server->Stop();		
	});

	server_thread.join();
	
	return 0;
}