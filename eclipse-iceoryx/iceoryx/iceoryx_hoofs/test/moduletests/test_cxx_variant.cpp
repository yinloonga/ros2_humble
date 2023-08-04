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

#include "iceoryx_hoofs/cxx/variant.hpp"
#include "test.hpp"

#include <string>
namespace
{
using namespace testing;

class variant_Test : public Test
{
  public:
    void SetUp() override
    {
        internal::CaptureStderr();
        DoubleDelete::dtorCalls = 0;
        DoubleDelete::ctorCalls = 0;
        DTorTest::dtorWasCalled = false;
    }

    void TearDown() override
    {
        std::string output = internal::GetCapturedStderr();
        if (Test::HasFailure())
        {
            std::cout << output << std::endl;
        }
    }

    class ComplexClass
    {
      public:
        ComplexClass(int a, float b)
            : a(a)
            , b(b)
        {
        }

        int a;
        float b;
    };

    struct DTorTest
    {
        ~DTorTest()
        {
            dtorWasCalled = true;
        }
        static bool dtorWasCalled;
    };

    class DoubleDelete
    {
      public:
        DoubleDelete()
            : doDtorCall{true}
        {
            ctorCalls++;
        }
        ~DoubleDelete()
        {
            Delete();
        }
        DoubleDelete(const DoubleDelete& rhs)
            : doDtorCall{false}
        {
            *this = rhs;
        }
        DoubleDelete(DoubleDelete&& rhs)
            : doDtorCall{false}
        {
            *this = std::move(rhs);
        }

        DoubleDelete& operator=(const DoubleDelete& rhs)
        {
            if (this != &rhs)
            {
                Delete();
                doDtorCall = rhs.doDtorCall;
            }
            return *this;
        }

        DoubleDelete& operator=(DoubleDelete&& rhs)
        {
            if (this != &rhs)
            {
                Delete();
                doDtorCall = std::move(rhs.doDtorCall);
                rhs.doDtorCall = false;
            }
            return *this;
        }

        void Delete()
        {
            if (doDtorCall)
            {
                dtorCalls++;
            }
        }
        static int dtorCalls;
        static int ctorCalls;

      private:
        bool doDtorCall;
    };

    iox::cxx::variant<int, float, ComplexClass> sut;
};
bool variant_Test::DTorTest::dtorWasCalled = false;
int variant_Test::DoubleDelete::dtorCalls = 0;
int variant_Test::DoubleDelete::ctorCalls = 0;

TEST_F(variant_Test, DefaultCTorCreatesInvalidVariant)
{
    ::testing::Test::RecordProperty("TEST_ID", "368fdd21-fd98-4f7a-abb6-8e8b8da5cd8d");
    EXPECT_THAT(sut.index(), Eq(iox::cxx::INVALID_VARIANT_INDEX));
}

TEST_F(variant_Test, InitializedVariantReturnsCorrectIndex)
{
    ::testing::Test::RecordProperty("TEST_ID", "7b1cf4b5-e38d-4aea-804a-f8bd75e0a605");
    sut.emplace<float>(1231.22F);
    EXPECT_THAT(sut.index(), Eq(1U));
}

TEST_F(variant_Test, CreatingVariantFromPODTypeReturnsProvidedValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "d087b440-a669-4467-a016-3a4ae5b5882a");
    iox::cxx::variant<ComplexClass, float> sut2{42.42F};

    ASSERT_THAT(sut2.index(), Eq(1U));
    ASSERT_THAT(sut2.get<float>(), Ne(nullptr));
    EXPECT_THAT(*sut2.get<float>(), Eq(42.42F));
}

TEST_F(variant_Test, CreatingVariantFromLValueReturnsProvidedValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "ff991aeb-15de-45fe-b6fb-a0a3d3c36c68");
    std::string string("Buhh");
    iox::cxx::variant<std::string, float> sut2{string};
    ASSERT_THAT(sut2.index(), Eq(0U));
    ASSERT_THAT(sut2.get<std::string>(), Ne(nullptr));
    EXPECT_THAT(sut2.get<std::string>()->c_str(), StrEq("Buhh"));
}

