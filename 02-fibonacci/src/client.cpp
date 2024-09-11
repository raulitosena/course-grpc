#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.grpc.pb.h>
#include <proto/fibonacci.pb.h>


class FibonacciClient {
public:
	FibonacciClient(std::shared_ptr<grpc::Channel> channel) : stub_(::fibonacci::FibonacciServices::NewStub(channel))
	{		
	}

	std::vector<unsigned int> GetFibonacciSequence(unsigned int num) 
	{
		//grpc::Status status;
		grpc::ClientContext context;

		// RPC request
	 	::fibonacci::FibonacciRequest request;		
	 	request.set_number(num);

		// RPC responses
		::fibonacci::FibonacciResponse response;
		std::unique_ptr< grpc::ClientReader<::fibonacci::FibonacciResponse> > reader(stub_->GetFibonacciSequence(&context, request));
		std::vector<unsigned int> fib_sequence;
			
		while (reader->Read(&response)) 
		{
			fib_sequence.push_back(response.number());
		}

		grpc::Status status = reader->Finish();

		if (!status.ok()) 
		{
			std::cerr << "RPC failed." << std::endl;
			return {};
		}

		return fib_sequence;
	}

private:
	std::unique_ptr<::fibonacci::FibonacciServices::Stub> stub_;
};


int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Invalid parameter!" << std::endl;
		return 1001;
	}

	auto host = (argv[1];
	auto limit = std::stoul(argv[2]);

	auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
	FibonacciClient client(channel);
	auto fib_sequence = client.GetFibonacciSequence(limit);

	std::cout << "From Fibonacci server: ";
	for (auto &&val : fib_sequence)
	{
		std::cout << val << " ";
	}

	std::cout << std::endl;

	return 0;
}