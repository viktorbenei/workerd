
// Copyright (c) 2017-2023 Cloudflare, Inc.
// Licensed under the Apache 2.0 license found in the LICENSE file or at:
//     https://opensource.org/licenses/Apache-2.0

#include "js-rpc-session.h"
#include <workerd/api/global-scope.h>
#include <workerd/jsg/ser.h>
#include <workerd/io/worker-interface.capnp.h>

namespace workerd::api {

JsRpcSessionEvent::JsRpcSessionEvent()
    : ExtendableEvent("webSocketMessage") {};

kj::Promise<WorkerInterface::CustomEvent::Result> JsRpcSessionCustomEventImpl::run(
    kj::Own<IoContext_IncomingRequest> incomingRequest,
    kj::Maybe<kj::StringPtr> entrypointName) {
  EventOutcome outcome = EventOutcome::OK;

  try {
    server = kj::heap<JsRpcTargetImpl>();
  } catch(kj::Exception e) {
    if (auto desc = e.getDescription();
        !jsg::isTunneledException(desc) && !jsg::isDoNotLogException(desc)) {
      LOG_EXCEPTION("JsRpcSessionCustomEventImpl"_kj, e);
    }
    outcome = EventOutcome::EXCEPTION;
  }

  co_return WorkerInterface::CustomEvent::Result {
    .outcome = outcome
  };
}

kj::Promise<WorkerInterface::CustomEvent::Result>
  JsRpcSessionCustomEventImpl::sendRpc(
    capnp::HttpOverCapnpFactory& httpOverCapnpFactory,
    capnp::ByteStreamFactory& byteStreamFactory,
    kj::TaskSet& waitUntilTasks,
    rpc::EventDispatcher::Client dispatcher) {
  auto req = dispatcher.jsRpcSessionRequest();
  this->clientCap = req.send().getServer();
  co_return WorkerInterface::CustomEvent::Result {
    .outcome = EventOutcome::OK
  };
}

}  // namespace workerd::api