TEST_F(variant_Test, CreatingVariantWithSameTypeChoosesFirstFittingType)
{
    ::testing::Test::RecordProperty("TEST_ID", "819b5c7d-106c-476a-a49e-aaa78e092e3f");
    iox::cxx::variant<float, float> sut2{73.73F};

    ASSERT_THAT(sut2.index(), Eq(0U));
    ASSERT_THAT(sut2.get<float>(), Ne(nullptr));
    EXPECT_THAT(*sut2.get<float>(), Eq(73.73F));
}

TEST_F(variant_Test, EmplaceValidElementWorks)
{
    ::testing::Test::RecordProperty("TEST_ID", "24021bd7-3749-4ca3-9b35-db3cb4837bff");
    ASSERT_THAT(sut.emplace<ComplexClass>(123, 456.789F), Eq(true));
    ASSERT_THAT(sut.get<ComplexClass>(), Ne(nullptr));
    EXPECT_THAT(sut.get<ComplexClass>()->a, Eq(123));
    EXPECT_THAT(sut.get<ComplexClass>()->b, Eq(456.789F));
}

TEST_F(variant_Test, EmplaceSecondValidElementWorks)
{
    ::testing::Test::RecordProperty("TEST_ID", "fb4ba160-dd0c-4255-9916-cc56d950e970");
    sut.emplace<ComplexClass>(123, 456.789f);
    ASSERT_THAT(sut.emplace<ComplexClass>(912, 65.03F), Eq(true));
    ASSERT_THAT(sut.get<ComplexClass>(), Ne(nullptr));
    EXPECT_THAT(sut.get<ComplexClass>()->a, Eq(912));
    EXPECT_THAT(sut.get<ComplexClass>()->b, Eq(65.03F));
}

TEST_F(variant_Test, DISABLED_emplaceInvalidElement)
{
    ::testing::Test::RecordProperty("TEST_ID", "6fa3b290-8249-4825-8eac-72235a06710e");
    // this is a compile time check, if you uncomment this the compiler will
    // fail
    //    EXPECT_THAT(sut.emplace< unsigned int >(0), Eq(false));
}

TEST_F(variant_Test, EmplaceWhenAlreadyDifferentTypeAssignedDoesNotWork)
{
    ::testing::Test::RecordProperty("TEST_ID", "f2c0825a-349c-4acb-9643-cb046f7e0310");
    sut.emplace<int>(123);
    EXPECT_THAT(sut.emplace<float>(123.F), Eq(false));
}

TEST_F(variant_Test, GetOnUninitializedVariantFails)
{
    ::testing::Test::RecordProperty("TEST_ID", "d1da6416-f198-4756-9bdd-2c61b9750005");
    EXPECT_THAT(sut.get<float>(), Eq(nullptr));
}

TEST_F(variant_Test, GetVariantWithCorrectValueWorks)
{
    ::testing::Test::RecordProperty("TEST_ID", "ab08976b-0986-4fad-bfdb-f10810d19ae6");
    sut.emplace<float>(123.12F);
    EXPECT_THAT(sut.get<float>(), Ne(nullptr));
}

TEST_F(variant_Test, GetVariantWithIncorrectValueFails)
{
    ::testing::Test::RecordProperty("TEST_ID", "9c83f8ed-f4a1-4e45-b63f-26924544cc77");
    sut.emplace<float>(123.12F);
    EXPECT_THAT(sut.get<int>(), Eq(nullptr));
}

TEST_F(variant_Test, ConstGetOnUninitializedVariantFails)
{
    ::testing::Test::RecordProperty("TEST_ID", "16b511c8-e56a-48c9-bbff-5a7d07e7a500");
    EXPECT_THAT(const_cast<const decltype(sut)*>(&sut)->get<float>(), Eq(nullptr));
}

TEST_F(variant_Test, constGetVariantWithCorrectValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "4419f569-0541-43ab-8448-9ba10556b459");
    sut.emplace<float>(123.12F);
    EXPECT_THAT(const_cast<const decltype(sut)*>(&sut)->get<float>(), Ne(nullptr));
}

TEST_F(variant_Test, ConstGetVariantWithIncorrectValueFails)
{
    ::testing::Test::RecordProperty("TEST_ID", "979e1131-e981-4f67-bef2-a1fe4770c5a6");
    sut.emplace<float>(123.12F);
    EXPECT_THAT(const_cast<const decltype(sut)*>(&sut)->get<int>(), Eq(nullptr));
}

