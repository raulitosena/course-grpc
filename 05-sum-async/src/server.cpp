#include <iostream>
#include <thread>
#include <grpcpp/grpcpp.h>
#include <proto/sum.pb.h>
#include <proto/sum.grpc.pb.h>
#include "absl/log/check.h"


// Class encompassing the state and logic needed to serve a request.
class CallData
{
public:
	// Take in the "service" instance (in this case representing an asynchronous server)
	// and the completion queue "cq" used for asynchronous communication with the gRPC runtime.
	CallData(sum::SumService::AsyncService* service, grpc::ServerCompletionQueue* cq)
		: service(service), cq(cq), stream(&ctx), status(CREATE)
	{
		// Invoke the serving logic right away.
		Proceed();
	}

	void Proceed() 
	{
		if (this->status == CREATE)
		{
			// Make this instance progress to the PROCESS state.
			this->status = PROCESS;
			// As part of the initial CREATE state, we *request* that the system
			// start processing SayHello requests. In this request, "this" acts are
			// the tag uniquely identifying the request (so that different CallData
			// instances can serve different requests concurrently), in this case
			// the memory address of this CallData instance.
			this->service->RequestComputeSum(&this->ctx, &this->request, &this->stream, this->cq, this->cq, this);
		}
		else if (this->status == PROCESS)
		{
			// Spawn a new CallData instance to serve new clients while we process the one for
			// this CallData. The instance will deallocate itself as part of its FINISH state.
			new CallData(this->service, this->cq);
			// The actual processing.
			this->response.set_result(this->request.op1() + this->request.op2());

			std::this_thread::sleep_for(std::chrono::seconds(3));
			this->status = FINISH;
			this->stream.Finish(this->response, grpc::Status::OK, this);
		}
		else 
		{
			CHECK_EQ(this->status, FINISH);
			// Once in the FINISH state, deallocate ourselves (CallData).
			delete this;
		}
	}

private:
	// The means of communication with the gRPC runtime for an asynchronous server.
	sum::SumService::AsyncService* service;
	// The producer-consumer queue where for asynchronous server notifications.
	grpc::ServerCompletionQueue* cq;
	// Context for the rpc, allowing to tweak aspects of it such as the use of compression,
	// authentication, as well as to send metadata back to the client.
	grpc::ServerContext ctx;

	// What we get from the client.
	sum::SumOperand request;
	// What we send back to the client.
	sum::SumResult response;

	// The means to get back to the client.
	grpc::ServerAsyncResponseWriter<sum::SumResult> stream;

	// Let's implement a tiny state machine with the following states.
	enum CallStatus { CREATE=1001, PROCESS=2001, FINISH=3001 };
	// The current serving state.
	CallStatus status; 
};

class SumServiceRpc : public sum::SumService::Service
{
public:
	SumServiceRpc(unsigned short port)
	{
		this->host = absl::StrFormat("localhost:%d", port);
	}

	virtual ~SumServiceRpc()
	{
		this->Stop();		
	}

	void Start()
	{
		if (this->server)
			return;

		grpc::ServerBuilder builder;
		builder.AddListeningPort(this->host, grpc::InsecureServerCredentials());
		builder.RegisterService(&this->service);
		this->cq = builder.AddCompletionQueue();
		this->server = builder.BuildAndStart();
		std::cout << "Server running on " << this->host << " ..." << std::endl;
		this->HandleRpcs();
	}

	void Stop()
	{
		if (this->server)
			this->server->Shutdown();
		if (this->cq)
			this->cq->Shutdown();
	}

	// This can be run in multiple threads if needed.
	void HandleRpcs()
	{
		if (!this->server)
			return;

		// Spawn a new CallData instance to serve new clients.
		new CallData(&this->service, this->cq.get());
		void* tag;  // uniquely identifies a request.
		bool ok;
		while (this->cq->Next(&tag, &ok))
		{
			if (!ok)
				break;

			static_cast<CallData*>(tag)->Proceed();
		}
	}

private:
	std::unique_ptr<grpc::ServerCompletionQueue> cq;
	sum::SumService::AsyncService service;
	std::unique_ptr<grpc::Server> server;
	std::string host;
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		SumServiceRpc service(port);
		service.Start();
	} 
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}

	return 0;
}
