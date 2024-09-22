#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/evens.grpc.pb.h>
#include <proto/evens.pb.h>
#include <queue>


// Note: Because these methods are not invoked concurrently for the same reactor instance,
// the operations on the buffer happen in a serialized manner for a given RPC. Therefore, 
// explicit synchronization like using std::mutex or std::lock_guard is not required.
// In other words, In this case, you don’t need to synchronize access to the buffer, as gRPC
// ensures that the callbacks for the same RPC reactor (e.g., OnReadDone(), OnWriteDone(), etc.) 
// are executed one at a time.

class EvensServerBidiReactor : public grpc::ServerBidiReactor<evens::Number, evens::Number>
{
public:
	EvensServerBidiReactor()
	{
		this->NextCicle();
	}
	
	void OnReadDone(bool ok) override
	{
		if (ok)
		{
			if (this->incoming.value() % 2 == 0)
			{
				this->buffer.push(this->incoming);
			}

			this->NextCicle();
		}
		else
		{
			this->Finish(grpc::Status::OK);
		}
	}

	void OnWriteDone(bool ok) override
	{
		if (ok)
		{
			this->buffer.pop();
			this->NextCicle();
		}
		else
		{
			Finish(grpc::Status::CANCELLED);
		}
	}

	void OnDone() override
	{
		delete this;
	}

	void OnCancel() override
	{
		std::cout << "RPC cancelled!" << std::endl;
	}

private:
	void NextCicle()
	{
		if (!this->buffer.empty())
		{
			this->StartWrite(&this->buffer.front());
		}
		else
		{
			this->StartRead(&this->incoming);
		}
	}

private:
	evens::Number incoming;
	std::queue<evens::Number> buffer;
};

class EvensServiceRpc : public evens::EvensService::CallbackService
{
public:
	EvensServiceRpc(unsigned short port)
	{
		this->host = absl::StrFormat("localhost:%d", port);
	}

	grpc::ServerBidiReactor<evens::Number, evens::Number>* FindEvens(grpc::CallbackServerContext* context) override
	{
		return new EvensServerBidiReactor();
	}

	void Run() 
	{
		grpc::ServerBuilder builder;
		builder.AddListeningPort(this->host, grpc::InsecureServerCredentials());
		builder.RegisterService(this);
		this->server = builder.BuildAndStart();
		if (this->server)
		{
			std::cout << "Server listening on " << this->host << std::endl;
			server->Wait();
		}
	}

private:
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
		EvensServiceRpc service(port);
		service.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}

	return 0;
}
