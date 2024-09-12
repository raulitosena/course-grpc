#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>



class SumServicesImpl : public sum::SumServices::Service
{
	::grpc::Status ComputeSum(::grpc::ServerContext* context, const ::sum::SumOperand* request, ::sum::SumResult* response) override
	{
		float result = request->op1() + request->op2();
		response->set_result(result);
		return grpc::Status::OK;
	}
};

int main()
{
	printf("SUM server running... \n");
	SumServicesImpl service;
	grpc::ServerBuilder builder;
	builder.AddListeningPort("localhost:5000", grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	auto server(builder.BuildAndStart());
	server->Wait();

	return 0;
}