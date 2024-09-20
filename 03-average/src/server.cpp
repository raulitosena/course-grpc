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
	std::cout << "..:: 03-average ::.." << std::endl;

	if (argc != 2)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1001;
	}
	
	try
	{
		int port = std::stoi(argv[1]);
		std::string host = absl::StrFormat("localhost:%d", port);
		AverageServiceImpl service;
		grpc::ServerBuilder builder;
		builder.AddListeningPort(host, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);
		auto server(builder.BuildAndStart());
		if (server)
		{
			std::cout << "Server running on " << host << " ..." << std::endl;
			server->Wait();
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}