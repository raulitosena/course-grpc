#include <grpcpp/grpcpp.h>
#include <proto/average.pb.h>
#include <proto/average.grpc.pb.h>


class AverageClient {
public:
	explicit AverageClient(std::shared_ptr<grpc::Channel> channel) : stub(average::AverageService::NewStub(channel)) 
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
	if (argc != 2)
	{
		std::cerr << "Missing parameters!" << std::endl;
		return 1001;
	}

	std::string host = argv[1];

	auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
	AverageClient client(channel);
	std::vector<int> samples = { 1, 2, 3, 4 };
	float average = client.ComputeAverage(samples);
	std::cout << "Average: " << average << std::endl;

	return 0;
}