TEST_F(variant_Test, Get_ifWhenUninitializedReturnsProvidedValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "64585111-3495-4c01-8900-af2e19223e62");
    float bla;
    EXPECT_THAT(sut.get_if<float>(&bla), Eq(&bla));
}

TEST_F(variant_Test, Get_ifInitializedWithCorrectValueWorks)
{
    ::testing::Test::RecordProperty("TEST_ID", "27b80822-0f32-46a6-83d9-595b35e23139");
    sut.emplace<float>(12.1F);
    float bla;
    EXPECT_THAT(sut.get_if<float>(&bla), Ne(&bla));
}

TEST_F(variant_Test, Get_ifInitializedWithIncorrectValueReturnsProvidedValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "4126af94-d4ce-405a-bcc7-1b6d1fce6d0b");
    sut.emplace<float>(12.1F);
    int bla;
    EXPECT_THAT(sut.get_if<int>(&bla), Eq(&bla));
}

TEST_F(variant_Test, DTorIsCalled)
{
    ::testing::Test::RecordProperty("TEST_ID", "46f16073-af10-4d4b-9944-a316bc5847ef");
    DTorTest::dtorWasCalled = false;
    {
        iox::cxx::variant<int, DTorTest> schlomo;
        schlomo.emplace<DTorTest>();
    }
    EXPECT_THAT(DTorTest::dtorWasCalled, Eq(true));
}

TEST_F(variant_Test, DTorIsCalledAfterEmplace)
{
    ::testing::Test::RecordProperty("TEST_ID", "0de8d215-76be-4727-9fb4-42c54cb1b742");
    {
        iox::cxx::variant<int, float, DTorTest, double> ignatz;
        ignatz.emplace<DTorTest>();
        DTorTest::dtorWasCalled = false;
    }
    EXPECT_THAT(DTorTest::dtorWasCalled, Eq(true));
}

TEST_F(variant_Test, CopyCTorWithValueLeadsToSameValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "710f8add-4a3c-4b9c-b429-5f42689b49da");
    iox::cxx::variant<int, char> schlomo;
    schlomo.emplace<int>(123);
    iox::cxx::variant<int, char> ignatz(schlomo);
    ASSERT_THAT(ignatz.get<int>(), Ne(nullptr));
    EXPECT_THAT(*ignatz.get<int>(), Eq(123));
}

TEST_F(variant_Test, CopyCTorWithoutValueResultsInInvalidVariant)
{
    ::testing::Test::RecordProperty("TEST_ID", "31b12efc-4f2d-4b3c-ad72-c12ca8a0cfc3");
    iox::cxx::variant<int, char> schlomo;
    iox::cxx::variant<int, char> ignatz(schlomo);
    ASSERT_THAT(ignatz.index(), Eq(iox::cxx::INVALID_VARIANT_INDEX));
}

TEST_F(variant_Test, CopyAssignmentWithValueLeadsToSameValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "ebf341fa-c218-4ab8-ae13-a35d707bb5b2");
    iox::cxx::variant<int, char> ignatz;
    ignatz.emplace<char>('c');
    {
        iox::cxx::variant<int, char> schlomo;
        schlomo.emplace<int>(447);
        ignatz = schlomo;
    }
    ASSERT_THAT(ignatz.get<int>(), Ne(nullptr));
    ASSERT_THAT(*ignatz.get<int>(), Eq(447));
}

TEST_F(variant_Test, CopyAssignmentWithoutValueResultsInInvalidVariant)
{
    ::testing::Test::RecordProperty("TEST_ID", "75466e8c-78b3-4a2f-84cb-06849cf80baa");
    iox::cxx::variant<int, char> ignatz;
    ignatz.emplace<char>('c');
    {
        iox::cxx::variant<int, char> schlomo;
        ignatz = schlomo;
    }
    ASSERT_THAT(ignatz.index(), Eq(iox::cxx::INVALID_VARIANT_INDEX));
    ASSERT_THAT(ignatz.get<char>(), Eq(nullptr));
}

