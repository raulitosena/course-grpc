#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/math.grpc.pb.h>
#include <proto/math.pb.h>



class MathServicesImpl : public math::MathServices::Service
{
	::grpc::Status computeSum(::grpc::ServerContext* context, const ::math::SumOperand* request, ::math::SumResult* response)
	{
		float result = request->op1() + request->op2();
		response->set_result(result);
		return grpc::Status::OK;
	}
};

int main()
{
	printf("SUM server running... \n");

	MathServicesImpl service;
	grpc::ServerBuilder builder;
	builder.AddListeningPort("0.0.0.0:5000", grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	auto server(builder.BuildAndStart());
	server->Wait();

	return 0;
}