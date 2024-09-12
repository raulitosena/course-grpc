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
	if (argc != 2)
	{
		std::cerr << "Missing parameters!" << std::endl;
		return 1001;
	}

	std::string host = argv[1];

	SumServiceImpl service;
	grpc::ServerBuilder builder;
	builder.AddListeningPort(host, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	auto server(builder.BuildAndStart());
	std::cout << "Sum server running on " << host << " ..." << std::endl;

	server->Wait();

	return 0;
}