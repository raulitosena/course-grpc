#include <grpcpp/grpcpp.h>
#include <proto/evens.pb.h>
#include <proto/evens.grpc.pb.h>


class EvensClient {
public:
	explicit EvensClient(std::shared_ptr<grpc::Channel> channel) : stub(evens::EvensService::NewStub(channel)) 
	{		
	}

	std::vector<int> GetEvens(std::vector<int> numbers) 
	{
 		grpc::ClientContext context;
		evens::Number request;
		evens::Number response;
		std::vector<int> evens_list;

		std::shared_ptr<grpc::ClientReaderWriter<evens::Number, evens::Number>> stream(this->stub->FindEvens(&context));

		for (auto &&num : numbers)
		{
			request.set_value(num);
			stream->Write(request);
		}

		stream->WritesDone();

		while (stream->Read(&response)) 
		{
			evens_list.push_back(response.value());
		}

		grpc::Status status = stream->Finish();

 		if (status.ok())
 		{
 			return evens_list;
 		} 
 		else 
 		{
 			std::cout << "gRPC error: " << status.error_message() << std::endl;
 			return { };
 		}
	}

private:
	std::unique_ptr<evens::EvensService::Stub> stub;
};

int main(int argc, char** argv) 
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./client <port>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		std::string host = absl::StrFormat("localhost:%d", port);
		std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		EvensClient client(channel);
		std::vector<int> numbers = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		std::vector<int> evens = client.GetEvens(numbers);
		std::cout << "Evens: ";
		for (auto &&val : evens)
		{
			std::cout << val << " " << std::flush;
		}

		std::cout << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}