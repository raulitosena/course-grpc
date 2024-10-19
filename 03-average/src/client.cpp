#include <grpcpp/grpcpp.h>
#include <proto/average.pb.h>
#include <proto/average.grpc.pb.h>


class AverageClient {
public:
	explicit AverageClient(std::shared_ptr<grpc::Channel> channel)
		: stub(average::AverageService::NewStub(channel)) 
	{		
	}

	float ComputeAverage(std::vector<int> samples) 
	{
		grpc::ClientContext context;
		average::AvgSample request;
		average::AvgTotal response;

		std::unique_ptr<grpc::ClientWriter<average::AvgSample> > writer(this->stub->ComputeAvg(&context, &response));

		for (auto && sample : samples)
		{
			request.set_value(sample);
			if (!writer->Write(request))
				break;
		}

		writer->WritesDone();
		grpc::Status status = writer->Finish();

		if (status.ok())
		{
			return response.value();
		} 
		else 
		{
			std::cout << "gRPC error: " << status.error_message() << std::endl;
			return 0.0f;
		}
	}

private:
	std::unique_ptr<average::AverageService::Stub> stub;
};

int main(int argc, char** argv)
{
	// Ensure at least 1 argument (port) is provided, but no more than 11 (port + 10 numbers)
	if (argc < 2 || argc > 12)
	{
		std::cerr << "Usage: ./client <port> [numbers... (0 to 10)]" << std::endl;
		return 1001;
	}

	try
	{
		// Parse the port
		int port = std::stoi(argv[1]);
		std::string host = absl::StrFormat("localhost:%d", port);
		auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		AverageClient client(channel);

		// Parse numbers from command-line arguments (if any) into the samples vector
		std::vector<int> samples;
		for (int i = 2; i < argc; ++i)
		{
			int sample = std::stoi(argv[i]);
			samples.push_back(sample);
		}

		// If no numbers are provided, an empty vector will be passed
		float average = client.ComputeAverage(samples);
		std::cout << "Average: " << average << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
	}

	return 0;
}
