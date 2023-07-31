
// Copyright (c) 2017-2023 Cloudflare, Inc.
// Licensed under the Apache 2.0 license found in the LICENSE file or at:
//     https://opensource.org/licenses/Apache-2.0

#pragma once

#include <kj/debug.h>
#include <kj/string.h>

#include <workerd/io/worker-interface.capnp.h>
#include <workerd/io/worker-interface.h>
#include <workerd/api/basics.h>

namespace workerd::api {

class JsRpcTargetImpl final : public rpc::JsRpcTarget::Server {
public:
  JsRpcTargetImpl() {}

  kj::Promise<void> call(CallContext context) {
    // TODO(now): Deserialize the arguments.
    auto methodName = kj::heapString(context.getParams().getMethodName());
    auto serializedArgs = context.getParams().getSerializedArgs().getV8Serialized().asBytes();
    KJ_DBG("PARAMS: ", methodName, serializedArgs);
    // TODO(now): Execute the JS.
    auto reader = context.initResults(capnp::MessageSize { 4, 1 }).initResult();
    reader.setV8Serialized(kj::heapString("hello from remote").asBytes());
    // TODO(now): Set the resulting serialized JSG value.
    return kj::READY_NOW;
  }
};

class JsRpcSessionEvent final: public ExtendableEvent {
public:
  explicit JsRpcSessionEvent();

  static jsg::Ref<JsRpcSessionEvent> constructor(kj::String type) = delete;

  JSG_RESOURCE_TYPE(JsRpcSessionEvent) {
    JSG_INHERIT(ExtendableEvent);
  }
};

class JsRpcSessionCustomEventImpl final: public WorkerInterface::CustomEvent,
    public kj::Refcounted {
public:
  JsRpcSessionCustomEventImpl(uint16_t typeId): typeId(typeId) {}

  kj::Promise<Result> run(
      kj::Own<IoContext_IncomingRequest> incomingRequest,
      kj::Maybe<kj::StringPtr> entrypointName) override;

  kj::Promise<Result> sendRpc(
      capnp::HttpOverCapnpFactory& httpOverCapnpFactory,
      capnp::ByteStreamFactory& byteStreamFactory,
      kj::TaskSet& waitUntilTasks,
      rpc::EventDispatcher::Client dispatcher) override;

  uint16_t getType() override {
    return typeId;
  }

  // We need to set the client/server on the event itself to get around CustomEvent's limited
  // return type.

  kj::Maybe<kj::Own<JsRpcTargetImpl>> server;
  // Set by the destination Worker/DO.
  kj::Maybe<rpc::JsRpcTarget::Client> clientCap;
  // Set by callers of the JsRpcSessionRequest().
private:
  uint16_t typeId;
};

#define EW_JS_RPC_SESSION_ISOLATE_TYPES   \
  api::JsRpcSessionEvent,                 \
  api::JsRpcSessionExportedHandler
}  // namespace workerd::api
