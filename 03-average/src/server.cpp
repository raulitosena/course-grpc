#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/average.pb.h>
#include <proto/average.grpc.pb.h>


class AverageServiceImpl : public average::AverageService::Service
{
	grpc::Status ComputeAvg(grpc::ServerContext* context, grpc::ServerReader<average::AvgSample>* reader, average::AvgTotal* response) override
	{
		average::AvgSample sample;
		float sum = 0;
		int point_count = 0;

		std::cout << "Samples: ";

		while (reader->Read(&sample))
		{
			sum += sample.value();
			point_count++;
			std::cout << sample.value() << " " << std::flush;
		}

		std::cout << std::endl;

		float result = 0;		
		if (point_count > 0)
			result = sum/point_count;

		response->set_value(result);
		return grpc::Status::OK;
	}
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Missing parameters!" << std::endl;
		return 1001;
	}

	std::string host = argv[1];
	
	AverageServiceImpl service;
	grpc::ServerBuilder builder;
	builder.AddListeningPort(host, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	auto server(builder.BuildAndStart());
	std::cout << "Average server running on " << host << " ..." << std::endl;
	
	server->Wait();

	return 0;
}