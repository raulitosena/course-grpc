#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.pb.h>
#include <proto/fibonacci.grpc.pb.h>
#include <condition_variable>


class FibonacciReaderReactor : public grpc::ClientReadReactor<fibonacci::FibonacciResponse>
{
public:
	FibonacciReaderReactor(fibonacci::FibonacciService::Stub* stub, const fibonacci::FibonacciRequest& rect)
	{
		stub->async()->GetFibonacciSequence(&context_, &rect, this);
		StartRead(&response_);
		StartCall();
	}

	void OnReadDone(bool ok) override 
	{
		if (ok)
		{
			this->sequence_.push_back(response_.value());
			StartRead(&response_);
		}
	}

	void OnDone(const grpc::Status& s) override
	{
		std::unique_lock<std::mutex> l(mu_);
		status_ = s;
		done_ = true;
		cv_.notify_one();
	}
	
	grpc::Status Await(std::vector<unsigned int>& sequence)
	{
		std::unique_lock<std::mutex> l(mu_);
		cv_.wait(l, [this] { return done_; });
		sequence = std::move(this->sequence_);
		return std::move(status_);
	}

private:
	grpc::ClientContext context_;
	fibonacci::FibonacciResponse response_;
	std::mutex mu_;
	std::condition_variable cv_;
	grpc::Status status_;
	bool done_ = false;
	std::vector<unsigned int> sequence_;
};

class FibonacciClient {
public:
	explicit FibonacciClient(std::shared_ptr<grpc::Channel> channel) : stub(fibonacci::FibonacciService::NewStub(channel))
	{		
	}

	std::vector<unsigned int> GetFibonacciSequence(unsigned int num) 
	{
		fibonacci::FibonacciRequest request;
		request.set_value(num);

		FibonacciReaderReactor reader(this->stub.get(), request);
		std::vector<unsigned int> sequence;
		grpc::Status status = reader.Await(sequence);

		if (status.ok()) 
		{
			std::cout << "GetFibonacciSequence RPC succeeded." << std::endl;
		} 
		else
		{
			std::cerr << "GetFibonacciSequence RPC failed: " << status.error_message() << std::endl;
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
		std::cerr << "Missing parameters! Usage: ./client <port> <limit>" << std::endl;
		return 1001;
	}

	int port = std::stoi(argv[1]);
	uint32_t limit = std::stoul(argv[2]);
	std::string host = absl::StrFormat("localhost:%d", port);

	std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
	FibonacciClient client(channel);
	std::vector<unsigned int> sequence = client.GetFibonacciSequence(limit);

	std::cout << "From Fibonacci server: ";
	for (auto&& val : sequence)
	{
		std::cout << val << " ";
	}
	std::cout << std::endl;

	return 0;
}