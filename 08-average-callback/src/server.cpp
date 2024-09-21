#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/average.grpc.pb.h>
#include <proto/average.grpc.pb.h>
#include <vector>


class AverageReadReactor : public grpc::ServerReadReactor<average::AvgSample> 
{
public:
	AverageReadReactor(average::AvgTotal* response) : response(response)
	{
		this->StartRead(&this->request);
	}

	void OnReadDone(bool ok) override
	{
		if (ok)
		{
			// One element pull from stream
			this->counter++;
			this->accumulator += this->request.value();
			this->StartRead(&this->request);
		} 
		else 
		{	
			// End of stream
			float avg = 0.0f;
			if (this->counter != 0)
				avg = this->accumulator/this->counter;
			this->response->set_value(avg);
			Finish(grpc::Status::OK);
		}
	}

	void OnDone() override
	{
		std::cout << "RPC Completed" << std::endl;
		delete this;
	}

	void OnCancel() override
	{ 
		std::cerr << "RPC Canceled" << std::endl;
	}

private:
	average::AvgTotal* response;
	average::AvgSample request;
	int counter = 0;
	float accumulator = 0.0f;
};

class AverageServiceRpc : public average::AverageService::CallbackService
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

	grpc::ServerReadReactor<average::AvgSample>* ComputeAvg(grpc::CallbackServerContext* context, average::AvgTotal* response) override
	{
		return new AverageReadReactor(response);
	}


private:
	std::string host;
};

int main(int argc, char** argv)
{
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