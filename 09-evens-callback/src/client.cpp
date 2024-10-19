#include <grpcpp/grpcpp.h>
#include <proto/evens.pb.h>
#include <proto/evens.grpc.pb.h>
#include <mutex>
#include <condition_variable>


class EvensClientBidiReactor : public grpc::ClientBidiReactor<evens::Number, evens::Number>
{
public:
	// Constructor: takes a stub and a list of numbers to send
	EvensClientBidiReactor(evens::EvensService::Stub* stub, std::vector<int> numbers)
		: numbers_to_send(std::move(numbers)), current_number(numbers_to_send.begin()), done(false)
	{
		stub->async()->FindEvens(&this->context, this);
		this->StartCall();
		this->StartRead(&response);
		this->NextWrite();
	}

	// Method called when a message is successfully sent
	void OnWriteDone(bool ok) override
	{
		if (ok) 
		{
			// Send the next number
			this->NextWrite();
		}
		else
		{
			// If not ok, finish the writes
			this->StartWritesDone();
		}
	}

	// Method called when a message is received from the server
	void OnReadDone(bool ok) override
	{
		if (ok)
		{
			// Add the even number from the server to the response vector
			this->received_evens.push_back(response.value());
			// Continue reading more responses from the server
			this->StartRead(&response);
		}
	}

	// Method called when the RPC is complete
	void OnDone(const grpc::Status& status) override
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->status = status;
		this->done = true;
		this->cv.notify_one();
	}

	// Await method to wait for completion and get the result
	grpc::Status Await(std::vector<int>& evens)
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->cv.wait(lock, [this] { return this->done; });
		evens = std::move(this->received_evens);
		return this->status;
	}

private:
	// Helper function to send the next number
	void NextWrite()
	{
		if (current_number != numbers_to_send.end()) 
		{
			this->request.set_value(*current_number);
			
			this->StartWrite(&this->request);
			// Move to the next number
			this->current_number++;
		}
		else
		{
			// If no more numbers, finish the writes
			this->StartWritesDone();
		}
	}

private:
	grpc::ClientContext context;
	evens::Number request;
	evens::Number response;
	std::vector<int> numbers_to_send;
	std::vector<int>::iterator current_number;
	std::vector<int> received_evens;
	std::mutex mtx;
	std::condition_variable cv;
	grpc::Status status;
	bool done;
};


class EvensClient
{
public:
	explicit EvensClient(std::shared_ptr<grpc::Channel> channel) : stub(evens::EvensService::NewStub(channel))
	{		
	}

	std::vector<int> GetEvens(const std::vector<int>& numbers)
	{
		grpc::ClientContext context;

		EvensClientBidiReactor reactor(this->stub.get(), numbers);

		std::vector<int> evens;
		grpc::Status status = reactor.Await(evens);

		if (status.ok()) 
		{
			std::cout << "RPC succeeded." << std::endl;
		} 
		else
		{
			std::cout << "RPC error: " << status.error_message() << " code: " << status.error_code() << std::endl;			
			throw std::runtime_error(status.error_message());
		}

		return evens;
	}

private:
	std::unique_ptr<evens::EvensService::Stub> stub;
};


int main(int argc, char** argv)
{
	// Ensure at least 1 argument (port) is provided, but no more than 11 (port + 10 numbers)
	if (argc < 2 || argc > 12)
	{
		std::cerr << "Usage: ./client <port> [numbers... (0 to 10)]" << std::endl;
		return 1001;
	}

	try
	{
		// Parse the port
		int port = std::stoi(argv[1]);
		std::string host = absl::StrFormat("localhost:%d", port);
		std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		EvensClient client(channel);

		// Parse numbers from command-line arguments (if any) into the samples vector
		std::vector<int> numbers = {};
		for (int i = 2; i < argc; ++i)
		{
			int sample = std::stoi(argv[i]);
			numbers.push_back(sample);
		}

		// If no numbers are provided, an empty vector will be passed
		std::vector<int> evens = client.GetEvens(numbers);
		std::cout << "Evens: " << evens.size() << std::endl;
		for (auto val : evens)
			std::cout << val << ' ';
		std::cout << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
	}

	return 0;
}