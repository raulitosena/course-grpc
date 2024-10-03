#include <proto/sum.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>


constexpr std::string_view kRetryPolicy =
	"{\"methodConfig\" : [{"
	"   \"name\" : [{\"service\": \"helloworld.Greeter\"}],"
	"   \"waitForReady\": true,"
	"   \"retryPolicy\": {"
	"     \"maxAttempts\": 4,"
	"     \"initialBackoff\": \"1s\","
	"     \"maxBackoff\": \"120s\","
	"     \"backoffMultiplier\": 1.0,"
	"     \"retryableStatusCodes\": [\"UNAVAILABLE\"]"
	"    }"
	"}]}";

class SumClientReactor : public grpc::ClientUnaryReactor
{
public:
	SumClientReactor(::sum::SumService::Stub* stub, int op1, int op2)
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
	::sum::SumResult response;
	::sum::SumOperand request;
	std::mutex mtx;
	std::condition_variable cv;
	grpc::Status status;
	bool done;
};

class SumClient
{
public:
	explicit SumClient(std::shared_ptr<grpc::Channel> channel) 
		: stub(::sum::SumService::NewStub(channel))
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
		std::cerr << "Usage: ./client <port> <op1> <op2>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		int op1 = std::stoi(argv[2]);
		int op2 = std::stoi(argv[3]);
		std::string host = absl::StrFormat("localhost:%d", port);
		auto channel_args = grpc::ChannelArguments();
		channel_args.SetServiceConfigJSON(std::string(kRetryPolicy));
		auto channel = grpc::CreateCustomChannel(host, grpc::InsecureChannelCredentials(), channel_args);
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