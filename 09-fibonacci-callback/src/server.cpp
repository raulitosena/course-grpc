#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.pb.h>
#include <proto/fibonacci.grpc.pb.h>
#include <vector>


class FibonacciWriterReactor : public grpc::ServerWriteReactor<fibonacci::FibonacciResponse>
{
public:
	FibonacciWriterReactor(const fibonacci::FibonacciRequest* request)
	{
		this->SetResponseStream(request->value());
		this->current_response = responses_stream.begin();
		this->NextWrite();
	}

	void OnDone() override
	{
		std::cout << "RPC Completed" << std::endl;
		delete this;
	}

	void OnCancel() override 
	{
		std::cerr << "RPC Cancelled" << std::endl;
		delete this;
	}

	void OnWriteDone(bool ok) override
	{
		if (!ok) 
		{
			this->Finish(grpc::Status(grpc::StatusCode::UNKNOWN, "Write failed."));
		}

		this->NextWrite();
	}
	
private:
	void NextWrite()
	{
		if (this->current_response == this->responses_stream.end())
		{
			Finish(grpc::Status::OK);
			return;
		}

		const fibonacci::FibonacciResponse& response = *this->current_response;
		this->current_response++;
		this->StartWrite(&response);
	}

	void SetResponseStream(unsigned int limit)
	{
		this->responses_stream.reserve(limit);
		unsigned int a = 0, b = 1;
		for (unsigned int i = 0; i < limit; i++)
		{
			fibonacci::FibonacciResponse response;
			response.set_value(a);
			this->responses_stream.push_back(std::move(response));
			unsigned int next = a + b;
			a = b;
			b = next;
		}
	}

private:
	std::vector<fibonacci::FibonacciResponse> responses_stream;
	std::vector<fibonacci::FibonacciResponse>::iterator current_response;
};

class FibonacciServiceRpc : public fibonacci::FibonacciService::CallbackService
{
public:
	FibonacciServiceRpc(unsigned short port)
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

	grpc::ServerWriteReactor<fibonacci::FibonacciResponse>* GetFibonacciSequence(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest* request) override
	{
		return new FibonacciWriterReactor(request);
	}

private:
	std::string host;
};


int main(int argc, char** argv)
{
	std::cout << "..:: 09-fibonacci-callback ::.." << std::endl;
	
	if (argc != 2)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		FibonacciServiceRpc service(port);
		service.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}

	return 0;
}