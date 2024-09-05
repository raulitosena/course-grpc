// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: math.proto
#ifndef GRPC_math_2eproto__INCLUDED
#define GRPC_math_2eproto__INCLUDED

#include "math.pb.h"

#include <functional>
#include <grpcpp/generic/async_generic_service.h>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/proto_utils.h>
#include <grpcpp/impl/rpc_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/status.h>
#include <grpcpp/support/stub_options.h>
#include <grpcpp/support/sync_stream.h>

namespace math {

class MathServices final {
 public:
  static constexpr char const* service_full_name() {
    return "math.MathServices";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status computeSum(::grpc::ClientContext* context, const ::math::SumOperand& request, ::math::SumResult* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::math::SumResult>> AsynccomputeSum(::grpc::ClientContext* context, const ::math::SumOperand& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::math::SumResult>>(AsynccomputeSumRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::math::SumResult>> PrepareAsynccomputeSum(::grpc::ClientContext* context, const ::math::SumOperand& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::math::SumResult>>(PrepareAsynccomputeSumRaw(context, request, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      virtual void computeSum(::grpc::ClientContext* context, const ::math::SumOperand* request, ::math::SumResult* response, std::function<void(::grpc::Status)>) = 0;
      virtual void computeSum(::grpc::ClientContext* context, const ::math::SumOperand* request, ::math::SumResult* response, ::grpc::ClientUnaryReactor* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::math::SumResult>* AsynccomputeSumRaw(::grpc::ClientContext* context, const ::math::SumOperand& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::math::SumResult>* PrepareAsynccomputeSumRaw(::grpc::ClientContext* context, const ::math::SumOperand& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    ::grpc::Status computeSum(::grpc::ClientContext* context, const ::math::SumOperand& request, ::math::SumResult* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::math::SumResult>> AsynccomputeSum(::grpc::ClientContext* context, const ::math::SumOperand& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::math::SumResult>>(AsynccomputeSumRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::math::SumResult>> PrepareAsynccomputeSum(::grpc::ClientContext* context, const ::math::SumOperand& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::math::SumResult>>(PrepareAsynccomputeSumRaw(context, request, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void computeSum(::grpc::ClientContext* context, const ::math::SumOperand* request, ::math::SumResult* response, std::function<void(::grpc::Status)>) override;
      void computeSum(::grpc::ClientContext* context, const ::math::SumOperand* request, ::math::SumResult* response, ::grpc::ClientUnaryReactor* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::math::SumResult>* AsynccomputeSumRaw(::grpc::ClientContext* context, const ::math::SumOperand& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::math::SumResult>* PrepareAsynccomputeSumRaw(::grpc::ClientContext* context, const ::math::SumOperand& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_computeSum_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status computeSum(::grpc::ServerContext* context, const ::math::SumOperand* request, ::math::SumResult* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_computeSum : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_computeSum() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_computeSum() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status computeSum(::grpc::ServerContext* /*context*/, const ::math::SumOperand* /*request*/, ::math::SumResult* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestcomputeSum(::grpc::ServerContext* context, ::math::SumOperand* request, ::grpc::ServerAsyncResponseWriter< ::math::SumResult>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_computeSum<Service > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_computeSum : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_computeSum() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::math::SumOperand, ::math::SumResult>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::math::SumOperand* request, ::math::SumResult* response) { return this->computeSum(context, request, response); }));}
    void SetMessageAllocatorFor_computeSum(
        ::grpc::MessageAllocator< ::math::SumOperand, ::math::SumResult>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(0);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::math::SumOperand, ::math::SumResult>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_computeSum() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status computeSum(::grpc::ServerContext* /*context*/, const ::math::SumOperand* /*request*/, ::math::SumResult* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* computeSum(
      ::grpc::CallbackServerContext* /*context*/, const ::math::SumOperand* /*request*/, ::math::SumResult* /*response*/)  { return nullptr; }
  };
  typedef WithCallbackMethod_computeSum<Service > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_computeSum : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_computeSum() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_computeSum() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status computeSum(::grpc::ServerContext* /*context*/, const ::math::SumOperand* /*request*/, ::math::SumResult* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_computeSum : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_computeSum() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_computeSum() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status computeSum(::grpc::ServerContext* /*context*/, const ::math::SumOperand* /*request*/, ::math::SumResult* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestcomputeSum(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_computeSum : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_computeSum() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->computeSum(context, request, response); }));
    }
    ~WithRawCallbackMethod_computeSum() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status computeSum(::grpc::ServerContext* /*context*/, const ::math::SumOperand* /*request*/, ::math::SumResult* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* computeSum(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_computeSum : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_computeSum() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler<
          ::math::SumOperand, ::math::SumResult>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::math::SumOperand, ::math::SumResult>* streamer) {
                       return this->StreamedcomputeSum(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_computeSum() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status computeSum(::grpc::ServerContext* /*context*/, const ::math::SumOperand* /*request*/, ::math::SumResult* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedcomputeSum(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::math::SumOperand,::math::SumResult>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_computeSum<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_computeSum<Service > StreamedService;
};

}  // namespace math


#endif  // GRPC_math_2eproto__INCLUDED
