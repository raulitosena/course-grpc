#include <proto/fibonacci.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <mutex>
#include <vector>
#include <condition_variable>
#include "client_interceptor.hpp"


class FibonacciClientReactor : public grpc::ClientUnaryReactor
{
public:
	FibonacciClientReactor(::fibonacci::FibonacciSlowService::Stub* stub, const uint64_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->GetFibonacciList(&this->context, &this->request, &this->response, this);
		this->StartCall();
	}

	void OnDone(const grpc::Status& status) override
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->status = status;
		this->done = true;
		this->cv.notify_one();
	}

	grpc::Status Await(std::vector<unsigned long long>& result)
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->cv.wait(lock, [this] { return this->done; });

		if (this->status.ok()) 
		{
			for (size_t i = 0; i < this->response.number_size(); i++)
			{
				unsigned long long num = this->response.number(i);
				result.push_back(num);
			}
		}
		
		return this->status;
	}

private:
	grpc::ClientContext context;
	::fibonacci::FibonacciListResponse response;
	::fibonacci::FibonacciRequest request;
	std::mutex mtx;
	std::condition_variable cv;
	grpc::Status status;
	bool done;
};

class FibonacciClient
{
public:
	explicit FibonacciClient(std::shared_ptr<grpc::Channel> channel) 
		: stub(::fibonacci::FibonacciSlowService::NewStub(channel))
	{
	}

	std::vector<unsigned long long> Calculate(uint64_t number) 
	{
		FibonacciClientReactor reactor(this->stub.get(), number);
		std::vector<unsigned long long> fibonacci_list;
		grpc::Status status = reactor.Await(fibonacci_list);

		if (!status.ok())
		{
			std::stringstream ss;
			ss << status.error_message() << " (" << status.error_code() << ")";
			throw std::runtime_error(ss.str());
		}

		return fibonacci_list;
	}

private:
	std::unique_ptr<::fibonacci::FibonacciSlowService::Stub> stub;	
};

int main(int argc, char** argv) 
{
	int port;
	uint64_t number;
	std::string host;


	if (argc != 3)
	{
		std::cerr << "Usage: ./client <port> <limit>" << std::endl;
		return 1001;
	}

	try
	{
		port = std::stoi(argv[1]);
		number = std::stoull(argv[2]);
		host = absl::StrFormat("localhost:%d", port);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1002;
	}	

	try
	{
		// Bind interceptor
		std::vector<std::unique_ptr<grpc::experimental::ClientInterceptorFactoryInterface>> interceptor_creators;
		interceptor_creators.push_back(std::make_unique<ClientLogInterceptorFactory>());
		grpc::ChannelArguments args;
		std::shared_ptr<grpc::Channel> channel = grpc::experimental::CreateCustomChannelWithInterceptors(host, grpc::InsecureChannelCredentials(), args, std::move(interceptor_creators));
		
		// Set channel
		std::unique_ptr<FibonacciClient> client = std::make_unique<FibonacciClient>(channel);		

		// Calculate fibonacci
		std::vector<unsigned long long> sequence = client->Calculate(number);
		std::cout << "From Fibonacci server: \n";
		for (auto&& val : sequence)
			std::cout << val << " ";
		std::cout << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1003;
	}

	return 0;
}