TEST_F(variant_Test, MoveCTorWithValueLeadsToSameValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "34962242-8319-48e5-a064-5118c6ffa080");
    iox::cxx::variant<int, char> schlomo;
    schlomo.emplace<int>(123);
    iox::cxx::variant<int, char> ignatz(std::move(schlomo));
    ASSERT_THAT(ignatz.get<int>(), Ne(nullptr));
    EXPECT_THAT(*ignatz.get<int>(), Eq(123));
    EXPECT_THAT(schlomo.index(), Eq(0U));
}

TEST_F(variant_Test, MoveCTorWithoutValueResultsInInvalidVariant)
{
    ::testing::Test::RecordProperty("TEST_ID", "83db1777-3e66-4755-9072-0c90973ae303");
    iox::cxx::variant<int, char> schlomo;
    iox::cxx::variant<int, char> ignatz(std::move(schlomo));
    ASSERT_THAT(ignatz.index(), Eq(iox::cxx::INVALID_VARIANT_INDEX));
}

TEST_F(variant_Test, MoveAssignmentWithValueLeadsToSameValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "ee36df28-545f-42bc-9ef6-3699284f1a42");
    iox::cxx::variant<int, char> ignatz;
    ignatz.emplace<char>('c');
    {
        iox::cxx::variant<int, char> schlomo;
        schlomo.emplace<int>(447);
        ignatz = std::move(schlomo);
    }
    ASSERT_THAT(ignatz.get<int>(), Ne(nullptr));
    ASSERT_THAT(*ignatz.get<int>(), Eq(447));
}

TEST_F(variant_Test, MoveAssignmentWithoutValueResultsInInvalidVariant)
{
    ::testing::Test::RecordProperty("TEST_ID", "5b6c8183-3ea1-44ee-ac99-7f427127b82b");
    iox::cxx::variant<int, char> ignatz;
    ignatz.emplace<char>('c');
    {
        iox::cxx::variant<int, char> schlomo;
        ignatz = std::move(schlomo);
    }
    ASSERT_THAT(ignatz.get<int>(), Eq(nullptr));
    ASSERT_THAT(ignatz.index(), Eq(iox::cxx::INVALID_VARIANT_INDEX));
}

TEST_F(variant_Test, CreatingSecondObjectViaCopyCTorResultsInTwoDTorCalls)
{
    ::testing::Test::RecordProperty("TEST_ID", "57a5836f-d981-445a-9040-6c3358cee6c4");
    {
        iox::cxx::variant<int, DTorTest> ignatz;
        ignatz.emplace<DTorTest>();
        DTorTest::dtorWasCalled = false;
        {
            iox::cxx::variant<int, DTorTest> schlomo(ignatz);
            EXPECT_THAT(DTorTest::dtorWasCalled, Eq(false));
        }
        EXPECT_THAT(DTorTest::dtorWasCalled, Eq(true));
        DTorTest::dtorWasCalled = false;
    }
    EXPECT_THAT(DTorTest::dtorWasCalled, Eq(true));
}

TEST_F(variant_Test, CreatingSecondObjectViaCopyAssignmentResultsInTwoDTorCalls)
{
    ::testing::Test::RecordProperty("TEST_ID", "6932e30b-f24f-46be-88e0-d745d9f0db92");
    {
        iox::cxx::variant<int, DTorTest> ignatz;
        ignatz.emplace<DTorTest>();
        DTorTest::dtorWasCalled = false;
        {
            iox::cxx::variant<int, DTorTest> schlomo;
            schlomo.emplace<int>(123);
            schlomo = ignatz;
            EXPECT_THAT(DTorTest::dtorWasCalled, Eq(false));
        }
        EXPECT_THAT(DTorTest::dtorWasCalled, Eq(true));
        DTorTest::dtorWasCalled = false;
    }
    EXPECT_THAT(DTorTest::dtorWasCalled, Eq(true));
}

