#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>


class SumClient {
public:
	SumClient(std::shared_ptr<grpc::Channel> channel) : stub_(::sum::SumServices::NewStub(channel)) 
	{		
	}

	float ComputeSum(float op1, float op2) 
	{
		::sum::SumOperand request;		
		request.set_op1(op1);
		request.set_op2(op2);

		::sum::SumResult response;
		grpc::ClientContext context;
		grpc::Status status = stub_->ComputeSum(&context, request, &response);

		if (status.ok()) 
		{
			return response.result();
		}
		else 
		{
			std::cerr << "RPC failed" << std::endl;
			return 0.0;
		}
	}

private:
	std::unique_ptr<::sum::SumServices::Stub> stub_;
};

int main(int argc, char** argv) 
{
	auto channel = grpc::CreateChannel("localhost:5000", grpc::InsecureChannelCredentials());
	SumClient client(channel);
	float op1 = 10.0;
	float op2 = 20.0;
	float result = client.ComputeSum(op1, op2);	
	std::cout << "The sum of " << op1 << " and " << op2 << " is " << result << std::endl;

	return 0;
}