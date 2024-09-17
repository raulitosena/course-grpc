#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.pb.h>
#include <proto/fibonacci.grpc.pb.h>
#include <vector>
#include "fibonacci.hpp"


class FibonacciServiceImpl : public fibonacci::FibonacciService::CallbackService
{
public:
	grpc::ServerWriteReactor<fibonacci::FibonacciResponse>* GetFibonacciSequence(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest* request) override
	{
		class FibonacciReactor : public grpc::ServerWriteReactor<fibonacci::FibonacciResponse>
		{
		public:
			FibonacciReactor(const fibonacci::FibonacciRequest* request, const std::vector<fibonacci::FibonacciResponse>* responses)
				: responses_(responses), next_response(responses_->begin())
			{
				this->fibonacci_sequence_ = GenerateFibonacci(request->value());

				for (auto &&value : fibonacci_sequence_)
				{
					fibonacci::FibonacciResponse fibo;
					fibo.set_value(value);
					
					//this->responses->push_back(std::move(fibo));
					std::vector<fibonacci::FibonacciResponse>* res;
					res->push_back(std::move(fibo));
				}

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
				while (this->next_response != this->responses_->end()) 
				{
					const fibonacci::FibonacciResponse& f = *this->next_response;
					this->next_response++;
					this->StartWrite(&f);
					return;
				}

				Finish(grpc::Status::OK);
			}

			std::vector<uint32_t> fibonacci_sequence_;
			const std::vector<fibonacci::FibonacciResponse>* responses_;
			std::vector<fibonacci::FibonacciResponse>::const_iterator next_response;
		};


		return new FibonacciReactor(request, &feature_list_);
	}

private:
  std::vector<fibonacci::FibonacciResponse> feature_list_;
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