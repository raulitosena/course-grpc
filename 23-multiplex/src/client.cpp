#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sqrt.grpc.pb.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>



class DoubleReactor : public grpc::ClientUnaryReactor
{
public:
	DoubleReactor(::math::MathService::Stub* stub, const int32_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->CalculateDouble(&this->context, &this->request, &this->response, this);
		this->StartCall();
	}

	void OnDone(const grpc::Status& status) override
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->status = status;
		this->done = true;
		this->cv.notify_one();
	}

	grpc::Status Await(int32_t& result)
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->cv.wait(lock, [this] { return this->done; });
		result = this->response.result();
		return this->status;
	}

private:
	grpc::ClientContext context;
	::math::ResultResponse response;
	::math::OperandRequest request;
	std::mutex mtx;
	std::condition_variable cv;
	grpc::Status status;
	bool done;
};

class DoubleClient
{
public:
	explicit DoubleClient(std::shared_ptr<grpc::Channel> channel) : stub(::math::MathService::NewStub(channel))
	{
	}

	int32_t Calculate(int32_t number) 
	{
		DoubleReactor reactor(this->stub.get(), number);
		int32_t result;
		grpc::Status status = reactor.Await(result);

		if (!status.ok())
		{
			throw std::runtime_error(status.error_message());
		}

		return result;
	}

private:
	std::unique_ptr<::math::MathService::Stub> stub;
};

class TripleReactor : public grpc::ClientUnaryReactor
{
public:
	TripleReactor(::math::MathService::Stub* stub, const int32_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->CalculateTriple(&this->context, &this->request, &this->response, this);
		this->StartCall();
	}

	void OnDone(const grpc::Status& status) override
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->status = status;
		this->done = true;
		this->cv.notify_one();
	}

	grpc::Status Await(int32_t& result)
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->cv.wait(lock, [this] { return this->done; });
		result = this->response.result();
		return this->status;
	}

private:
	grpc::ClientContext context;
	::math::ResultResponse response;
	::math::OperandRequest request;
	std::mutex mtx;
	std::condition_variable cv;
	grpc::Status status;
	bool done;
};

class TripleClient
{
public:
	explicit TripleClient(std::shared_ptr<grpc::Channel> channel) : stub(::math::MathService::NewStub(channel))
	{
	}

	int32_t Calculate(int32_t number) 
	{
		TripleReactor reactor(this->stub.get(), number);
		int32_t result;
		grpc::Status status = reactor.Await(result);

		if (!status.ok())
		{
			throw std::runtime_error(status.error_message());
		}

		return result;
	}

private:
	std::unique_ptr<::math::MathService::Stub> stub;
};

void run_double_client(DoubleClient* client, int32_t number, int32_t* result_double)
{
	try
	{
		*result_double = client->Calculate(number);
	}
	catch (const std::exception& e)
	{
		std::cerr << "DoubleClient Error: " << e.what() << std::endl;
	}
}

void run_triple_client(TripleClient* client, int32_t number, int32_t* result_triple)
{
	try
	{
		*result_triple = client->Calculate(number);
	}
	catch (const std::exception& e)
	{
		std::cerr << "TripleClient Error: " << e.what() << std::endl;
	}
}

int main(int argc, char** argv) 
{
	unsigned short port;
	int32_t number;
	std::string host;
	std::shared_ptr<grpc::Channel> channel;

	if (argc != 3)
	{
		std::cerr << "Usage: ./client <port> <number>" << std::endl;
		return 1001;
	}

	try
	{
		port = std::stoi(argv[1]);
		number = std::stoi(argv[2]);
		host = absl::StrFormat("localhost:%d", port);
		channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1002;
	}

	// Declare arrays to store results
	std::vector<int32_t> results_double(number + 1);
	std::vector<int32_t> results_triple(number + 1);

	std::unique_ptr<DoubleClient> client_double = std::make_unique<DoubleClient>(channel);
	std::unique_ptr<TripleClient> client_triple = std::make_unique<TripleClient>(channel);

	// Vector to hold threads
	std::vector<std::thread> threads;

	std::cout << "Computing..." << std::endl;

	// Launch threads for each number from 0 to the input number
	for (int32_t i = 0; i <= number; ++i)
	{
		// Create threads for both Double and Triple clients, passing result arrays by reference
		threads.emplace_back(run_double_client, client_double.get(), i, &results_double[i]);
		threads.emplace_back(run_triple_client, client_triple.get(), i, &results_triple[i]);
	}

	// Join all threads
	for (auto& th : threads)
	{
		th.join();
	}

	// Print final results
	std::cout << "Final Results:" << std::endl;
	for (int32_t i = 0; i <= number; ++i)
	{
		std::cout << i << " -> Double: " << results_double[i] << ", Triple: " << results_triple[i] << std::endl;
	}

	return 0;
}