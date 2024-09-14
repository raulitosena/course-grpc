#include <iostream>
#include <thread>
#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>


// Class encompassing the state and logic needed to serve a request.
class CallData
{
public:
	// Take in the "service" instance (in this case representing an asynchronous server)
	// and the completion queue "cq" used for asynchronous communication with the gRPC runtime.
	CallData(sum::SumService::AsyncService* service, grpc::ServerCompletionQueue* cq)
		: service_(service), cq_(cq), stream_(&ctx_), status_(CREATE)
	{
		// Invoke the serving logic right away.
		Proceed();
	}
	~CallData()
	{
		//std::cout << "CallData destroyed" <<std::endl;
	}

	void Proceed() 
	{
		if (status_ == CREATE)
		{
			// Make this instance progress to the PROCESS state.
			status_ = PROCESS;
			// As part of the initial CREATE state, we *request* that the system
			// start processing SayHello requests. In this request, "this" acts are
			// the tag uniquely identifying the request (so that different CallData
			// instances can serve different requests concurrently), in this case
			// the memory address of this CallData instance.
			service_->RequestComputeSum(&ctx_, &request_, &stream_, cq_, cq_, this);
		}
		else if (status_ == PROCESS)
		{
			// Spawn a new CallData instance to serve new clients while we process the one for
			// this CallData. The instance will deallocate itself as part of its FINISH state.
			new CallData(service_, cq_);
			// The actual processing.
			response_.set_result(request_.op1() + request_.op2());
			std::cout << " - Calculating... (" << request_.op1() << " + " << request_.op2() << ")" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(10));
			// And we are done! Let the gRPC runtime know we've finished, using the memory
			// address of this instance as the uniquely identifying tag for the event.
			status_ = FINISH;
			stream_.Finish(response_, grpc::Status::OK, this);
		}
		else 
		{
			//CHECK_EQ(status_, FINISH);
			// Once in the FINISH state, deallocate ourselves (CallData).
			delete this;
		}
	}

private:
	// The means of communication with the gRPC runtime for an asynchronous server.
	sum::SumService::AsyncService* service_;
	// The producer-consumer queue where for asynchronous server notifications.
	grpc::ServerCompletionQueue* cq_;
	// Context for the rpc, allowing to tweak aspects of it such as the use of compression,
	// authentication, as well as to send metadata back to the client.
	grpc::ServerContext ctx_;

	// What we get from the client.
	sum::SumOperand request_;
	// What we send back to the client.
	sum::SumResult response_;

	// The means to get back to the client.
	grpc::ServerAsyncResponseWriter<sum::SumResult> stream_;

	// Let's implement a tiny state machine with the following states.
	enum CallStatus { CREATE, PROCESS, FINISH };
	// The current serving state.
	CallStatus status_; 
};

class SumServiceImpl : public sum::SumService::Service
{
public:
	~SumServiceImpl()
	{
		this->Stop();		
	}

	// There is no shutdown handling in this code.
	void Start(uint16_t port)
	{
		std::string server_address = absl::StrFormat("0.0.0.0:%d", port);

		grpc::ServerBuilder builder;
		// Listen on the given address without any authentication mechanism.
		builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
		// Register "service_" as the instance through which we'll communicate with
		// clients. In this case it corresponds to an *asynchronous* service.
		builder.RegisterService(&service_);
		// Get hold of the completion queue used for the asynchronous communication
		// with the gRPC runtime.
		cq_ = builder.AddCompletionQueue();
		// Finally assemble the server.
		server_ = builder.BuildAndStart();
		std::cout << "Server listening on " << server_address << std::endl;

		// Proceed to the server's main loop.
		HandleRpcs();
	}

	void Stop()
	{
		server_->Shutdown();
		// Always shutdown the completion queue after the server.
		cq_->Shutdown();
	}

	// This can be run in multiple threads if needed.
	void HandleRpcs()
	{
		// Spawn a new CallData instance to serve new clients.
		new CallData(&service_, cq_.get());
		void* tag;  // uniquely identifies a request.
		bool ok;
		while (cq_->Next(&tag, &ok))
		{
			if (!ok)
				break;

			static_cast<CallData*>(tag)->Proceed();
		}
		// while (true) {
		// // Block waiting to read the next event from the completion queue. The
		// // event is uniquely identified by its tag, which in this case is the
		// // memory address of a CallData instance.
		// // The return value of Next should always be checked. This return value
		// // tells us whether there is any kind of event or cq_ is shutting down.
		// CHECK(cq_->Next(&tag, &ok));
		// CHECK(ok);
		// static_cast<CallData*>(tag)->Proceed();
	}

	// grpc::Status ComputeSum(grpc::ServerContext* context, const sum::SumOperand* request, sum::SumResult* response) override
	// {
	// 	float result = request->op1() + request->op2();
	// 	response->set_result(result);

	// 	std::this_thread::sleep_for(std::chrono::seconds(3));

	// 	return grpc::Status::OK;
	// }

private:
	std::unique_ptr<grpc::ServerCompletionQueue> cq_;
	sum::SumService::AsyncService service_;
	std::unique_ptr<grpc::Server> server_;
};

// int main(int argc, char** argv)
// {
// 	if (argc != 2)
// 	{
// 		std::cerr << "Missing parameters!" << std::endl;
// 		return 1001;
// 	}

// 	std::string host = argv[1];

// 	SumServiceImpl service;
// 	grpc::ServerBuilder builder;
// 	builder.AddListeningPort(host, grpc::InsecureServerCredentials());
// 	builder.RegisterService(&service);
// 	auto cq = builder.AddCompletionQueue();
// 	auto server(builder.BuildAndStart());

// 	std::cout << "Sum server running on " << host << " ..." << std::endl;

// 	server->Wait();

// 	return 0;
// }



int main(int argc, char** argv)
{
	SumServiceImpl server;
	std::cout << "Started!" << std::endl;

	server.Start(5000);
	std::cout << "Running..." << std::endl;

	server.HandleRpcs();

	std::cout << "Exit!" << std::endl;

	return 0;
}
