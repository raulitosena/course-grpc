#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.pb.h>
#include <proto/fibonacci.grpc.pb.h>


class FibonacciClient {
public:
	explicit FibonacciClient(std::shared_ptr<grpc::Channel> channel) : stub_(fibonacci::FibonacciService::NewStub(channel))
	{		
	}

	std::vector<unsigned int> GetFibonacciSequence(unsigned int num) 
	{
		//grpc::Status status;
        grpc::ClientContext context;

		// RPC request
	 	fibonacci::FibonacciRequest request;		
	 	request.set_value(num);

		// RPC responses
		fibonacci::FibonacciResponse response;
		std::unique_ptr< grpc::ClientReader<fibonacci::FibonacciResponse> > reader(stub_->GetFibonacciSequence(&context, request));
		std::vector<unsigned int> sequence;
			
        while (reader->Read(&response))
		{
			sequence.push_back(response.value());
        }

        grpc::Status status = reader->Finish();

		if (!status.ok()) 
		{
			std::cerr << "RPC failed." << std::endl;
            return {};
        }

		return sequence;
    }

private:
	std::unique_ptr<fibonacci::FibonacciService::Stub> stub_;
};


int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Missing parameters!" << std::endl;
		return 1001;
	}

	std::string host = argv[1];
	uint32_t limit = std::stoul(argv[2]);

	auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
	FibonacciClient client(channel);
	auto sequence = client.GetFibonacciSequence(limit);

	std::cout << "From Fibonacci server: ";
	for (auto &&val : sequence)
	{
		std::cout << val << " ";
	}

	std::cout << std::endl;

	return 0;
}