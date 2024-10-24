#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/math.grpc.pb.h>
#include <cmath>


class SqrtReactor : public grpc::ServerUnaryReactor 
{
public:
	SqrtReactor(const ::math::SqrtRequest& request, ::math::SqrtResponse* response) 
	{
		int number = request.number();

		if (number < 0)
		{
			this->Finish(grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Number cannot be negative", "Provide a number greater or equal to zero"));
			return;
		}

		double result = std::sqrt(number);
		response->set_result(result);
		this->Finish(grpc::Status::OK);
	}

private:
	void OnDone() override 
	{
		delete this;
	}

	void OnCancel() override 
	{
		std::cerr << "RPC Cancelled" << std::endl;
	}
};

class SqrtServiceImpl : public ::math::SqrtService::CallbackService
{
public:
	SqrtServiceImpl(unsigned short port)
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

	grpc::ServerUnaryReactor* Calculate(grpc::CallbackServerContext* context, const ::math::SqrtRequest* request, ::math::SqrtResponse* response) override
	{
		return new SqrtReactor(*request, response);
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
		SqrtServiceImpl service(port);
		service.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}
	
	return 0;
}
