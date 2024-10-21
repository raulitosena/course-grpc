#include <proto/fibonacci.grpc.pb.h>
#include <iostream>
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
	
	grpc::ServerWriteReactor<fibonacci::FibonacciResponse>* GetFibonacciStream(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest* request) override
	{
		return new FibonacciWriterReactor(request);
	}

private:
	std::string host;
};