TEST_F(variant_Test, CreatingSecondObjectViaMoveCTorResultsInTwoDTorCalls)
{
    ::testing::Test::RecordProperty("TEST_ID", "0cb220db-be81-4279-b646-a35b0d177451");
    {
        iox::cxx::variant<int, DTorTest> ignatz;
        ignatz.emplace<DTorTest>();
        DTorTest::dtorWasCalled = false;
        {
            iox::cxx::variant<int, DTorTest> schlomo(std::move(ignatz));
            EXPECT_THAT(DTorTest::dtorWasCalled, Eq(false));
            EXPECT_THAT(ignatz.index(), Eq(1U));
        }
        EXPECT_THAT(DTorTest::dtorWasCalled, Eq(true));
        DTorTest::dtorWasCalled = false;
    }
    EXPECT_THAT(DTorTest::dtorWasCalled, Eq(true));
}

TEST_F(variant_Test, CreatingSecondObjectViaMoveAssignmentResultsInTwoDTorCalls)
{
    ::testing::Test::RecordProperty("TEST_ID", "24a7b31a-eafa-4492-a57b-26ee7f9801f1");
    {
        iox::cxx::variant<int, DTorTest> ignatz;
        ignatz.emplace<DTorTest>();
        DTorTest::dtorWasCalled = false;
        {
            iox::cxx::variant<int, DTorTest> schlomo;
            schlomo.emplace<int>(123);
            schlomo = std::move(ignatz);
            EXPECT_THAT(ignatz.index(), Eq(1U));
            EXPECT_THAT(DTorTest::dtorWasCalled, Eq(false));
        }
        EXPECT_THAT(DTorTest::dtorWasCalled, Eq(true));
        DTorTest::dtorWasCalled = false;
    }
    EXPECT_THAT(DTorTest::dtorWasCalled, Eq(true));
}

TEST_F(variant_Test, DirectValueAssignmentResultsInCorrectIndex)
{
    ::testing::Test::RecordProperty("TEST_ID", "55377419-08dc-4d6b-b163-b2da7ad70417");
    iox::cxx::variant<int, float> schlomo;
    schlomo = 123;
    EXPECT_THAT(schlomo.index(), Eq(0U));
}

TEST_F(variant_Test, DirectValueAssignmentWhenAlreadyAssignedWithDifferentType)
{
    ::testing::Test::RecordProperty("TEST_ID", "a058c173-497b-43ec-ba03-2702f3ba8190");
    iox::cxx::variant<int, float> schlomo;
    schlomo = 123;
    schlomo = 123.01F;
    EXPECT_THAT(schlomo.index(), Eq(0U));
}

TEST_F(variant_Test, HoldsAlternativeForCorrectType)
{
    ::testing::Test::RecordProperty("TEST_ID", "9da264db-a84e-41cd-94ff-92af529e2d6b");
    iox::cxx::variant<int, float> schlomo;
    schlomo = 123;
    EXPECT_THAT(iox::cxx::holds_alternative<int>(schlomo), Eq(true));
}

TEST_F(variant_Test, HoldsAlternativeForIncorrectType)
{
    ::testing::Test::RecordProperty("TEST_ID", "63f3690f-1f66-407c-8050-97f47f62638e");
    iox::cxx::variant<int, float> schlomo;
    schlomo = 123;
    EXPECT_THAT(iox::cxx::holds_alternative<float>(schlomo), Eq(false));
}

TEST_F(variant_Test, SameTypeVariantAndEmplaceWithIndexResultsInCorrectValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "fb55e6d8-d42d-4073-b5db-5300c56df540");
    iox::cxx::variant<int, float, int> schlomo;

    ASSERT_THAT(schlomo.emplace_at_index<2>(123), Eq(true));
    EXPECT_THAT(*schlomo.get_at_index<2>(), Eq(123));
}

TEST_F(variant_Test, SameTypeVariantResultsInCorrectIndex)
{
    ::testing::Test::RecordProperty("TEST_ID", "10994259-9fb5-411f-9e12-365f2d8e09fd");
    iox::cxx::variant<int, float, int> schlomo;

    EXPECT_THAT(schlomo.emplace_at_index<1>(1.23F), Eq(true));
    EXPECT_THAT(schlomo.index(), Eq(1U));
}

