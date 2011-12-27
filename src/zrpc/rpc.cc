// Copyright 2011 Google Inc. All Rights Reserved.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: nadavs@google.com <Nadav Samet>

#include "boost/lexical_cast.hpp"
#include "zrpc/logging.h"
#include "zrpc/reactor.h"
#include "zrpc/rpc.h"
#include "zrpc/sync_event.h"
#include "zrpc/zrpc.pb.h"

namespace zrpc {

RPC::RPC()
    : status_(GenericRPCResponse::INACTIVE),
      application_error_(0),
      deadline_ms_(-1),
      sync_event_(new SyncEvent()) {
};

RPC::~RPC() {}

void RPC::SetFailed(int application_error, const std::string& error_message) {
  SetStatus(GenericRPCResponse::APPLICATION_ERROR);
  error_message_ = error_message;
  application_error_ = application_error;
}

void RPC::SetStatus(GenericRPCResponse::Status status) {
  status_ = status;
}

int RPC::Wait() {
  GenericRPCResponse::Status status = GetStatus();
  CHECK_NE(status, GenericRPCResponse::INACTIVE)
      << "Request must be sent before calling Wait()";
  if (status != GenericRPCResponse::INFLIGHT) {
    return GetStatus();
  }
  sync_event_->Wait();
  return 0;
}

std::string RPC::ToString() const {
  std::string result =
      "status: " + GenericRPCResponse::Status_Name(GetStatus());
  if (GetStatus() == GenericRPCResponse::APPLICATION_ERROR) {
    result += "(" + boost::lexical_cast<std::string>(GetApplicationError())
           + ")";
  }
  std::string error_message = GetErrorMessage();
  if (!error_message.empty()) {
    result += ": " + error_message;
  }
  return result;
}
}  // namespace zrpc
