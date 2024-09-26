#include <proto/fibonacci.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <map>


std::map<uint64_t, ::fibonacci::FibonacciListResponse> cache_;

class CachingFiboInterceptor : public grpc::experimental::Interceptor
{
public:
	CachingFiboInterceptor(grpc::experimental::ClientRpcInfo* info)
	{
	}

	void Intercept(grpc::experimental::InterceptorBatchMethods* methods) override
	{
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA)) { std::cout << "PRE_SEND_INITIAL_METADATA" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_SEND_MESSAGE)) { std::cout << "PRE_SEND_MESSAGE" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::POST_SEND_MESSAGE)) { std::cout << "POST_SEND_MESSAGE" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_SEND_STATUS)) { std::cout << "PRE_SEND_STATUS" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_SEND_CLOSE)) { std::cout << "PRE_SEND_CLOSE" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_RECV_INITIAL_METADATA)) { std::cout << "PRE_RECV_INITIAL_METADATA" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_RECV_MESSAGE)) { std::cout << "PRE_RECV_MESSAGE" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_RECV_STATUS)) { std::cout << "PRE_RECV_STATUS" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::POST_RECV_INITIAL_METADATA)) { std::cout << "POST_RECV_INITIAL_METADATA" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::POST_RECV_MESSAGE)) { std::cout << "POST_RECV_MESSAGE" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::POST_RECV_STATUS)) { std::cout << "POST_RECV_STATUS" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::POST_RECV_CLOSE)) { std::cout << "POST_RECV_CLOSE" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_SEND_CANCEL)) { std::cout << "PRE_SEND_CANCEL" << std::endl; };
		// if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::NUM_INTERCEPTION_HOOKS)) { std::cout << "NUM_INTERCEPTION_HOOKS" << std::endl; };

		bool hijack = false;
		// The hijack flag is set when the interceptor decides to stop the gRPC request 
		// from reaching the server and instead use the cached response. If the interceptor
		// doesn't hijack the message, the request proceeds to the server as usual.

		if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA))
		{
			hijack = true;
			stub_ = ::fibonacci::FibonacciSlowService::NewStub(methods->GetInterceptedChannel());
		}

		if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_SEND_MESSAGE))
		{
			const ::fibonacci::FibonacciRequest* req_msg = static_cast<const ::fibonacci::FibonacciRequest*>(methods->GetSendMessage());

			if (req_msg)
			{
				uint64_t requested_number = req_msg->number();
				auto search = cache_.find(requested_number);

				if (search != cache_.end())
				{
					// Cached response found, hijack the request
					std::cout << "Fibonacci for " << requested_number << " found in cache." << std::endl;
					response_ = search->second;
				}
				else
				{
					// Cache miss, forward request to server
					std::cout << "Fibonacci for " << requested_number << " not found in cache. Sending request to server." << std::endl;
					::fibonacci::FibonacciListResponse resp;
					grpc::ClientContext context;
					grpc::Status status = stub_->GetFibonacciList(&context, *req_msg, &resp);

					if (status.ok())
					{
						response_ = resp;
						cache_[requested_number] = resp;
					}
					else
					{
						std::cerr << "Failed to get response from server: " << status.error_message() << std::endl;
					}
				}
			}
		}

		if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_RECV_MESSAGE))
		{
			::fibonacci::FibonacciListResponse* resp_msg = static_cast<::fibonacci::FibonacciListResponse*>(methods->GetRecvMessage());
			*resp_msg = response_;
		}

		if (hijack)
		{
			methods->Hijack();
		}
		else
		{
			methods->Proceed();
		}
	}

private:
	grpc::ClientContext context_;
	std::unique_ptr<::fibonacci::FibonacciSlowService::Stub> stub_;
	::fibonacci::FibonacciListResponse response_;	
};

class CachingFiboInterceptorFactory : public grpc::experimental::ClientInterceptorFactoryInterface 
{
public:
	grpc::experimental::Interceptor* CreateClientInterceptor(grpc::experimental::ClientRpcInfo* info) override 
	{
		return new CachingFiboInterceptor(info);
	}
};
