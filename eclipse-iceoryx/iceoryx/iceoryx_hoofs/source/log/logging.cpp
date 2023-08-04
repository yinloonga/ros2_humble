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

#include <utility>

#include "iceoryx_hoofs/log/logging.hpp"

#include "iceoryx_hoofs/log/logger.hpp"
#include "iceoryx_hoofs/log/logmanager.hpp"
#include "iceoryx_hoofs/log/logstream.hpp"

namespace iox
{
namespace log
{
Logger&
createLogger(const std::string& ctxId, const std::string& ctxDescription, const LogLevel appDefLogLevel) noexcept
{
    return LogManager::CreateLogContext(ctxId, ctxDescription, appDefLogLevel);
}

} // namespace log
} // namespace iox
