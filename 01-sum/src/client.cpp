#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>


class SumClient {
public:
	explicit SumClient(std::shared_ptr<grpc::Channel> channel) : stub(sum::SumService::NewStub(channel)) 
	{		
	}

	int ComputeSum(int op1, int op2) 
	{
		grpc::ClientContext context;
		sum::SumOperand request;		
		sum::SumResult response;

		request.set_op1(op1);
		request.set_op2(op2);		
		grpc::Status status = this->stub->ComputeSum(&context, request, &response);

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
	std::unique_ptr<sum::SumService::Stub> stub;
};

int main(int argc, char** argv) 
{
	if (argc != 4)
	{
		std::cerr << "Usage: ./client <port> <op1> <op2>" << std::endl;
		return 1001;
	}
	
	try
	{
		int port = std::stoi(argv[1]);
		int op1 = std::stoi(argv[2]);
		int op2 = std::stoi(argv[3]);
		std::string host = absl::StrFormat("localhost:%d", port);

		auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		SumClient client(channel);
		int result = client.ComputeSum(op1, op2);	
		std::cout << op1 << " + " << op2 << " = " << result << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}