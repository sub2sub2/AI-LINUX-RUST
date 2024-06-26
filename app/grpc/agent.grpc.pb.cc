// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: agent.proto

#include "agent.pb.h"
#include "agent.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace agent {

static const char* IrisInference_method_names[] = {
  "/agent.IrisInference/Inference",
};

std::unique_ptr< IrisInference::Stub> IrisInference::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< IrisInference::Stub> stub(new IrisInference::Stub(channel, options));
  return stub;
}

IrisInference::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_Inference_(IrisInference_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status IrisInference::Stub::Inference(::grpc::ClientContext* context, const ::agent::IrisInferenceRequest& request, ::agent::IrisInferenceResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::agent::IrisInferenceRequest, ::agent::IrisInferenceResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Inference_, context, request, response);
}

void IrisInference::Stub::async::Inference(::grpc::ClientContext* context, const ::agent::IrisInferenceRequest* request, ::agent::IrisInferenceResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::agent::IrisInferenceRequest, ::agent::IrisInferenceResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Inference_, context, request, response, std::move(f));
}

void IrisInference::Stub::async::Inference(::grpc::ClientContext* context, const ::agent::IrisInferenceRequest* request, ::agent::IrisInferenceResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Inference_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::agent::IrisInferenceResponse>* IrisInference::Stub::PrepareAsyncInferenceRaw(::grpc::ClientContext* context, const ::agent::IrisInferenceRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::agent::IrisInferenceResponse, ::agent::IrisInferenceRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Inference_, context, request);
}

::grpc::ClientAsyncResponseReader< ::agent::IrisInferenceResponse>* IrisInference::Stub::AsyncInferenceRaw(::grpc::ClientContext* context, const ::agent::IrisInferenceRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncInferenceRaw(context, request, cq);
  result->StartCall();
  return result;
}

IrisInference::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      IrisInference_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< IrisInference::Service, ::agent::IrisInferenceRequest, ::agent::IrisInferenceResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](IrisInference::Service* service,
             ::grpc::ServerContext* ctx,
             const ::agent::IrisInferenceRequest* req,
             ::agent::IrisInferenceResponse* resp) {
               return service->Inference(ctx, req, resp);
             }, this)));
}

IrisInference::Service::~Service() {
}

::grpc::Status IrisInference::Service::Inference(::grpc::ServerContext* context, const ::agent::IrisInferenceRequest* request, ::agent::IrisInferenceResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


static const char* MnistInference_method_names[] = {
  "/agent.MnistInference/Inference",
};

std::unique_ptr< MnistInference::Stub> MnistInference::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< MnistInference::Stub> stub(new MnistInference::Stub(channel, options));
  return stub;
}

MnistInference::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_Inference_(MnistInference_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status MnistInference::Stub::Inference(::grpc::ClientContext* context, const ::agent::MnistInferenceRequest& request, ::agent::MnistInferenceResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::agent::MnistInferenceRequest, ::agent::MnistInferenceResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Inference_, context, request, response);
}

void MnistInference::Stub::async::Inference(::grpc::ClientContext* context, const ::agent::MnistInferenceRequest* request, ::agent::MnistInferenceResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::agent::MnistInferenceRequest, ::agent::MnistInferenceResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Inference_, context, request, response, std::move(f));
}

void MnistInference::Stub::async::Inference(::grpc::ClientContext* context, const ::agent::MnistInferenceRequest* request, ::agent::MnistInferenceResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Inference_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::agent::MnistInferenceResponse>* MnistInference::Stub::PrepareAsyncInferenceRaw(::grpc::ClientContext* context, const ::agent::MnistInferenceRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::agent::MnistInferenceResponse, ::agent::MnistInferenceRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Inference_, context, request);
}

::grpc::ClientAsyncResponseReader< ::agent::MnistInferenceResponse>* MnistInference::Stub::AsyncInferenceRaw(::grpc::ClientContext* context, const ::agent::MnistInferenceRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncInferenceRaw(context, request, cq);
  result->StartCall();
  return result;
}

MnistInference::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      MnistInference_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< MnistInference::Service, ::agent::MnistInferenceRequest, ::agent::MnistInferenceResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](MnistInference::Service* service,
             ::grpc::ServerContext* ctx,
             const ::agent::MnistInferenceRequest* req,
             ::agent::MnistInferenceResponse* resp) {
               return service->Inference(ctx, req, resp);
             }, this)));
}

MnistInference::Service::~Service() {
}

::grpc::Status MnistInference::Service::Inference(::grpc::ServerContext* context, const ::agent::MnistInferenceRequest* request, ::agent::MnistInferenceResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


static const char* FileInference_method_names[] = {
  "/agent.FileInference/Inference",
};

std::unique_ptr< FileInference::Stub> FileInference::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< FileInference::Stub> stub(new FileInference::Stub(channel, options));
  return stub;
}

FileInference::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_Inference_(FileInference_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status FileInference::Stub::Inference(::grpc::ClientContext* context, const ::agent::FileInferenceRequest& request, ::agent::FileInferenceResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::agent::FileInferenceRequest, ::agent::FileInferenceResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Inference_, context, request, response);
}

void FileInference::Stub::async::Inference(::grpc::ClientContext* context, const ::agent::FileInferenceRequest* request, ::agent::FileInferenceResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::agent::FileInferenceRequest, ::agent::FileInferenceResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Inference_, context, request, response, std::move(f));
}

void FileInference::Stub::async::Inference(::grpc::ClientContext* context, const ::agent::FileInferenceRequest* request, ::agent::FileInferenceResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Inference_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::agent::FileInferenceResponse>* FileInference::Stub::PrepareAsyncInferenceRaw(::grpc::ClientContext* context, const ::agent::FileInferenceRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::agent::FileInferenceResponse, ::agent::FileInferenceRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Inference_, context, request);
}

::grpc::ClientAsyncResponseReader< ::agent::FileInferenceResponse>* FileInference::Stub::AsyncInferenceRaw(::grpc::ClientContext* context, const ::agent::FileInferenceRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncInferenceRaw(context, request, cq);
  result->StartCall();
  return result;
}

FileInference::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      FileInference_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< FileInference::Service, ::agent::FileInferenceRequest, ::agent::FileInferenceResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](FileInference::Service* service,
             ::grpc::ServerContext* ctx,
             const ::agent::FileInferenceRequest* req,
             ::agent::FileInferenceResponse* resp) {
               return service->Inference(ctx, req, resp);
             }, this)));
}

FileInference::Service::~Service() {
}

::grpc::Status FileInference::Service::Inference(::grpc::ServerContext* context, const ::agent::FileInferenceRequest* request, ::agent::FileInferenceResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace agent

