#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>


class SumServiceImpl : public sum::SumService::CallbackService
{
public:
	grpc::ServerUnaryReactor* ComputeSum(grpc::CallbackServerContext* context, const sum::SumOperand* request, sum::SumResult* response) override 
	{
		response->set_result(request->op1() + request->op2());
		grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		std::cout << "Result: " << response->result() << std::endl;
		reactor->Finish(grpc::Status::OK);
		return reactor;
	}
};

int main(int argc, char** argv)
{
	std::cout << "..:: 07-sum-callback ::.." << std::endl;
	
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
