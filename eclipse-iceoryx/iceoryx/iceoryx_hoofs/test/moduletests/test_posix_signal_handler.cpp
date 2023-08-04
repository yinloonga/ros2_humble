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

#include "iceoryx_hoofs/posix_wrapper/signal_handler.hpp"
#include "test.hpp"
#include <atomic>

namespace
{
using namespace ::testing;
using namespace iox::posix;

std::atomic_int signalOfCallback1{0};
std::atomic_int signalOfCallback2{0};

template <Signal SignalValue>
struct SignalType
{
    static constexpr Signal VALUE = SignalValue;
};

template <typename T>
class SignalHandler_test : public Test
{
  public:
    static constexpr Signal SIGNAL_VALUE = T::VALUE;

    void SetUp() override
    {
        internal::CaptureStderr();
        signalOfCallback1 = INVALID_SIGNAL;
        signalOfCallback2 = INVALID_SIGNAL;
    }

    void TearDown() override
    {
        std::string output = internal::GetCapturedStderr();
        if (Test::HasFailure())
        {
            std::cout << output << std::endl;
        }
    }

    static void signalHandler1(int s)
    {
        signalOfCallback1 = s;
    }

    static void signalHandler2(int s)
    {
        signalOfCallback2 = s;
    }

    void registerSignal(const int signal, void (*const callback)(int))
    {
        struct sigaction action = {};
        sigemptyset(&action.sa_mask);
        action.sa_handler = callback;
        action.sa_flags = 0;

        sigaction(signal, &action, nullptr);
    }

    static constexpr int INVALID_SIGNAL = std::numeric_limits<int>::max();
};

using Implementations =
    Types<SignalType<Signal::INT>, SignalType<Signal::BUS>, SignalType<Signal::TERM>, SignalType<Signal::HUP>>;

TYPED_TEST_SUITE(SignalHandler_test, Implementations);

TYPED_TEST(SignalHandler_test, RegisteringSignalGuardCallbackWorks)
{
    ::testing::Test::RecordProperty("TEST_ID", "7836be02-28ab-43b7-b7a7-7c43c4830eb4");
    Signal signalValue = TestFixture::SIGNAL_VALUE;
    auto signalGuard = registerSignalHandler(signalValue, this->signalHandler1);

    raise(static_cast<int>(signalValue));

    EXPECT_THAT(signalOfCallback1, Eq(static_cast<int>(signalValue)));
    EXPECT_THAT(signalOfCallback2, Eq(this->INVALID_SIGNAL));
}

TYPED_TEST(SignalHandler_test, WhenSignalGuardGoesOutOfScopePreviousStateIsRestored)
{
    ::testing::Test::RecordProperty("TEST_ID", "8d2efc2b-837b-446d-ba16-fbca9a539b82");
    Signal signalValue = TestFixture::SIGNAL_VALUE;
    this->registerSignal(static_cast<int>(signalValue), this->signalHandler2);
    {
        auto signalGuard = registerSignalHandler(signalValue, this->signalHandler1);
    }

    raise(static_cast<int>(signalValue));

    EXPECT_THAT(signalOfCallback1, Eq(this->INVALID_SIGNAL));
    EXPECT_THAT(signalOfCallback2, Eq(static_cast<int>(signalValue)));
}

TYPED_TEST(SignalHandler_test, MoveConstructedSignalGuardCallbackWorks)
{
    ::testing::Test::RecordProperty("TEST_ID", "8fcf886b-babb-41ab-a8ee-6ba123224aef");
    Signal signalValue = TestFixture::SIGNAL_VALUE;
    auto signalGuard = registerSignalHandler(signalValue, this->signalHandler1);

    SignalGuard signalGuard2(std::move(signalGuard));

    raise(static_cast<int>(signalValue));

    EXPECT_THAT(signalOfCallback1, Eq(static_cast<int>(signalValue)));
    EXPECT_THAT(signalOfCallback2, Eq(this->INVALID_SIGNAL));
}

TYPED_TEST(SignalHandler_test, MoveConstructedSignalGuardRestoresPreviousState)
{
    ::testing::Test::RecordProperty("TEST_ID", "718c396f-ab2f-4ea3-bb90-f67f7ab131d8");
    Signal signalValue = TestFixture::SIGNAL_VALUE;
    this->registerSignal(static_cast<int>(signalValue), this->signalHandler2);

    {
        auto signalGuard = registerSignalHandler(signalValue, this->signalHandler1);
        SignalGuard signalGuard2(std::move(signalGuard));
    }

    raise(static_cast<int>(signalValue));

    EXPECT_THAT(signalOfCallback1, Eq(this->INVALID_SIGNAL));
    EXPECT_THAT(signalOfCallback2, Eq(static_cast<int>(signalValue)));
}
} // namespace
