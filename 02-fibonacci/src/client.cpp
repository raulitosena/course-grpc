#include <grpcpp/grpcpp.h>
#include <proto/fibonacci.grpc.pb.h>
#include <proto/fibonacci.pb.h>


class FibonacciClient {
public:
	FibonacciClient(std::shared_ptr<grpc::Channel> channel) : stub_(::fibonacci::FibonacciServices::NewStub(channel))
	{		
	}

	unsigned int GetFibonacciSequence(unsigned int num) 
	{
	 	// ::fibonacci::FibonacciRequest request;		
	 	// request.set_number(num);

	 	// ::fibonacci::FibonacciResponse response;
	 	// grpc::ClientContext context;
		// grpc::Status status ;
		// //stub_->GetFibonacciSequence(&context, request);
		// //stub_->GetFibonacciSequence(&context, request, &response);

		// if (status.ok()) 
		// {
		// 	return response.number();
		// }
		// else 
		// {
		// 	std::cerr << "RPC failed" << std::endl;
		// 	return 0;
		// }
	}

private:
	std::unique_ptr<::fibonacci::FibonacciServices::Stub> stub_;
};


int main(int argc, char** argv) 
{
	auto channel = grpc::CreateChannel("localhost:5000", grpc::InsecureChannelCredentials());
	FibonacciClient client(channel);
	unsigned int number = 4;
	auto response = client.GetFibonacciSequence(number);	
	std::cout << "From Fibonacci server: " << response << std::endl;

	return 0;
}