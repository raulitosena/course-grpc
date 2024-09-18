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

class SumServiceImpl : public sum::SumService::CallbackService
{
public:
	grpc::ServerUnaryReactor* ComputeSum(grpc::CallbackServerContext* context, const sum::SumOperand* request, sum::SumResult* response) override
	{
		return new SumReactor(*request, response);
	}
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
		std::string host = absl::StrFormat("localhost:%d", port);
		SumServiceImpl service;
		grpc::ServerBuilder builder;
		builder.AddListeningPort(host, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);
		std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
		if (server)
		{
			std::cout << "Server running on " << host << " ..." << std::endl;
			server->Wait();
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return 0;
}
