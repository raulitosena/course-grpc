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
		this->SetResponseStream(request->number());
		this->current_response = stream_responses.begin();
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
		if (this->current_response == this->stream_responses.end())
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
		this->stream_responses.reserve(limit);
		unsigned int a = 0, b = 1;
		for (unsigned int i = 0; i < limit; i++)
		{
			fibonacci::FibonacciResponse response;
			response.set_number(a);
			this->stream_responses.push_back(std::move(response));
			unsigned int next = a + b;
			a = b;
			b = next;
		}
	}

private:
	std::vector<fibonacci::FibonacciResponse> stream_responses;
	std::vector<fibonacci::FibonacciResponse>::iterator current_response;
};

class FibonacciServiceImpl : public fibonacci::FibonacciService::CallbackService
{
public:
	FibonacciServiceImpl(unsigned short port)
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

	grpc::ServerWriteReactor<fibonacci::FibonacciResponse>* GetFibonacciStream(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest* request) override
	{
		return new FibonacciWriterReactor(request);
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
		FibonacciServiceImpl service(port);
		service.Run();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}

	return 0;
}