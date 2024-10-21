#include <proto/fibonacci.grpc.pb.h>
#include <iostream>
#include <vector>


unsigned long long getFibonacci(unsigned long long n)
{
	if (n <= 1)
	{
		return n;
	}
	
	return getFibonacci(n - 1) + getFibonacci(n - 2);
}

class FibonacciServerSlowReactor : public grpc::ServerUnaryReactor 
{
public:
	FibonacciServerSlowReactor(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest& request, fibonacci::FibonacciListResponse* response) 
	{
		uint64_t number = request.number();
		std::vector<unsigned long long> fibonacci_list;

		for (unsigned long long i = 0; i < number; ++i)
		{
			std::cerr << i << " " << std::flush;
			fibonacci_list.push_back(getFibonacci(i));
		}
		
		for (auto &&num : fibonacci_list)
		{
			response->add_number(num);
		}

		this->Finish(grpc::Status::OK);
	}

private:
	void OnDone() override 
	{
		std::cout << "RPC done!" << std::endl;
		delete this;
	}

	void OnCancel() override 
	{
		std::cerr << "RPC cancelled!" << std::endl;
	}
};

class FibonacciSlowServiceImpl : public fibonacci::FibonacciSlowService::CallbackService
{
public:	
	grpc::ServerUnaryReactor* GetFibonacciList(grpc::CallbackServerContext* context, const fibonacci::FibonacciRequest* request, fibonacci::FibonacciListResponse* response) override
	{
		return new FibonacciServerSlowReactor(context, *request, response);
	}
	
private:
	std::string host;
};
