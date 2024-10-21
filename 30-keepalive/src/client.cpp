#include <proto/sum.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <thread>


class SumClientReactor : public grpc::ClientUnaryReactor
{
public:
	SumClientReactor(sum::SumService::Stub* stub, int op1, int op2)
		: done(false)
	{
		this->request.set_op1(op1);
		this->request.set_op2(op2);
		stub->async()->ComputeSum(&this->context, &this->request, &this->response, this);
		this->StartCall();
	}

	void OnDone(const grpc::Status& status) override
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->status = status;
		this->done = true;
		this->cv.notify_one();
	}

	grpc::Status Await(int& result)
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->cv.wait(lock, [this] { return this->done; });
		result = this->response.result();
		return this->status;
	}

private:
	grpc::ClientContext context;
	sum::SumResult response;
	sum::SumOperand request;
	std::mutex mtx;
	std::condition_variable cv;
	grpc::Status status;
	bool done;
};

class SumClient
{
public:
	explicit SumClient(std::shared_ptr<grpc::Channel> channel) 
		: stub(sum::SumService::NewStub(channel))
	{
	}

	int ComputeSum(int op1, int op2)
	{
		SumClientReactor reactor(this->stub.get(), op1, op2);
		int result;
		grpc::Status status = reactor.Await(result);
		if (!status.ok())
			throw std::runtime_error(status.error_message());
		
		return result;
	}

private:
	std::unique_ptr<sum::SumService::Stub> stub;
};

int main(int argc, char** argv) 
{
	if (argc != 4)
	{
		std::cerr << "Usage: ./client <host:port> <op1> <op2>" << std::endl;
		return 1001;
	}

	std::string host = absl::StrFormat("%s", argv[1]);
	int op1 = std::stoi(argv[2]);
	int op2 = std::stoi(argv[3]);

	std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
	SumClient client(channel);

	std::thread client_thread([&client, op1, op2]()
	{
		std::cout << "Commands: c=calculate, q=quit" << std::endl;
		std::string command;
		
		do
		{
			std::cout << "> " << std::flush;
			std::cin >> command;

			if (command == "c")
			{
				int result = client.ComputeSum(op1, op2);
				std::cout << op1 << " + " << op2 << " = " << result << std::endl;
			}

		}
		while (command != "q");

	});

	client_thread.join();

	return 0;
}