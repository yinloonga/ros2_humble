// Copyright (c) 2020 - 2021 by Apex.AI Inc. All rights reserved.
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

#include "iceoryx_hoofs/posix_wrapper/thread.hpp"
#include "test.hpp"

#include <atomic>
#include <thread>

namespace
{
using namespace ::testing;
using namespace iox::posix;
using namespace iox::cxx;

class Thread_test : public Test
{
  public:
    Thread_test()
    {
    }

    void SetUp()
    {
        m_run = true;
        m_thread = new std::thread(&Thread_test::threadFunc, this);
    }

    void TearDown()
    {
        m_run = false;
        m_thread->join();
        delete m_thread;
    }

    ~Thread_test()
    {
    }

    void threadFunc()
    {
        while (m_run)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };

    std::atomic_bool m_run{true};
    std::thread* m_thread;
};

#if !defined(__APPLE__)
TEST_F(Thread_test, SetAndGetWithEmptyThreadNameIsWorking)
{
    ::testing::Test::RecordProperty("TEST_ID", "b805a0a6-29c0-41df-b5b7-3f66499d151a");
    ThreadName_t emptyString = "";

    setThreadName(m_thread->native_handle(), emptyString);
    auto getResult = getThreadName(m_thread->native_handle());

    EXPECT_THAT(getResult, StrEq(emptyString));
}

TEST_F(Thread_test, SetAndGetWithThreadNameCapacityIsWorking)
{
    ::testing::Test::RecordProperty("TEST_ID", "115cf4e9-4c7a-4fcc-8df8-65e3b3b547d1");
    ThreadName_t stringEqualToThreadNameCapacitiy = "123456789ABCDEF";
    EXPECT_THAT(stringEqualToThreadNameCapacitiy.capacity(), Eq(stringEqualToThreadNameCapacitiy.size()));

    setThreadName(m_thread->native_handle(), stringEqualToThreadNameCapacitiy);
    auto getResult = getThreadName(m_thread->native_handle());

    EXPECT_THAT(getResult, StrEq(stringEqualToThreadNameCapacitiy));
}

TEST_F(Thread_test, SetAndGetSmallStringIsWorking)
{
    ::testing::Test::RecordProperty("TEST_ID", "d6c2d0b5-a6ee-43e6-8870-053feb6de845");
    char stringShorterThanThreadNameCapacitiy[] = "I'm short";

    setThreadName(m_thread->native_handle(), stringShorterThanThreadNameCapacitiy);
    auto getResult = getThreadName(m_thread->native_handle());

    EXPECT_THAT(getResult, StrEq(stringShorterThanThreadNameCapacitiy));
}
#endif
} // namespace
