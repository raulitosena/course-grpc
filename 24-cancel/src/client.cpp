#include <proto/fibonacci.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

class FibonacciClientReactor : public grpc::ClientUnaryReactor
{
public:
	FibonacciClientReactor(fibonacci::FibonacciSlowService::Stub* stub, const uint64_t& number)
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

	void Cancel() 
	{
		this->context.TryCancel();
	}

private:
	grpc::ClientContext context;
	fibonacci::FibonacciListResponse response;
	fibonacci::FibonacciRequest request;
	std::mutex mtx;
	std::condition_variable cv;
	grpc::Status status;
	bool done;
};

class FibonacciClient
{
public:
	explicit FibonacciClient(std::shared_ptr<grpc::Channel> channel) 
		: stub(fibonacci::FibonacciSlowService::NewStub(channel))
	{
	}

	std::vector<unsigned long long> Calculate(uint64_t number) 
	{
		this->reactor = std::make_unique<FibonacciClientReactor>(this->stub.get(), number);
		std::vector<unsigned long long> fibonacci_list;

		grpc::Status status = this->reactor->Await(fibonacci_list);

		if (!status.ok())
		{
			std::stringstream ss;
			ss << status.error_message() << " (" << status.error_code() << ")";
			throw std::runtime_error(ss.str());
		}

		return fibonacci_list;
	}

	void Cancel()
	{
		if (this->reactor)
		{
			this->reactor->Cancel();
		}
	}

private:
	std::unique_ptr<fibonacci::FibonacciSlowService::Stub> stub;
	std::unique_ptr<FibonacciClientReactor> reactor;
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
		channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		client = std::make_unique<FibonacciClient>(channel);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1002;
	}

	// Start the cancellation thread in main
	std::thread cancel_thread([&client]()
	{
		std::cout << "Press 'c' to cancel the request..." << std::endl;
		char c;
		std::cin >> c;
		if (c == 'c')
		{
			client->Cancel();
			std::cout << "Client sent cancellation request!" << std::endl;
		}
	});

	try
	{
		std::vector<unsigned long long> sequence = client->Calculate(number);

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

	cancel_thread.join();

	return 0;
}