TEST_F(variant_Test, SameTypeVariantReturnsNothingForIncorrectIndex)
{
    ::testing::Test::RecordProperty("TEST_ID", "04c16cb1-f67f-47fa-bde8-f15ff0d044c3");
    iox::cxx::variant<int, float, int> schlomo;

    ASSERT_THAT(schlomo.emplace_at_index<2>(123), Eq(true));
    EXPECT_THAT(schlomo.get_at_index<1>(), Eq(nullptr));
}

TEST_F(variant_Test, ConstSameTypeVariantAndEmplaceWithIndexResultsInCorrectValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "89511ef2-46e6-49f5-8272-713a860ff070");
    iox::cxx::variant<int, float, int> schlomo;
    const iox::cxx::variant<int, float, int>* ignatz = &schlomo;

    ASSERT_THAT(schlomo.emplace_at_index<2>(4123), Eq(true));
    EXPECT_THAT(*ignatz->get_at_index<2>(), Eq(4123));
}

TEST_F(variant_Test, InPlaceAtIndexCTorResultsInCorrectIndexAndValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "6a1fe90d-da43-4d82-b5f5-e940b4724e86");
    iox::cxx::variant<int, float, int> schlomo(iox::cxx::in_place_index<0>(), 445);

    ASSERT_THAT(schlomo.index(), Eq(0U));
    EXPECT_THAT(*schlomo.get_at_index<0>(), Eq(445));
}

TEST_F(variant_Test, InPlaceAtTypeCTorResultsInCorrectIndexAndValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "71ddd0cd-125f-4f69-b12d-f0f024c541a4");
    iox::cxx::variant<int, float, double> schlomo(iox::cxx::in_place_type<double>(), 90.12);

    ASSERT_THAT(schlomo.index(), Eq(2U));
    EXPECT_THAT(*schlomo.get_at_index<2>(), Eq(90.12));
}

TEST_F(variant_Test, ComplexDTorUsingWrongTypeResultsInNoDTorCall)
{
    ::testing::Test::RecordProperty("TEST_ID", "6a686a84-bbbf-47e4-b2a2-8c8f295cf999");
    DoubleDelete::dtorCalls = 0;
    {
        iox::cxx::variant<int, DoubleDelete> schlomo(iox::cxx::in_place_type<int>(), 90);
    }

    EXPECT_THAT(DoubleDelete::dtorCalls, Eq(0));
}

TEST_F(variant_Test, ComplexDTorUsingCorrectTypeWithEmplace)
{
    ::testing::Test::RecordProperty("TEST_ID", "02198ec8-9bea-4dce-81ed-0b2596cfe91e");
    DoubleDelete::ctorCalls = 0;
    DoubleDelete::dtorCalls = 0;
    {
        iox::cxx::variant<int, DoubleDelete> schlomo;
        schlomo.emplace<DoubleDelete>();
    }

    EXPECT_THAT(DoubleDelete::ctorCalls, Eq(1));
    EXPECT_THAT(DoubleDelete::dtorCalls, Eq(1));
}

TEST_F(variant_Test, ComplexDTorUsingCorrectTypeWithInPlace)
{
    ::testing::Test::RecordProperty("TEST_ID", "4b218bd9-936d-4eed-9c11-8e9cb6748885");
    DoubleDelete::ctorCalls = 0;
    DoubleDelete::dtorCalls = 0;
    {
        iox::cxx::variant<int, DoubleDelete> schlomo{iox::cxx::in_place_type<DoubleDelete>()};
    }

    EXPECT_THAT(DoubleDelete::ctorCalls, Eq(1));
    EXPECT_THAT(DoubleDelete::dtorCalls, Eq(1));
}

TEST_F(variant_Test, ComplexDTorWithCopyCTor)
{
    ::testing::Test::RecordProperty("TEST_ID", "0fe12067-1ce8-48b0-8ca7-42f2a755e075");
    DoubleDelete::ctorCalls = 0;
    DoubleDelete::dtorCalls = 0;
    {
        iox::cxx::variant<int, DoubleDelete> schlomo{iox::cxx::in_place_type<DoubleDelete>()};
        iox::cxx::variant<int, DoubleDelete> sut{schlomo};
    }

    EXPECT_THAT(DoubleDelete::ctorCalls, Eq(1));
    EXPECT_THAT(DoubleDelete::dtorCalls, Eq(2));
}

