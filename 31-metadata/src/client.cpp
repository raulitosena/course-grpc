#include <proto/fibonacci.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>


void printMetadata(const std::multimap<grpc::string_ref, grpc::string_ref> metadata)
{
	for (auto iter = metadata.begin(); iter != metadata.end(); ++iter)
	{
		std::cout << " - Header key: " << iter->first << ", value: ";
		size_t isbin = iter->first.find("-bin");
		if ((isbin != std::string::npos) && (isbin + 4 == iter->first.size()))
		{
			std::cout << std::hex;
			for (auto c : iter->second)
			{
				std::cout << static_cast<unsigned int>(c);
			}
			std::cout << std::dec;
		}
		else
		{
			std::cout << iter->second;
		}
		std::cout << std::endl;
	}
}

class FibonacciClientReactor : public grpc::ClientUnaryReactor
{
public:
	FibonacciClientReactor(::fibonacci::FibonacciSlowService::Stub* stub, const uint64_t& number)
		: done(false)
	{
		// Setup metadata (request timestamp, auth token, client ID, request ID, ...)
		// Metadata generated size (system headers + user headers): ~788

		auto now = std::chrono::system_clock::now();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		this->context.AddMetadata("request-timestamp", std::to_string(millis));
		this->context.AddMetadata("token", "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJsb2dnZWRJbk");
		this->context.AddMetadata("client-id", std::string("client-6789"));
		this->context.AddMetadata("request-id", "37");
		char bytes[8] = {'\0', '\1', '\2', '\3', '\4', '\5', '\6', '\7'};

		// binary data header
		this->context.AddMetadata("custom-bin", std::string(bytes, 8));
		
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
			std::cout << "Server user-defined metadata: " << std::endl;
			const std::multimap<grpc::string_ref, grpc::string_ref> metadata = context.GetServerInitialMetadata();

			printMetadata(metadata);
			
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

	uint64_t number;
	std::shared_ptr<grpc::Channel> channel;
	std::unique_ptr<FibonacciClient> client;

	try
	{
		std::string host = argv[1];
		number = std::stoull(argv[2]);
		
		channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
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