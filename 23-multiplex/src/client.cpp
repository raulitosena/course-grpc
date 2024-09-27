#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/sqrt.grpc.pb.h>
#include <iostream>
#include <mutex>
#include <condition_variable>


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

int main(int argc, char** argv) 
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./client <port>" << std::endl;
		return 1001;
	}

	std::unique_ptr<DoubleClient> client_double;
	std::unique_ptr<TripleClient> client_triple;

	try
	{
		unsigned short port = std::stoi(argv[1]);
		int32_t number = std::stoi(argv[2]);
		std::string host = absl::StrFormat("localhost:%d", port);		
		std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());

		client_double = std::make_unique<DoubleClient>(channel);
		int32_t result_double = client_double->Calculate(number);		
		std::cout << number << " -> " << result_double << std::endl;

		client_triple = std::make_unique<TripleClient>(channel);
		int32_t result_triple = client_triple->Calculate(number);		
		std::cout << number << " -> " << result_triple << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1002;
	}
	
	return 0;
}