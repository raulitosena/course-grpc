#include <proto/fibonacci.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>


class ClientLogInterceptor : public grpc::experimental::Interceptor 
{
public:
	ClientLogInterceptor(grpc::experimental::ClientRpcInfo* info) 
	{
	}

	void Intercept(::grpc::experimental::InterceptorBatchMethods* methods) override
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

		if (methods->QueryInterceptionHookPoint(grpc::experimental::InterceptionHookPoints::PRE_SEND_MESSAGE)) 
		{
			const auto* request = static_cast<const ::fibonacci::FibonacciRequest*>(methods->GetSendMessage());

			if (request)
				std::cout << "Request sent: " << request->number() << std::endl;
		}

		methods->Proceed();
	}

private:
	grpc::experimental::ClientRpcInfo* info_;
};

class ClientLogInterceptorFactory : public grpc::experimental::ClientInterceptorFactoryInterface
{
public:
  	grpc::experimental::Interceptor* CreateClientInterceptor(grpc::experimental::ClientRpcInfo* info) override 
	{
	    return new ClientLogInterceptor(info);
  	}
};