TEST_F(variant_Test, ComplexDTorWithCopyAssignmentTwoVariantsWithValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "2a1ff555-0f07-41d6-8539-8e767deb4c43");
    DoubleDelete::ctorCalls = 0;
    DoubleDelete::dtorCalls = 0;
    {
        iox::cxx::variant<int, DoubleDelete> schlomo{iox::cxx::in_place_type<DoubleDelete>()};
        iox::cxx::variant<int, DoubleDelete> sut{iox::cxx::in_place_type<DoubleDelete>()};
        sut = schlomo;
    }

    EXPECT_THAT(DoubleDelete::ctorCalls, Eq(2));
    EXPECT_THAT(DoubleDelete::dtorCalls, Eq(3));
}

TEST_F(variant_Test, ComplexDTorWithMove)
{
    ::testing::Test::RecordProperty("TEST_ID", "486d5b9e-04f7-4ace-b817-2863709d624b");
    DoubleDelete::ctorCalls = 0;
    DoubleDelete::dtorCalls = 0;
    {
        iox::cxx::variant<int, DoubleDelete> schlomo{iox::cxx::in_place_type<DoubleDelete>()};
        iox::cxx::variant<int, DoubleDelete> sut = std::move(schlomo);
    }

    EXPECT_THAT(DoubleDelete::ctorCalls, Eq(1));
    EXPECT_THAT(DoubleDelete::dtorCalls, Eq(1));
}

TEST_F(variant_Test, ComplexDTorWithMoveAssignment)
{
    ::testing::Test::RecordProperty("TEST_ID", "56bc9246-5c49-4e9e-a16a-5952a82ed784");
    DoubleDelete::ctorCalls = 0;
    DoubleDelete::dtorCalls = 0;
    {
        iox::cxx::variant<int, DoubleDelete> sut;
        iox::cxx::variant<int, DoubleDelete> schlomo{iox::cxx::in_place_type<DoubleDelete>()};
        sut = std::move(schlomo);
    }

    EXPECT_THAT(DoubleDelete::ctorCalls, Eq(1));
    EXPECT_THAT(DoubleDelete::dtorCalls, Eq(1));
}

TEST_F(variant_Test, ComplexDTorWithMoveAssignmentTwoVariantsWithValue)
{
    ::testing::Test::RecordProperty("TEST_ID", "4b1d0931-34b0-4e2a-a70c-445693b6371f");
    DoubleDelete::ctorCalls = 0;
    DoubleDelete::dtorCalls = 0;
    {
        iox::cxx::variant<int, DoubleDelete> sut{iox::cxx::in_place_type<DoubleDelete>()};
        iox::cxx::variant<int, DoubleDelete> schlomo{iox::cxx::in_place_type<DoubleDelete>()};
        sut = std::move(schlomo);
    }

    EXPECT_THAT(DoubleDelete::ctorCalls, Eq(2));
    EXPECT_THAT(DoubleDelete::dtorCalls, Eq(2));
}

TEST_F(variant_Test, MoveVariantIntoVariantOfDifferentType)
{
    ::testing::Test::RecordProperty("TEST_ID", "1f292f13-8a88-4f73-8589-df4d5259c791");
    DoubleDelete::ctorCalls = 0;
    DoubleDelete::dtorCalls = 0;
    iox::cxx::variant<DoubleDelete, ComplexClass> sut1, sut2;
    sut1.emplace<DoubleDelete>();
    sut2.emplace<ComplexClass>(12, 12.12F);

    sut1 = std::move(sut2);

    EXPECT_THAT(DoubleDelete::dtorCalls, Eq(1));
}

TEST_F(variant_Test, CopyVariantIntoVariantOfDifferentType)
{
    ::testing::Test::RecordProperty("TEST_ID", "d0ea4fed-7b18-4d0d-aa05-d76911fb29f7");
    DoubleDelete::ctorCalls = 0;
    DoubleDelete::dtorCalls = 0;
    iox::cxx::variant<DoubleDelete, ComplexClass> sut1, sut2;
    sut1.emplace<DoubleDelete>();
    sut2.emplace<ComplexClass>(12, 12.12F);

    sut1 = sut2;

    EXPECT_THAT(DoubleDelete::dtorCalls, Eq(1));
}
} // namespace
