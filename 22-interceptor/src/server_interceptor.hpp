#include <proto/fibonacci.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>


class ServerLogInterceptor : public grpc::experimental::Interceptor
{
public:
	ServerLogInterceptor(grpc::experimental::ServerRpcInfo* info) : info_(info)
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

		if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::POST_RECV_MESSAGE)) 
		{
			// Log time of request reception
			auto now = std::chrono::system_clock::now();
			std::time_t now_time = std::chrono::system_clock::to_time_t(now);
			std::cout << "Request received at: " << std::ctime(&now_time);

			const auto* request = static_cast<const fibonacci::FibonacciRequest*>(methods->GetRecvMessage());
			if (request) 
				std::cout << "Request number received: " << request->number() << std::endl;
		}

		methods->Proceed();
	}

private:
	grpc::experimental::ServerRpcInfo* info_;
};

class ServerLogInterceptorFactory : public grpc::experimental::ServerInterceptorFactoryInterface
{
public:
	grpc::experimental::Interceptor* CreateServerInterceptor(grpc::experimental::ServerRpcInfo* info) override
	{
		return new ServerLogInterceptor(info);
	}
};
