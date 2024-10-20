#include <proto/checksum.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <thread>


class FileChecksumClientReactor : public grpc::ClientUnaryReactor
{
public:
	FileChecksumClientReactor(checksum::FileChecksumService::Stub* stub, const std::string& file_path)
		: done(false)
	{
		std::ifstream file(file_path, std::ios::binary);
		if (!file)
		{
			throw std::runtime_error("Failed to open file: " + file_path);
		}

		std::vector<char> file_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		this->request.set_file_data(file_data.data(), file_data.size());

		stub->async()->UploadFile(&this->context, &this->request, &this->response, this);
		this->StartCall();
	}

	void OnDone(const grpc::Status& status) override
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->status = status;
		this->done = true;
		this->cv.notify_one();
	}

	grpc::Status Await(std::string& checksum)
	{
		std::unique_lock<std::mutex> lock(this->mtx);
		this->cv.wait(lock, [this] { return this->done; });
		checksum = this->response.checksum();
		return this->status;
	}

private:
	grpc::ClientContext context;
	checksum::FileUploadRequest request;
	checksum::FileUploadResponse response;
	std::mutex mtx;
	std::condition_variable cv;
	grpc::Status status;
	bool done;
};

class FileChecksumClient
{
public:
	explicit FileChecksumClient(std::shared_ptr<grpc::Channel> channel) 
		: stub(checksum::FileChecksumService::NewStub(channel))
	{
	}

	std::string GetFileChecksum(const std::string& file_path)
	{
		FileChecksumClientReactor reactor(this->stub.get(), file_path);
		std::string checksum;
		grpc::Status status = reactor.Await(checksum);
		if (!status.ok())
			throw std::runtime_error(status.error_message());
		
		return checksum;
	}

private:
	std::unique_ptr<checksum::FileChecksumService::Stub> stub;
};

int main(int argc, char** argv) 
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./client <host:port> <file_path>" << std::endl;
		return 1001;
	}

	std::string host = argv[1];
	std::string file_path = argv[2];

	grpc::ChannelArguments args;
	args.SetCompressionAlgorithm(GRPC_COMPRESS_GZIP);
	std::shared_ptr<grpc::Channel> channel = grpc::CreateCustomChannel(host, grpc::InsecureChannelCredentials(), args);
	FileChecksumClient client(channel);

	std::thread client_thread([&client, &file_path]()
	{
		std::cout << "Commands: c=calculate, q=quit" << std::endl;
		std::string command;
		
		do
		{
			std::cout << "> " << std::flush;
			std::cin >> command;

			if (command == "c")
			{
				try
				{
					std::string checksum = client.GetFileChecksum(file_path);
					std::cout << "SHA-256 checksum: " << checksum << std::endl;
				}
				catch(const std::exception& e)
				{
					std::cerr << "Error: " << e.what() << std::endl;
				}
			}

		}
		while (command != "q");

	});

	client_thread.join();

	return 0;
}
