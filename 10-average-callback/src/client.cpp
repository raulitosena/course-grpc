#include <grpcpp/grpcpp.h>
#include <proto/average.pb.h>
#include <proto/average.grpc.pb.h>
#include <condition_variable>


class AverageWriteReactor : public grpc::ClientWriteReactor<average::AvgSample>
{
public:
	AverageWriteReactor(average::AverageService::Stub* stub, const std::vector<int>& samples)
		: samples_(samples), index_(0) 
	{
		stub->async()->ComputeAvg(&this->context_, &this->response_, this);
	}

	void OnWriteDone(bool ok) override
	{
		if (ok && index_ < samples_.size())
		{
			// Send the next sample if available
			this->request.set_value(samples_[index_++]);
			std::cout << "Write " << this->request.value() << std::endl;
			StartWrite(&this->request);  // Asynchronous write
		}
		else
		{
			// No more samples to send
			this->StartWritesDone();  // Signal that writes are done
		}
	}

	void OnDone(const grpc::Status& status) override
	{
		std::unique_lock<std::mutex> lock(this->mtx_);
		this->status_ = status;
		this->done_ = true;
		this->cv_.notify_one();
	}

	grpc::Status Await(average::AvgTotal& response)
	{
		std::unique_lock<std::mutex> lock(this->mtx_);
		this->cv_.wait(lock, [this]	{ return this->done_; });
		response = std::move(this->response_);
		return std::move(this->status_);
	}

	void Push()
	{
		this->StartCall();

		if (index_ < samples_.size())
		{
			this->request.set_value(samples_[index_++]);
			std::cout << "Write " << this->request.value() << std::endl;
			StartWrite(&this->request);  // Start the first write
		}
		else
		{
			StartWritesDone();  // No samples to send, finish early
		}
	}

private:
	grpc::ClientContext context_;
	average::AvgSample request;
	average::AvgTotal response_;
	std::vector<int> samples_;
	size_t index_;
	std::mutex mtx_;
	std::condition_variable cv_;
	grpc::Status status_;
	bool done_ = false;
};


class AverageClient
{
public:
	explicit AverageClient(std::shared_ptr<grpc::Channel> channel) : stub(average::AverageService::NewStub(channel))
	{
	}

	float ComputeAverage(const std::vector<int>& samples)
	{
		AverageWriteReactor reactor(this->stub.get(), samples);

		// Start the sending process
		reactor.Push();

		// Wait for the response
		average::AvgTotal response;
		grpc::Status status = reactor.Await(response);

		if (status.ok())
		{
			return response.value();
		}
		else
		{
			throw std::runtime_error(status.error_message());
		}
	}

private:
	std::unique_ptr<average::AverageService::Stub> stub;
};


int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./client <port>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		std::string host = absl::StrFormat("localhost:%d", port);
		auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		AverageClient client(channel);
		std::vector<int> samples = { 1, 2, 3, 4 };
		float average = client.ComputeAverage(samples);
		std::cout << "Average: " << average << std::endl;	
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}