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


class FibonacciServiceImpl : public fibonacci::FibonacciService::CallbackService
{
public:
	grpc::ServerWriteReactor<fibonacci::FibonacciResponse>* GetFibonacciSequence(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest* request) override
	{
		return new FibonacciWriterReactor(request);
	}
};


int main(int argc, char** argv)
{
	std::cout << "..:: 09-fibonacci-callback ::.." << std::endl;
	int port = 5000;

	try
	{
		if (argc == 2)
			port = std::stoi(argv[1]);
	
		std::string host = absl::StrFormat("localhost:%d", port);

		FibonacciServiceImpl service;
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