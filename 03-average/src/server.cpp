#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/average.grpc.pb.h>
#include <proto/average.pb.h>



// class MathServicesImpl : public average::MathServices::Service
// {
// 	::grpc::Status ComputeSum(::grpc::ServerContext* context, const ::average::SumOperand* request, ::average::SumResult* response)
// 	{
// 		float result = request->op1() + request->op2();
// 		response->set_result(result);
// 		return grpc::Status::OK;
// 	}
// };

int main()
{
	// printf("SUM server running... \n");
	// MathServicesImpl service;
	// grpc::ServerBuilder builder;
	// builder.AddListeningPort("localhost:5000", grpc::InsecureServerCredentials());
	// builder.RegisterService(&service);
	// auto server(builder.BuildAndStart());
	// server->Wait();

	return 0;
}