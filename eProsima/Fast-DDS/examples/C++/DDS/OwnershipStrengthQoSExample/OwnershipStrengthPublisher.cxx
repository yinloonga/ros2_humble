// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

/*************************************************************************
 * @file OwnershipStrengthPublisher.cpp
 * This file contains the implementation of the publisher functions.
 *
 * This file was generated by the tool fastcdrgen.
 */

#include "OwnershipStrengthPublisher.h"

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <sstream>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::rtps;

OwnershipStrengthPublisher::OwnershipStrengthPublisher()
    : participant_(nullptr)
    , publisher_(nullptr)
    , topic_(nullptr)
    , writer_(nullptr)
    , myType(new ExampleMessagePubSubType())
    , m_strength(DefaultStrength)
    , m_messagesSent(0)
{
}

OwnershipStrengthPublisher::~OwnershipStrengthPublisher()
{
    if (writer_ != nullptr)
    {
        publisher_->delete_datawriter(writer_);
    }
    if (publisher_ != nullptr)
    {
        participant_->delete_publisher(publisher_);
    }
    if (topic_ != nullptr)
    {
        participant_->delete_topic(topic_);
    }
    DomainParticipantFactory::get_instance()->delete_participant(participant_);
}

void OwnershipStrengthPublisher::setOwnershipStrength(
        unsigned int strength)
{
    m_strength = strength;
}

bool OwnershipStrengthPublisher::init()
{
    // Create Participant
    DomainParticipantQos pqos;
    pqos.wire_protocol().builtin.discovery_config.leaseDuration = eprosima::fastrtps::c_TimeInfinite;
    pqos.name("Participant_publisher");

    participant_ = DomainParticipantFactory::get_instance()->create_participant(0, pqos);
    if (participant_ == nullptr)
    {
        return false;
    }

    //Register the type
    myType.register_type(participant_);

    // Create Publisher
    publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT);

    if (publisher_ == nullptr)
    {
        return false;
    }

    //Create Topic
    topic_ = participant_->create_topic("OwnershipStrengthPubSubTopic", myType.get_type_name(), TOPIC_QOS_DEFAULT);

    if (topic_ == nullptr)
    {
        return false;
    }

    //Create DataWriter
    writer_ = publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT, &m_listener);
    if (writer_ == nullptr)
    {
        return false;
    }
    std::cout << "Publisher created, waiting for Subscribers." << std::endl;
    return true;
}

void OwnershipStrengthPublisher::PubListener::on_publication_matched(
        eprosima::fastdds::dds::DataWriter*,
        const eprosima::fastdds::dds::PublicationMatchedStatus& info)
{
    if (info.current_count_change == 1)
    {
        n_matched = info.total_count;
        std::cout << "Publisher matched." << std::endl;
    }
    else if (info.current_count_change == -1)
    {
        n_matched = info.total_count;
        std::cout << "Publisher unmatched." << std::endl;
    }
    else
    {
        std::cout << info.current_count_change
                  << " is not a valid value for PublicationMatchedStatus current count change" << std::endl;
    }
}

void OwnershipStrengthPublisher::run()
{
    while (m_listener.n_matched == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    char ch = 'y';
    do
    {
        if (ch == 'y')
        {
            ExampleMessage st;
            std::stringstream ss;
            ss << "Hello with strength " << m_strength;

            st.message(ss.str());
            st.ownershipStrength(m_strength);
            st.index(m_messagesSent);
            writer_->write(&st);

            m_messagesSent++;
            std::cout << "Sending message, index = " << m_messagesSent << " with strength " << m_strength <<
                ", send another sample?(y-yes,n-stop): ";
        }
        else if (ch == 'n')
        {
            std::cout << "Stopping execution " << std::endl;
            break;
        }
        else
        {
            std::cout << "Command " << ch << " not recognized, please enter \"y/n\":";
        }

    } while (std::cin >> ch);
}
