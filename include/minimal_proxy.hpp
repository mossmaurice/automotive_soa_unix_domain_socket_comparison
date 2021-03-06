// Copyright (c) 2022 by Apex.AI Inc. All rights reserved.
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
// SPDX-License-Identifier: Apache-2.0

#ifndef IOX_EXAMPLES_AUTOMOTIVE_SOA_MINIMAL_PROXY_HPP
#define IOX_EXAMPLES_AUTOMOTIVE_SOA_MINIMAL_PROXY_HPP

#include "topic.hpp"

#include "owl/kom/event_subscriber.hpp"
// Used only for benchmarking iceoryx against UNIX domain sockets
#include "owl/kom/event_subscriber_uds.hpp"
#include "owl/kom/field_subscriber.hpp"
#include "owl/kom/method_client.hpp"
#include "owl/runtime.hpp"
#include "owl/types.hpp"

/// @note Proxy classes are typically generated
/// @note Once a handler has been set with 'EnableFindServiceCallback', calling 'FindService' is not thread-safe!
class MinimalProxy
{
  public:
    static constexpr char SERVICE_IDENTIFIER[] = "ExampleSkeleton";

    MinimalProxy(const owl::kom::ProxyHandleType& handle) noexcept;
    ~MinimalProxy() noexcept = default;

    MinimalProxy(const MinimalProxy&) = delete;
    MinimalProxy(MinimalProxy&&) = delete;
    MinimalProxy& operator=(const MinimalProxy&) = delete;
    MinimalProxy& operator=(MinimalProxy&&) = delete;

    static owl::kom::FindServiceCallbackHandle
    EnableFindServiceCallback(const owl::kom::FindServiceCallback<owl::kom::ProxyHandleType> handler,
                              const owl::kom::InstanceIdentifier& instanceIdentifier) noexcept;

    static void DisableFindServiceCallback(const owl::kom::FindServiceCallbackHandle handle) noexcept;

    static owl::kom::ServiceHandleContainer<owl::kom::ProxyHandleType>
    FindService(const owl::kom::InstanceIdentifier& instanceIdentifier) noexcept;

    const owl::kom::InstanceIdentifier m_instanceIdentifier;
    owl::kom::EventSubscriber<TimestampTopic1Byte, EVENT_IPC_MECHANISM> m_event{
        SERVICE_IDENTIFIER, m_instanceIdentifier, "Event"};
    owl::kom::FieldSubscriber<Topic> m_field{SERVICE_IDENTIFIER, m_instanceIdentifier, "Field"};
    owl::kom::MethodClient computeSum{SERVICE_IDENTIFIER, m_instanceIdentifier, "Method"};
};

#endif // IOX_EXAMPLES_AUTOMOTIVE_SOA_MINIMAL_PROXY_HPP
