#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/math.grpc.pb.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>



class Reactor2x : public grpc::ClientUnaryReactor
{
public:
	Reactor2x(::math::MathService::Stub* stub, const int32_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->Calculate2x(&this->context, &this->request, &this->response, this);
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

class Client2x
{
public:
	explicit Client2x(std::shared_ptr<grpc::Channel> channel) : stub(::math::MathService::NewStub(channel))
	{
	}

	int32_t Calculate(int32_t number) 
	{
		Reactor2x reactor(this->stub.get(), number);
		int32_t result;
		grpc::Status status = reactor.Await(result);

		if (!status.ok())
		{
			std::stringstream ss;
			ss << "RPC error: " << status.error_message() << " code: " << status.error_code() << std::endl;			
			throw std::runtime_error(ss.str());
		}

		return result;
	}

private:
	std::unique_ptr<::math::MathService::Stub> stub;
};

class Reactor3x : public grpc::ClientUnaryReactor
{
public:
	Reactor3x(::math::MathService::Stub* stub, const int32_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->Calculate3x(&this->context, &this->request, &this->response, this);
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

class Client3x
{
public:
	explicit Client3x(std::shared_ptr<grpc::Channel> channel) : stub(::math::MathService::NewStub(channel))
	{
	}

	int32_t Calculate(int32_t number) 
	{
		Reactor3x reactor(this->stub.get(), number);
		int32_t result;
		grpc::Status status = reactor.Await(result);

		if (!status.ok())
		{
			std::stringstream ss;
			ss << "RPC error: " << status.error_message() << " code: " << status.error_code() << std::endl;			
			throw std::runtime_error(ss.str());
		}

		return result;
	}

private:
	std::unique_ptr<::math::MathService::Stub> stub;
};

class Reactor4x : public grpc::ClientUnaryReactor
{
public:
	Reactor4x(::math::MathService::Stub* stub, const int32_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->Calculate4x(&this->context, &this->request, &this->response, this);
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

class Client4x
{
public:
	explicit Client4x(std::shared_ptr<grpc::Channel> channel) : stub(::math::MathService::NewStub(channel))
	{
	}

	int32_t Calculate(int32_t number) 
	{
		Reactor4x reactor(this->stub.get(), number);
		int32_t result;
		grpc::Status status = reactor.Await(result);

		if (!status.ok())
		{
			std::stringstream ss;
			ss << "RPC error: " << status.error_message() << " code: " << status.error_code() << std::endl;			
			throw std::runtime_error(ss.str());
		}

		return result;
	}

private:
	std::unique_ptr<::math::MathService::Stub> stub;
};

class Reactor5x : public grpc::ClientUnaryReactor
{
public:
	Reactor5x(::math::MathService::Stub* stub, const int32_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->Calculate5x(&this->context, &this->request, &this->response, this);
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

class Client5x
{
public:
	explicit Client5x(std::shared_ptr<grpc::Channel> channel) : stub(::math::MathService::NewStub(channel))
	{
	}

	int32_t Calculate(int32_t number) 
	{
		Reactor5x reactor(this->stub.get(), number);
		int32_t result;
		grpc::Status status = reactor.Await(result);

		if (!status.ok())
		{
			std::stringstream ss;
			ss << "RPC error: " << status.error_message() << " code: " << status.error_code() << std::endl;			
			throw std::runtime_error(ss.str());
		}

		return result;
	}

private:
	std::unique_ptr<::math::MathService::Stub> stub;
};

void run_client2x(Client2x* client, int32_t number, int32_t* result_double)
{
	try
	{
		*result_double = client->Calculate(number);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Client2x Error: " << e.what() << std::endl;
	}
}

void run_client3x(Client3x* client, int32_t number, int32_t* result_triple)
{
	try
	{
		*result_triple = client->Calculate(number);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Client3x Error: " << e.what() << std::endl;
	}
}

void run_client4x(Client4x* client, int32_t number, int32_t* result_double)
{
	try
	{
		*result_double = client->Calculate(number);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Client4x Error: " << e.what() << std::endl;
	}
}

void run_client5x(Client5x* client, int32_t number, int32_t* result_double)
{
	try
	{
		*result_double = client->Calculate(number);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Client5x Error: " << e.what() << std::endl;
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
	std::vector<int32_t> results_2x(number + 1);
	std::vector<int32_t> results_3x(number + 1);
	std::vector<int32_t> results_4x(number + 1);
	std::vector<int32_t> results_5x(number + 1);

	std::unique_ptr<Client2x> client_2x = std::make_unique<Client2x>(channel);
	std::unique_ptr<Client3x> client_3x = std::make_unique<Client3x>(channel);
	std::unique_ptr<Client4x> client_4x = std::make_unique<Client4x>(channel);
	std::unique_ptr<Client5x> client_5x = std::make_unique<Client5x>(channel);

	// Vector to hold threads
	std::vector<std::thread> threads;

	std::cout << "Computing..." << std::endl;

	// Launch threads for each number from 0 to the input number
	for (int32_t i = 0; i <= number; ++i)
	{		
		// Create threads for both Double and Triple clients, passing result arrays by reference
		threads.emplace_back(run_client2x, client_2x.get(), i, &results_2x[i]);
		threads.emplace_back(run_client3x, client_3x.get(), i, &results_3x[i]);
        threads.emplace_back(run_client4x, client_4x.get(), i, &results_4x[i]);
        threads.emplace_back(run_client5x, client_5x.get(), i, &results_5x[i]);
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
		//std::cout << i << " -> Double: " << results_2x[i] << ", Triple: " << results_3x[i] << std::endl;
        std::cout << i 
            << " | (x2): " << results_2x[i] 
            << " | (x3): " << results_3x[i] 
            << " | (x4): " << results_4x[i] 
            << " | (x5): " << results_5x[i] 
            << std::endl;
	}

	return 0;
}