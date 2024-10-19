#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>
#include <thread>
#include "absl/log/check.h"


// struct for keeping state and data information
struct AsyncClientCall
{
	void * Id()
	{
		return this;
	}

	// Container for the data we expect from the server.
	sum::SumResult reply;

	// Context for the client. It could be used to convey extra information to
	// the server and/or tweak certain RPC behaviors.
	grpc::ClientContext context;

	// Storage for the status of the RPC upon completion.
	grpc::Status status;

	std::unique_ptr<grpc::ClientAsyncResponseReader<sum::SumResult>> rpc;
};


class SumClient {
public:
	explicit SumClient(std::shared_ptr<grpc::Channel> channel)
		: stub_(sum::SumService::NewStub(channel))
	{
	}

	void ComputeSum(int op1, int op2)
	{
		sum::SumOperand request;
		request.set_op1(op1);
		request.set_op2(op2);

		// Call object to store rpc data
		AsyncClientCall* call = new AsyncClientCall();
		// an instance to store in "call" but does not actually start the RPC
		// Because we are using the asynchronous API, we need to hold on to
		// the "call" instance in order to get updates on the ongoing RPC.
		call->rpc = stub_->PrepareAsyncComputeSum(&call->context, request, &cq_);
		// StartCall initiates the RPC call
		call->rpc->StartCall();
		// Request that, upon completion of the RPC, "reply" be updated with the
		// server's response; "status" with the indication of whether the operation
		// was successful. Tag the request with the memory address of the call
		// object.
		call->rpc->Finish(&call->reply, &call->status, call->Id());
	}

// Loop while listening for completed responses.
// Prints out the response from the server.
void AsyncCompleteRpc() 
{
	void* got_tag;
	bool ok = false;

	// Block until the next result is available in the completion queue "cq".
	while (cq_.Next(&got_tag, &ok))
	{
		// The tag in this example is the memory location of the call object
		AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

		// Verify that the request was completed successfully. Note that "ok"
		// corresponds solely to the request for updates introduced by Finish().
		CHECK(ok);

		std::cout << "Response (thread ID): " << std::this_thread::get_id() << std::endl;

		if (call->status.ok())
			std::cout << "SUM: " << call->reply.result() << std::endl;
		else
			std::cout << "RPC failed: " << call->status.error_message() << std::endl;

		// Once we're complete, deallocate the call object.
		delete call;
	}
}

void Shutdown()
{
	cq_.Shutdown();  // Shut down the completion queue to stop the loop
}

private:
	// Out of the passed in Channel comes the stub, stored here, our view of the server's exposed services.
	std::unique_ptr<sum::SumService::Stub> stub_;
	// The producer-consumer queue we use to communicate asynchronously with the gRPC runtime.
	grpc::CompletionQueue cq_;
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

		std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
		SumClient client(channel);

		std::thread thread_ = std::thread(&SumClient::AsyncCompleteRpc, &client);

		std::cout << "Request  (thread ID): " << std::this_thread::get_id() << std::endl;
		client.ComputeSum(op1, op2);

		client.Shutdown(); 		

		thread_.join();
	}
	catch(const std::exception& e)
	{	
		std::cerr << e.what() << '\n';
	}

	return 0;
}