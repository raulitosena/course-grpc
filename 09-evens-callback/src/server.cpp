#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/evens.grpc.pb.h>
#include <proto/evens.pb.h>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>


class EvensServerBidiReactor : public grpc::ServerBidiReactor<evens::Number, evens::Number>
{
public:
	void OnReadDone(bool ok) override
	{
		if (ok)
		{
			// Enqueue the received number
			std::unique_lock<std::mutex> lock(mtx);
			if (this->incoming.value() % 2 == 0)
			{
				this->buffer.push(this->incoming);
			}
			// Request next number
			this->StartRead(&this->incoming);
		}
		else
		{
			this->Finish(grpc::Status::OK);
		}
	}

	void OnWriteDone(bool ok) override
	{
		if (ok && !this->buffer.empty())
		{
			std::unique_lock<std::mutex> lock(mtx);
			this->outgoing = this->buffer.front();
			this->buffer.pop();
			this->StartWrite(&this->outgoing);
		}
		else if (this->buffer.empty())
		{
			// Finished writing evens
			Finish(grpc::Status::OK);
		}
	}

	void OnDone() override
	{
		delete this;
	}

	void StartProcessing()
	{
		this->StartRead(&this->incoming);
	}

private:
	evens::Number incoming;
	evens::Number outgoing;
	std::queue<evens::Number> buffer;
	std::mutex mtx;
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
		EvensServerBidiReactor* reactor = new EvensServerBidiReactor();
		reactor->StartProcessing();
		return reactor;
	}

	void Start() 
	{
		grpc::ServerBuilder builder;
		builder.AddListeningPort(this->host, grpc::InsecureServerCredentials());
		builder.RegisterService(this);
		this->server = builder.BuildAndStart();
		if (this->server)
			std::cout << "Server listening on " << this->host << std::endl;
	}

	void Stop()
	{
		if (not this->server)
			return;
		this->server->Shutdown();
	}

private:
	std::unique_ptr<grpc::Server> server;
	std::string host;
};


int main(int argc, char** argv)
{
	// if (argc != 2)
	// {
	// 	std::cerr << "Usage: ./server <port>" << std::endl;
	// 	return 1001;
	// }

	try
	{
		//int port = std::stoi(argv[1]);
		int port = 5000;
		EvensServiceRpc service(port);
		service.Start();

		std::this_thread::sleep_for(std::chrono::seconds(600));

		service.Stop();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1002;
	}

	return 0;
}




