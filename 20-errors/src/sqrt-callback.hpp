#include <proto/math.grpc.pb.h>
#include <iostream>
#include <mutex>
#include <condition_variable>


class SqrtClientWithCallback
{
public:
	explicit SqrtClientWithCallback(std::shared_ptr<grpc::Channel> channel)
		: stub(math::SqrtService::NewStub(channel))
	{
	}

	double Calculate(int number) 
	{
		grpc::ClientContext context;
		math::SqrtRequest request;
		math::SqrtResponse response;
		this->done = false;
		request.set_number(number);
		this->stub->async()->Calculate(&context, &request, &response, std::bind(&SqrtClientWithCallback::CalculateDone, this, std::placeholders::_1));

		std::unique_lock<std::mutex> lock(this->mtx);
		this->cv.wait(lock, [this] { return this->done; });

		if (!this->status.ok())
		{
			std::cerr << " - Status.code: " << this->status.error_code() << std::endl;
			std::cerr << " - Status.message: " << this->status.error_message() << std::endl;
			std::cerr << " - Status.details: " << this->status.error_details() << std::endl;
			throw std::runtime_error(this->status.error_message());
		}

		return response.result();
	}

	void CalculateDone(grpc::Status status)
	{
		std::unique_lock<std::mutex> lock(mtx);
		this->status = std::move(status);
		this->done = true;
		this->cv.notify_one();
	}

private:
	std::unique_ptr<math::SqrtService::Stub> stub;
	std::mutex mtx;
	std::condition_variable cv;
	bool done;
	grpc::Status status;
};