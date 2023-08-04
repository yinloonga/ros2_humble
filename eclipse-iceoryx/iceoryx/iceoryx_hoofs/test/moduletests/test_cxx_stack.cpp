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

#include "iceoryx_hoofs/cxx/stack.hpp"
#include "test.hpp"

namespace
{
using namespace iox;
using namespace ::testing;

class TestClass
{
  public:
    TestClass() noexcept = default;
    TestClass(const int32_t a, const int32_t b, const int32_t c) noexcept
        : m_a(a)
        , m_b(b)
        , m_c(c)
    {
    }

    bool operator==(const TestClass& rhs) const noexcept
    {
        return m_a == rhs.m_a && m_b == rhs.m_b && m_c == rhs.m_c;
    }
    int32_t m_a = 0, m_b = 0, m_c = 0;
};

class stack_test : public Test
{
  public:
    static constexpr uint64_t STACK_SIZE = 10U;
    cxx::stack<TestClass, STACK_SIZE> m_sut;

    void pushElements(const uint64_t numberOfElements)
    {
        for (uint64_t i = 0U; i < numberOfElements; ++i)
        {
            ASSERT_TRUE(m_sut.push(1 + i, 2 + i, 3 + i));
            EXPECT_THAT(m_sut.size(), Eq(i + 1U));
            EXPECT_THAT(m_sut.capacity(), Eq(STACK_SIZE));
        }
    }
};

TEST_F(stack_test, isEmptyOnCreation)
{
    ::testing::Test::RecordProperty("TEST_ID", "2a9ce587-9daf-479d-95da-0df96325023f");
    EXPECT_THAT(m_sut.size(), Eq(0U));
    EXPECT_THAT(m_sut.capacity(), Eq(STACK_SIZE));
    EXPECT_THAT(m_sut.pop(), Eq(cxx::nullopt));
}

TEST_F(stack_test, pushingOneElementWithDefaultCtorSucceeds)
{
    ::testing::Test::RecordProperty("TEST_ID", "f4e91f32-fb20-4502-b0c8-d4b4d4c2bec0");
    ASSERT_TRUE(m_sut.push());
    EXPECT_THAT(m_sut.size(), Eq(1U));
    EXPECT_THAT(m_sut.capacity(), Eq(STACK_SIZE));

    auto element = m_sut.pop();
    ASSERT_TRUE(element.has_value());
    EXPECT_THAT(*element, Eq(TestClass(0, 0, 0)));
}

TEST_F(stack_test, pushingOneElementWithCustomCtorSucceeds)
{
    ::testing::Test::RecordProperty("TEST_ID", "c9112cdb-af63-4a55-b8ee-94010dc2d819");
    pushElements(1U);

    auto element = m_sut.pop();
    ASSERT_TRUE(element.has_value());
    EXPECT_THAT(*element, Eq(TestClass(1, 2, 3)));
}

TEST_F(stack_test, pushingElementsTillStackIsFullAndPoppingInLIFOOrderSucceeds)
{
    ::testing::Test::RecordProperty("TEST_ID", "2d12fd5d-ded8-482d-86dd-094660c65f9c");
    pushElements(STACK_SIZE);

    for (uint64_t i = 0U; i < STACK_SIZE; ++i)
    {
        auto element = m_sut.pop();
        EXPECT_THAT(m_sut.size(), Eq(STACK_SIZE - i - 1U));
        ASSERT_TRUE(element.has_value());
        EXPECT_THAT(*element, Eq(TestClass(STACK_SIZE - i, 1 + STACK_SIZE - i, 2 + STACK_SIZE - i)));
    }
}

TEST_F(stack_test, ifCapacityIsExceededPushFails)
{
    ::testing::Test::RecordProperty("TEST_ID", "6844bc2d-f8e9-4614-ad14-62744a7421f7");
    pushElements(STACK_SIZE);

    EXPECT_FALSE(m_sut.push());
}

TEST_F(stack_test, popCreatesSpaceForAnotherElement)
{
    ::testing::Test::RecordProperty("TEST_ID", "3ebf7f6d-81ef-45d6-83a6-80f8588cbba6");
    pushElements(STACK_SIZE);

    EXPECT_THAT(m_sut.pop(), Ne(cxx::nullopt));
    EXPECT_TRUE(m_sut.push());
}

} // namespace
