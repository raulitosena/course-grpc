#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/evens.pb.h>
#include <proto/evens.grpc.pb.h>


class EvensServiceRpc : public evens::EvensService::Service
{
public:
	EvensServiceRpc(unsigned short port)
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

	grpc::Status FindEvens(grpc::ServerContext* context, grpc::ServerReaderWriter<evens::Number, evens::Number>* stream) override
	{
		evens::Number request;
		evens::Number response;
		
		while (stream->Read(&request)) 
		{
			std::cout << request.value() << " " << std::flush;

			if (request.value() % 2 == 0)
			{
				response.set_value(request.value());
				stream->Write(response);
			}
    	}

		std::cout << std::endl;

		return grpc::Status::OK;
	}

private:
	std::string host;
};

int main(int argc, char** argv)
{
	std::cout << "..:: 04-evens ::.." << std::endl;

	if (argc != 2)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		EvensServiceRpc service(port);
		service.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}

	return 0;
}