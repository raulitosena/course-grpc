#include <proto/sqrt.grpc.pb.h>
#include <iostream>
#include <mutex>
#include <condition_variable>


class SqrtReactor : public grpc::ClientUnaryReactor
{
public:
	SqrtReactor(::squareroot::SqrtService::Stub* stub, const int& number)
		: done(false)
	{
		this->request.set_number(number);
		stub->async()->Calculate(&this->context, &this->request, &this->response, this);
		this->StartCall();
	}

	void OnDone(const grpc::Status& status) override
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->status = status;
		this->done = true;
		this->cv.notify_one();
	}

	grpc::Status Await(double& result)
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->cv.wait(lock, [this] { return this->done; });
		result = this->response.result();
		return this->status;
	}

private:
	grpc::ClientContext context;
	::squareroot::SqrtResponse response;
	::squareroot::SqrtRequest request;
	std::mutex mtx;
	std::condition_variable cv;
	grpc::Status status;
	bool done;
};

class SqrtClientWithReactor
{
public:
	explicit SqrtClientWithReactor(std::shared_ptr<grpc::Channel> channel) : stub(::squareroot::SqrtService::NewStub(channel))
	{
	}

	double Calculate(int number) 
	{
		SqrtReactor reactor(this->stub.get(), number);
		double result;
		grpc::Status status = reactor.Await(result);

		if (!status.ok())
		{
			std::cerr << " - Status.code: " << status.error_code() << std::endl;
			std::cerr << " - Status.message: " << status.error_message() << std::endl;
			std::cerr << " - Status.details: " << status.error_details() << std::endl;
			throw std::runtime_error(status.error_message());
		}

		return result;
	}

private:
	std::unique_ptr<::squareroot::SqrtService::Stub> stub;
};
