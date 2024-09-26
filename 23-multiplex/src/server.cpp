#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sqrt.grpc.pb.h>
#include <cmath>


class DoubleReactor : public grpc::ServerUnaryReactor 
{
public:
	DoubleReactor(const ::squareroot::SqrtRequest& request, ::squareroot::SqrtResponse* response) 
	{
		int number = request.number();
		
		double result = 2 * number;
		response->set_result(result);
		this->Finish(grpc::Status::OK);
	}

private:
	void OnDone() override 
	{
		delete this;
	}

	void OnCancel() override 
	{
		std::cerr << "RPC Cancelled" << std::endl;
	}
};

class TripleReactor : public grpc::ServerUnaryReactor 
{
public:
	TripleReactor(const ::squareroot::SqrtRequest& request, ::squareroot::SqrtResponse* response) 
	{
		int number = request.number();
		
		double result = 3 * number;
		response->set_result(result);
		this->Finish(grpc::Status::OK);
	}

private:
	void OnDone() override 
	{
		delete this;
	}

	void OnCancel() override 
	{
		std::cerr << "RPC Cancelled" << std::endl;
	}
};

class QuadrupleReactor : public grpc::ServerUnaryReactor 
{
public:
	QuadrupleReactor(const ::squareroot::SqrtRequest& request, ::squareroot::SqrtResponse* response) 
	{
		int number = request.number();
		
		double result = 4 * number;
		response->set_result(result);
		this->Finish(grpc::Status::OK);
	}

private:
	void OnDone() override 
	{
		delete this;
	}

	void OnCancel() override 
	{
		std::cerr << "RPC Cancelled" << std::endl;
	}
};




class DoubleRpc : public ::squareroot::SqrtService::CallbackService
{
public:
	grpc::ServerUnaryReactor* Calculate(grpc::CallbackServerContext* context, const ::squareroot::SqrtRequest* request, ::squareroot::SqrtResponse* response) override
	{
		grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		response->set_result(2 * request->number());
		reactor->Finish(grpc::Status::OK);
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
		this->builder.AddListeningPort(this->host, grpc::InsecureServerCredentials());
		this->builder.RegisterService(&this->double_rpc);
		std::shared_ptr<grpc::Server> server = this->builder.BuildAndStart();

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
	grpc::ServerBuilder builder;
	DoubleRpc double_rpc;
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
