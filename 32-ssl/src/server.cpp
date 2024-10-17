#include <iostream>
#include <grpcpp/grpcpp.h>
#include <proto/checksum.grpc.pb.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <thread>
#include <vector>
#include <sstream>
#include <iomanip>
#include <fstream>


std::string ReadFile(std::string filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error("Failed to open file " + filename);

	std::stringstream sstr;
	sstr << file.rdbuf();
	return sstr.str();
}

std::string CalculateSHA256(const std::vector<uint8_t>& data)
{
	EVP_MD_CTX* ctx = EVP_MD_CTX_new();
	if (ctx == nullptr)
	{
		throw std::runtime_error("Failed to create EVP_MD_CTX");
	}

	if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1)
	{
		EVP_MD_CTX_free(ctx);
		throw std::runtime_error("EVP_DigestInit_ex failed");
	}

	if (EVP_DigestUpdate(ctx, data.data(), data.size()) != 1)
	{
		EVP_MD_CTX_free(ctx);
		throw std::runtime_error("EVP_DigestUpdate failed");
	}

	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned int lengthOfHash = 0;
	if (EVP_DigestFinal_ex(ctx, hash, &lengthOfHash) != 1)
	{
		EVP_MD_CTX_free(ctx);
		throw std::runtime_error("EVP_DigestFinal_ex failed");
	}

	EVP_MD_CTX_free(ctx);

	std::stringstream ss;
	for (unsigned int i = 0; i < lengthOfHash; ++i)
	{
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
	}

	return ss.str();
}

class FileChecksumServiceImpl : public checksum::FileChecksumService::CallbackService
{
public:
	grpc::ServerUnaryReactor* UploadFile(grpc::CallbackServerContext* context, const checksum::FileUploadRequest* request, checksum::FileUploadResponse* response) override
	{
		grpc::ServerUnaryReactor* reactor = context->DefaultReactor();
		std::vector<uint8_t> file_data(request->file_data().begin(), request->file_data().end());
		std::string checksum = CalculateSHA256(file_data);
		response->set_checksum(checksum);
		reactor->Finish(grpc::Status::OK);
		return reactor;
	}
};

class FileChecksumServer
{
public:
	FileChecksumServer(unsigned short port)
	{
		this->host = absl::StrFormat("localhost:%d", port);
	}

	virtual ~FileChecksumServer()
	{
		this->Stop();
	}

	void Start()
	{
		if (this->server)
			return;


		grpc::SslServerCredentialsOptions::PemKeyCertPair key_cert_pair = {
			ReadFile("server.key"),  // Server's private key
			ReadFile("server.crt")   // Server's certificate
		};

		grpc::SslServerCredentialsOptions ssl_opts;
		ssl_opts.pem_root_certs = ReadFile("root.crt");  // Root CA certificate
		ssl_opts.pem_key_cert_pairs.push_back(key_cert_pair);

		this->builder.AddListeningPort(this->host, grpc::SslServerCredentials(ssl_opts));
		this->builder.RegisterService(&this->service);

		// grpc::SslServerCredentialsOptions::PemKeyCertPair server_pair;]
		// server_pair.cert_chain = ReadFile("credentials/localhost.crt");
		// server_pair.private_key = ReadFile("credentials/localhost.key");
		// grpc::SslServerCredentialsOptions ssl_options;
		// ssl_options.pem_key_cert_pairs.emplace_back(server_pair);

		// this->builder.AddListeningPort(this->host, grpc::SslServerCredentials(ssl_options));


		this->server = this->builder.BuildAndStart();
		std::cout << "Server running on " << this->host << " ..." << std::endl;
	}

	void Stop()
	{
		if (!this->server)
			return;

		this->server->Shutdown();
	}

private:
	std::string host;
	grpc::ServerBuilder builder;
	FileChecksumServiceImpl service;
	std::unique_ptr<grpc::Server> server;
};

int main(int argc, char** argv)
{
	std::unique_ptr<FileChecksumServer> server;

	if (argc != 2)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1001;
	}

	try
	{
		int port = std::stoi(argv[1]);
		server = std::make_unique<FileChecksumServer>(port);
		server->Start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1002;
	}

	std::thread server_thread([&server]()
	{
		std::cout << "Commands: q=quit" << std::endl;
		std::string command;
		do
		{
			std::cout << "> " << std::flush;
			std::cin >> command;
		}
		while (command != "q");

		server->Stop();        
	});

	server_thread.join();
	
	return 0;
}
