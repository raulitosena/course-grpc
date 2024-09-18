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
		while (this->current_response != this->responses_stream.end()) 
		{
			const fibonacci::FibonacciResponse& f = *this->current_response;
			this->current_response++;
			this->StartWrite(&f);
			return;
		}

		Finish(grpc::Status::OK);
	}

	void SetResponseStream(unsigned int n)
	{
		this->responses_stream.reserve(n);  // Reserve space for `n` responses
		unsigned int a = 0, b = 1;
		for (unsigned int i = 0; i < n; ++i)
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