#include <grpcpp/grpcpp.h>
#include <proto/sqrt.pb.h>
#include <proto/sqrt.grpc.pb.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <stdexcept>


class SqrtClient {
public:
	explicit SqrtClient(std::shared_ptr<grpc::Channel> channel) : stub(::squareroot::SqrtService::NewStub(channel)) 
	{		
	}

	double Calculate(int number) 
	{
	 	grpc::ClientContext context;
	 	::squareroot::SqrtRequest request;
	 	::squareroot::SqrtResponse response;

		request.set_number(number);
	 	this->stub->async()->Calculate(&context, &request, &response, std::bind(&SqrtClient::CalculateDone, this, std::placeholders::_1));

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


    // grpc::Status Await(double& result)
	// {
    //     std::unique_lock<std::mutex> lock(this->mtx);
    //     this->cv.wait(lock, [this] { return this->done; });
    //     response = std::move(this->response);
    //     return std::move(this->status);
    // }

	void CalculateDone(grpc::Status status)
	{
		std::unique_lock<std::mutex> lock(mtx);
		this->status = std::move(status);
		this->done = true;
		this->cv.notify_one();
	}

private:
	std::unique_ptr<::squareroot::SqrtService::Stub> stub;
	std::mutex mtx;
	std::condition_variable cv;
	bool done = false;
	grpc::Status status;
};

int main(int argc, char** argv) 
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./client <port> <number>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		int number = std::stoi(argv[2]);
		std::string host = absl::StrFormat("localhost:%d", port);
		auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		SqrtClient client(channel);
		double result = client.Calculate(number);	
		std::cout << "Sqrt: " << result << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << '\n';
	}

	return 0;
}