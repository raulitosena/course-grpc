#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.pb.h>
#include <proto/fibonacci.grpc.pb.h>
#include <mutex>
#include <condition_variable>


class FibonacciReaderReactor : public grpc::ClientReadReactor<fibonacci::FibonacciResponse>
{
public:
	FibonacciReaderReactor(fibonacci::FibonacciService::Stub* stub, const fibonacci::FibonacciRequest& request)
		: done(false)
	{
		stub->async()->GetFibonaccisStream(&this->context, &request, this);
		this->StartCall();
		this->StartRead(&this->response);
	}

	void OnReadDone(bool ok) override 
	{
		if (ok)
		{
			this->fibonacci_numbers.push_back(this->response.number());
			this->StartRead(&this->response);
		}
	}

	void OnDone(const grpc::Status& status) override
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->status = std::move(status);
		this->done = true;
		this->cv.notify_one();
	}
	
	grpc::Status Await(std::vector<unsigned int>& sequence)
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->cv.wait(lock, [this] { return this->done; });
		sequence = std::move(this->fibonacci_numbers);
		return this->status;
	}

private:
	grpc::ClientContext context;
	fibonacci::FibonacciResponse response;
	std::mutex mtx;
	std::condition_variable cv;
	grpc::Status status;
	bool done = false;
	std::vector<unsigned int> fibonacci_numbers;
};

class FibonacciClient {
public:
	explicit FibonacciClient(std::shared_ptr<grpc::Channel> channel) : stub(fibonacci::FibonacciService::NewStub(channel))
	{		
	}

	std::vector<unsigned int> GetFibonacciSequence(unsigned int num) 
	{
		fibonacci::FibonacciRequest request;
		request.set_number(num);

		FibonacciReaderReactor reactor(this->stub.get(), request);
		std::vector<unsigned int> sequence;
		grpc::Status status = reactor.Await(sequence);

		if (status.ok()) 
		{
			std::cout << "GetFibonacciSequence RPC succeeded." << std::endl;
		} 
		else
		{
			throw std::runtime_error(status.error_message());
		}

		return sequence;
	}

private:
	std::unique_ptr<fibonacci::FibonacciService::Stub> stub;
};

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./client <host:port> <limit>" << std::endl;
		return 1001;
	}

	try
	{
		std::string host = argv[1];
		uint32_t limit = std::stoul(argv[2]);

		std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		FibonacciClient client(channel);
		std::vector<unsigned int> sequence = client.GetFibonacciSequence(limit);

		std::cout << "From Fibonacci server: ";
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