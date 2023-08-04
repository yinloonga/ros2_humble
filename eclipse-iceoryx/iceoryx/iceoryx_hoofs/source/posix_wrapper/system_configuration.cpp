// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2021 by Apex.AI Inc. All rights reserved.
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

#include "iceoryx_hoofs/internal/posix_wrapper/system_configuration.hpp"

#include "iceoryx_hoofs/cxx/helplets.hpp"
#include "iceoryx_hoofs/posix_wrapper/posix_call.hpp"

namespace iox
{
namespace posix
{
uint64_t pageSize() noexcept
{
    // sysconf fails when one provides an invalid name parameter. _SC_PAGESIZE
    // is a valid name parameter therefore it should never fail.
    return static_cast<uint64_t>(posixCall(sysconf)(_SC_PAGESIZE)
                                     .failureReturnValue(-1)
                                     .evaluate()
                                     .or_else([](auto& r) {
                                         std::cerr << "This should never happen: " << r.getHumanReadableErrnum()
                                                   << std::endl;
                                         cxx::Ensures(false && "Internal logic error");
                                     })
                                     .value()
                                     .value);
}
} // namespace posix
} // namespace iox
