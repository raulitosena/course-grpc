#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/average.pb.h>
#include <proto/average.grpc.pb.h>


class AverageServiceRpc : public average::AverageService::Service
{
public:
	AverageServiceRpc(unsigned short port)
	{
		this->host = absl::StrFormat("localhost:%d", port);
	}

	void Run()
	{
		grpc::ServerBuilder builder;
		builder.AddListeningPort(this->host, grpc::InsecureServerCredentials());
		builder.RegisterService(this);
		std::shared_ptr<grpc::Server> server = builder.BuildAndStart();

		if (server)
		{
			std::cout << "Server running on " << this->host << " ..." << std::endl;
			server->Wait();
		}
		else
		{
			throw std::runtime_error("Failed to start server on " + this->host);
		}
	}

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

private:
	std::string host;
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
		AverageServiceRpc service(port);
		service.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}

	return 0;
}