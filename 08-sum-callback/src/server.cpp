#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>


class SumReactor : public grpc::ServerUnaryReactor 
{
public:
	SumReactor(const sum::SumOperand& request, sum::SumResult* response) 
	{
		response->set_result(request.op1() + request.op2());
		this->Finish(grpc::Status::OK);
	}

private:
	void OnDone() override 
	{
		std::cout << "RPC Completed" << std::endl;
		delete this;
	}

	void OnCancel() override 
	{
		std::cerr << "RPC Cancelled" << std::endl;
		delete this;
	}
};

class SumServiceRpc : public sum::SumService::CallbackService
{
public:
	SumServiceRpc(unsigned short port)
	{
		this->host = absl::StrFormat("localhost:%d", port);
	}

	void Run()
	{
		grpc::ServerBuilder builder;
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

	grpc::ServerUnaryReactor* ComputeSum(grpc::CallbackServerContext* context, const sum::SumOperand* request, sum::SumResult* response) override
	{
		return new SumReactor(*request, response);
	}
	
private:
	std::string host;
};

int main(int argc, char** argv)
{
	std::cout << "..:: 08-sum-callback-custom ::.." << std::endl;

	if (argc != 2)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		SumServiceRpc service(port);
		service.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}
	
	return 0;
}
