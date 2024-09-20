#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>


class SumServiceImpl : public sum::SumService::Service
{
	grpc::Status ComputeSum(grpc::ServerContext* context, const sum::SumOperand* request, sum::SumResult* response) override
	{
		float result = request->op1() + request->op2();
		response->set_result(result);
		return grpc::Status::OK;
	}
};

int main(int argc, char** argv)
{
	std::cout << "..:: 01-sum ::.." << std::endl;

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
		auto server(builder.BuildAndStart());
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