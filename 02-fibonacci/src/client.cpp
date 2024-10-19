#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.pb.h>
#include <proto/fibonacci.grpc.pb.h>


class FibonacciClient {
public:
	explicit FibonacciClient(std::shared_ptr<grpc::Channel> channel)
		: stub(fibonacci::FibonacciService::NewStub(channel))
	{		
	}

	std::vector<unsigned int> GetFibonacciSequence(unsigned int num) 
	{
        grpc::ClientContext context;
	 	fibonacci::FibonacciRequest request;
		fibonacci::FibonacciResponse response;

	 	request.set_number(num);
		std::unique_ptr< grpc::ClientReader<fibonacci::FibonacciResponse> > reader(this->stub->GetFibonaccisStream(&context, request));
		std::vector<unsigned int> sequence;
			
        while (reader->Read(&response))
		{
			sequence.push_back(response.number());
        }

        grpc::Status status = reader->Finish();

		if (!status.ok()) 
		{
			std::cerr << "RPC failed: " << status.error_message() << std::endl;
            return {};
        }

		return sequence;
    }

private:
	std::unique_ptr<fibonacci::FibonacciService::Stub> stub;
};

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./client <port> <limit>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		uint32_t limit = std::stoul(argv[2]);
		std::string host = absl::StrFormat("localhost:%d", port);

		auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		FibonacciClient client(channel);
		auto sequence = client.GetFibonacciSequence(limit);

		std::cout << "From Fibonacci server: ";
		for (auto &&val : sequence)
		{
			std::cout << val << " ";
		}

		std::cout << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return 0;
}