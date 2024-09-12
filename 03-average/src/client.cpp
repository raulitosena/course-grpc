#include <grpcpp/grpcpp.h>
#include <proto/average.pb.h>
#include <proto/average.grpc.pb.h>


class AverageClient {
public:
	explicit AverageClient(std::shared_ptr<grpc::Channel> channel) : stub_(average::AverageService::NewStub(channel)) 
	{		
	}

	float ComputeAverage(std::vector<int> samples) 
	{
		// // Prepare the client context and the stream for the request
		// grpc::ClientContext context;
		// average::AvgTotal avg_total;
		

		// std::unique_ptr<grpc::ClientWriter<average::AvgTotal>> writer(stub_->ComputeAvg(&context, &avg_total));
		// for (auto &&sample : samples)
		// {
			
		// }

		// writer->WritesDone();
		// grpc::Status status = writer->Finish();
		

		/*

		// Create a client-side streaming call
		std::unique_ptr<grpc::ClientReader<average::AvgTotal>> reader(stub_->ComputeAvg(&context));

		// Send the integers to the server as a stream
		for (float value : samples) {
			average::AvgSample sample;
			sample.set_value(value);
			if (!reader->Write(sample)) {
				std::cerr << "Stream closed prematurely!" << std::endl;
				break;
			}
		}

		// Close the stream and get the final average
		reader->WritesDone();
		grpc::Status status = reader->Finish();

		if (status.ok()) {
			return avg_total.value();
		} else {
			std::cerr << "gRPC failed: " << status.error_message() << std::endl;
			return 0.0f;
		}
		// ::average::SumOperand request;		
		// request.set_op1(op1);
		// request.set_op2(op2);

		// ::average::SumResult response;
		// grpc::ClientContext context;
		// grpc::Status status = stub_->ComputeSum(&context, request, &response);
		*/

		// if (status.ok()) 
		// {
		// 	return avg_total.value();
		// }
		// else 
		// {
		// 	std::cerr << "RPC failed" << std::endl;
		// 	return 0.0;
		// }
		return 0.0f;
	}

private:
	std::unique_ptr<average::AverageService::Stub> stub_;
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
	AverageClient client(channel);

	std::vector<int> samples = {1,2,3,4};

	float average = client.ComputeAverage(samples);
	std::cout << "Average: " << average << std::endl;
	return 0;
}