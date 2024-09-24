#include <proto/fibonacci.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <mutex>
#include <condition_variable>


class LoggingInterceptor : public grpc::experimental::Interceptor 
{
public:
	LoggingInterceptor(grpc::experimental::ClientRpcInfo* info) 
	{
	}

	void Intercept(::grpc::experimental::InterceptorBatchMethods* methods) override {

		if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA))
		{
			std::cout << "Reactor RPC call started: " << std::endl;
		}

		methods->Proceed();
	}
};

class LoggingInterceptorFactory : public grpc::experimental::ClientInterceptorFactoryInterface
{
public:
  	grpc::experimental::Interceptor* CreateClientInterceptor(grpc::experimental::ClientRpcInfo* info) override 
	{
	    return new LoggingInterceptor(info);
  	}
};

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
	if (argc != 3)
	{
		std::cerr << "Usage: ./client <port> <limit>" << std::endl;
		return 1001;
	}

	int port;
	uint64_t number;
	std::shared_ptr<grpc::Channel> channel;
	std::unique_ptr<FibonacciClient> client;

	try
	{
		port = std::stoi(argv[1]);
		number = std::stoull(argv[2]);
		std::string host = absl::StrFormat("localhost:%d", port);

		std::vector<std::unique_ptr<grpc::experimental::ClientInterceptorFactoryInterface>> interceptor_creators;
		interceptor_creators.push_back(std::make_unique<LoggingInterceptorFactory>());
		grpc::ChannelArguments args;
		channel = grpc::experimental::CreateCustomChannelWithInterceptors(host, grpc::InsecureChannelCredentials(), args, std::move(interceptor_creators));
		client = std::make_unique<FibonacciClient>(channel);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1002;
	}

	try
	{
		std::vector<unsigned long long> sequence = client->Calculate(number);

		std::cout << "From Fibonacci server: \n";
		for (auto&& val : sequence)
		{
			std::cout << val << " ";
		}
		std::cout << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}