#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>
#include <thread>


class SumClient
{
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

		// The producer-consumer queue we use to communicate asynchronously with the gRPC runtime.
		grpc::CompletionQueue cq;
		// Storage for the status of the RPC upon completion.
   		grpc::Status status;

    	std::unique_ptr<grpc::ClientAsyncResponseReader<sum::SumResult>> rpc(this->stub->AsyncComputeSum(&context, request, &cq));

		rpc->Finish(&response, &status, (void*)1);

		void* got_tag;
		bool ok = false;

		while (cq.Next(&got_tag, &ok))
		{	
			if (got_tag == (void*)1) 
			{
				if (!ok)
					break;
				
				// check reply and status
				if (status.ok()) 
				{
					return response.result();
				}
				else 
				{
					std::cerr << "RPC failed: " << status.error_message() << std::endl;
					return 0;
				}
			}

			std::cout << "." << std::flush;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		return 0;
	}

private:
	std::unique_ptr<sum::SumService::Stub> stub;
};

int main(int argc, char** argv) 
{
	if (argc != 4)
	{
		std::cerr << "Missing parameters!" << std::endl;
		return 1001;
	}

	std::string host = argv[1];
	int op1 = std::stoi(argv[2]);
	int op2 = std::stoi(argv[3]);

	auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
	SumClient client(channel);
	int result = client.ComputeSum(op1, op2);	
	std::cout << op1 << " + " << op2 << " = " << result << std::endl;

	return 0;
}