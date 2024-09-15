#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>
#include <thread>
#include "absl/log/check.h"


class SumClient
{
public:
	explicit SumClient(std::shared_ptr<grpc::Channel> channel) : stub(sum::SumService::NewStub(channel))
	{
	}

	void ComputeSum(int op1, int op2)
	{
		grpc::ClientContext context;
		sum::SumOperand request;
		sum::SumResult response;

		request.set_op1(op1);
		request.set_op2(op2);

		// The producer-consumer queue we use to communicate asynchronously with the gRPC runtime.
		grpc::CompletionQueue cq;
		// Storage for the status of the RPC upon completion.
   		grpc::Status status;

    	std::unique_ptr<grpc::ClientAsyncResponseReader<sum::SumResult>> rpc(this->stub->AsyncComputeSum(&context, request, &cq));

		void* tag_id = (void*) 0x12345;
		// Request that, upon completion of the RPC, "reply" be updated with the
		// server's response; "status" with the indication of whether the operation
		// was successful. Tag the request with the integer 0x12345.
		rpc->Finish(&response, &status, tag_id);
		void* got_tag;
		bool ok = false;
		// Block until the next result is available in the completion queue "cq".
		// The return value of Next should always be checked. This return value
		// tells us whether there is any kind of event or the cq_ is shutting down.
		CHECK(cq.Next(&got_tag, &ok));
		// Verify that the result from "cq" corresponds, by its tag, our previous
		// request.
		CHECK_EQ(got_tag, tag_id);
		// ... and that the request was completed successfully. Note that "ok"
		// corresponds solely to the request for updates introduced by Finish().
		CHECK(ok);

		std::cout << "Response (thread ID): " << std::this_thread::get_id() << std::endl;

		if (status.ok())
			std::cout << "SUM: " << response.result() << std::endl;
		else
			throw std::runtime_error("RPC failed: " + status.error_message());
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
	
	try
	{
		std::string host = argv[1];
		int op1 = std::stoi(argv[2]);
		int op2 = std::stoi(argv[3]);

		auto channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		SumClient client(channel);

		std::cout << "Request  (thread ID): " << std::this_thread::get_id() << std::endl;
		client.ComputeSum(op1, op2);	
	}
	catch(const std::exception& e)
	{	
		std::cerr << e.what() << '\n';
	}
	
	return 0;
}