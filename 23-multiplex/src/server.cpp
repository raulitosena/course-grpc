#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/math.grpc.pb.h>



class MathServiceImpl : public ::math::MathService::CallbackService
{
public:
	::grpc::ServerUnaryReactor* Calculate2x(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(2 * request->number());
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}

	::grpc::ServerUnaryReactor* Calculate3x(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(3 * request->number());
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}

	::grpc::ServerUnaryReactor* Calculate4x(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(4 * request->number());
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}

	::grpc::ServerUnaryReactor* Calculate5x(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(5 * request->number());
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}

	::grpc::ServerUnaryReactor* Calculate6x(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(6 * request->number());
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}

	::grpc::ServerUnaryReactor* Calculate7x(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(7 * request->number());
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}

	::grpc::ServerUnaryReactor* Calculate8x(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(8 * request->number());
		reactor->Finish(::grpc::Status::OK);
		return reactor;
	}

	::grpc::ServerUnaryReactor* Calculate9x(::grpc::CallbackServerContext* context, const ::math::OperandRequest* request, ::math::ResultResponse* response) 
	{
		::grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(9 * request->number());
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
