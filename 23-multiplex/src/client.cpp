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

class Reactor6x : public grpc::ClientUnaryReactor
{
public:
	Reactor6x(::math::MathService::Stub* stub, const int32_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->Calculate6x(&this->context, &this->request, &this->response, this);
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

class Client6x
{
public:
	explicit Client6x(std::shared_ptr<grpc::Channel> channel) : stub(::math::MathService::NewStub(channel))
	{
	}

	int32_t Calculate(int32_t number) 
	{
		Reactor6x reactor(this->stub.get(), number);
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

class Reactor7x : public grpc::ClientUnaryReactor
{
public:
	Reactor7x(::math::MathService::Stub* stub, const int32_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->Calculate7x(&this->context, &this->request, &this->response, this);
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

class Client7x
{
public:
	explicit Client7x(std::shared_ptr<grpc::Channel> channel) : stub(::math::MathService::NewStub(channel))
	{
	}

	int32_t Calculate(int32_t number) 
	{
		Reactor7x reactor(this->stub.get(), number);
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

class Reactor8x : public grpc::ClientUnaryReactor
{
public:
	Reactor8x(::math::MathService::Stub* stub, const int32_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->Calculate8x(&this->context, &this->request, &this->response, this);
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

class Client8x
{
public:
	explicit Client8x(std::shared_ptr<grpc::Channel> channel) : stub(::math::MathService::NewStub(channel))
	{
	}

	int32_t Calculate(int32_t number) 
	{
		Reactor8x reactor(this->stub.get(), number);
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

class Reactor9x : public grpc::ClientUnaryReactor
{
public:
	Reactor9x(::math::MathService::Stub* stub, const int32_t& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->Calculate9x(&this->context, &this->request, &this->response, this);
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

class Client9x
{
public:
	explicit Client9x(std::shared_ptr<grpc::Channel> channel) : stub(::math::MathService::NewStub(channel))
	{
	}

	int32_t Calculate(int32_t number) 
	{
		Reactor9x reactor(this->stub.get(), number);
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
	std::vector<int32_t> results_6x(number + 1);
	std::vector<int32_t> results_7x(number + 1);
	std::vector<int32_t> results_8x(number + 1);
	std::vector<int32_t> results_9x(number + 1);

	std::unique_ptr<Client2x> client_2x = std::make_unique<Client2x>(channel);
	std::unique_ptr<Client3x> client_3x = std::make_unique<Client3x>(channel);
	std::unique_ptr<Client4x> client_4x = std::make_unique<Client4x>(channel);
	std::unique_ptr<Client5x> client_5x = std::make_unique<Client5x>(channel);
	std::unique_ptr<Client6x> client_6x = std::make_unique<Client6x>(channel);
	std::unique_ptr<Client7x> client_7x = std::make_unique<Client7x>(channel);
	std::unique_ptr<Client8x> client_8x = std::make_unique<Client8x>(channel);
	std::unique_ptr<Client9x> client_9x = std::make_unique<Client9x>(channel);

	// Vector to hold threads
	std::vector<std::thread> threads;

	std::cout << "Computing..." << std::endl;

	// Launch threads for each number from 0 to the input number
	for (int32_t i = 0; i <= number; ++i)
	{		
		// Create threads for both Double and Triple clients, passing result arrays by reference
		threads.emplace_back([&client_2x, i, &results_2x]() { results_2x[i] = client_2x->Calculate(i); });
		threads.emplace_back([&client_3x, i, &results_3x]() { results_3x[i] = client_3x->Calculate(i); });
		threads.emplace_back([&client_4x, i, &results_4x]() { results_4x[i] = client_4x->Calculate(i); });
		threads.emplace_back([&client_5x, i, &results_5x]() { results_5x[i] = client_5x->Calculate(i); });
		threads.emplace_back([&client_6x, i, &results_6x]() { results_6x[i] = client_6x->Calculate(i); });
		threads.emplace_back([&client_7x, i, &results_7x]() { results_7x[i] = client_7x->Calculate(i); });
		threads.emplace_back([&client_8x, i, &results_8x]() { results_8x[i] = client_8x->Calculate(i); });
		threads.emplace_back([&client_9x, i, &results_9x]() { results_9x[i] = client_9x->Calculate(i); });
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
			<< " | (x6): " << results_6x[i] 
			<< " | (x7): " << results_7x[i] 
			<< " | (x8): " << results_8x[i] 
			<< " | (x9): " << results_9x[i] 
			<< std::endl;
	}

	return 0;
}