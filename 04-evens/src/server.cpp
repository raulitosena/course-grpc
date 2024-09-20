#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/evens.pb.h>
#include <proto/evens.grpc.pb.h>


class EvensServiceImpl : public evens::EvensService::Service
{
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
		std::string host = absl::StrFormat("localhost:%d", port);
		EvensServiceImpl service;
		grpc::ServerBuilder builder;
		builder.AddListeningPort(host, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);
		std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
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