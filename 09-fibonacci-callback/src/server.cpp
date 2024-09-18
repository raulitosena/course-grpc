#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.pb.h>
#include <proto/fibonacci.grpc.pb.h>
#include <vector>
#include "fibonacci.hpp"


class FibonacciWriterReactor : public grpc::ServerWriteReactor<fibonacci::FibonacciResponse>
{
public:
	FibonacciWriterReactor(const fibonacci::FibonacciRequest* request, const std::vector<fibonacci::FibonacciResponse>* responses)
		: responses_ptr(responses), current_response(responses_ptr->begin())
	{
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
		while (this->current_response != this->responses_ptr->end()) 
		{
			const fibonacci::FibonacciResponse& f = *this->current_response;
			this->current_response++;
			this->StartWrite(&f);
			return;
		}

		Finish(grpc::Status::OK);
	}

	const std::vector<fibonacci::FibonacciResponse>* responses_ptr;
	std::vector<fibonacci::FibonacciResponse>::const_iterator current_response;
};

class FibonacciServiceImpl : public fibonacci::FibonacciService::CallbackService
{
public:
	grpc::ServerWriteReactor<fibonacci::FibonacciResponse>* GetFibonacciSequence(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest* request) override
	{
		this->responses_stream.clear();
		auto fibonacci = GenerateFibonacci(request->value());

		for (auto &&value : fibonacci)
		{
			fibonacci::FibonacciResponse response;
			response.set_value(value);			
			this->responses_stream.push_back(response);
		}
		return new FibonacciWriterReactor(request, &this->responses_stream);
	}

private:
	std::vector<fibonacci::FibonacciResponse> responses_stream;
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