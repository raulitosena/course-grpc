#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sqrt.grpc.pb.h>
#include <cmath>


class MathOffsetServiceImpl : public ::math::MathOffsetService::CallbackService
{
public:
	::grpc::ServerUnaryReactor* CalculateIncrement(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(request->number() + 1);
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}

	::grpc::ServerUnaryReactor* CalculateDecrement(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(request->number() - 1);
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}
	
};

class MathServiceImpl : public ::math::MathService::CallbackService
{
public:
	::grpc::ServerUnaryReactor* CalculateDouble(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(2 * request->number());
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}

	::grpc::ServerUnaryReactor* CalculateTriple(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(3 * request->number());
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}
};

class MathServer
{
public:
	MathServer(unsigned short port)
	{
		this->host = absl::StrFormat("localhost:%d", port);
	}

	void Run()
	{
		this->builder.AddListeningPort(this->host, ::grpc::InsecureServerCredentials());
		this->builder.RegisterService(&this->math_service);
		this->builder.RegisterService(&this->math_offset_service);
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
	MathServiceImpl math_service;
	MathOffsetServiceImpl math_offset_service;
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
		MathServer server(port);
		server.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}
	
	return 0;
}
