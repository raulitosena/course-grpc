#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>
#include <iostream>
#include <condition_variable>
#include <mutex>


class SumClient {
public:
	explicit SumClient(std::shared_ptr<grpc::Channel> channel) : stub(sum::SumService::NewStub(channel)) 
	{		
	}

	int ComputeSum(int op1, int op2) 
	{
		grpc::ClientContext context;
		sum::SumOperand request;
		sum::SumResult response;
		request.set_op1(op1);
		request.set_op2(op2);
		this->stub->async()->ComputeSum(&context, &request, &response, std::bind(&SumClient::ComputeSumDone, this, std::placeholders::_1));

		std::unique_lock<std::mutex> lock(this->mtx);
		while (!done) 
		{
			this->cv.wait(lock);
		}

		if (this->status.ok()) 
		{
			return response.result();
		} 
		else 
		{
			throw std::runtime_error(this->status.error_message());
		}
	}

	void ComputeSumDone(grpc::Status status)
	{
		this->status = std::move(status);
		std::unique_lock<std::mutex> lock(mtx);
		this->done = true;
		this->cv.notify_one();
	}

private:
	std::unique_ptr<sum::SumService::Stub> stub;
	std::mutex mtx;
	std::condition_variable cv;
	bool done = false;
	grpc::Status status;
};

int main(int argc, char** argv) 
{
	if (argc != 4)
	{
		std::cerr << "Usage: ./client <port> <op1> <op2>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		int op1 = std::stoi(argv[2]);
		int op2 = std::stoi(argv[3]);
		std::string host = absl::StrFormat("localhost:%d", port);

		auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		SumClient client(channel);
		int result = client.ComputeSum(op1, op2);	
		std::cout << op1 << " + " << op2 << " = " << result << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}