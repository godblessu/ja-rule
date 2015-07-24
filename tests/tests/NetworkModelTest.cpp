/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * NetworkModelTest.cpp
 * Tests for the Network Model RDM responder.
 * Copyright (C) 2015 Simon Newton
 */

#include <gtest/gtest.h>

#include <ola/rdm/UID.h>
#include <ola/rdm/RDMCommand.h>
#include <ola/rdm/RDMEnums.h>
#include <ola/rdm/RDMCommandSerializer.h>
#include <ola/network/NetworkUtils.h>
#include <string.h>
#include <memory>

#include "rdm.h"
#include "rdm_buffer.h"
#include "network_model.h"
#include "rdm_responder.h"
#include "Array.h"
#include "Matchers.h"
#include "TestHelpers.h"

using ola::network::HostToNetwork;
using ola::rdm::UID;
using ola::rdm::GetResponseFromData;
using ola::rdm::RDMGetRequest;
using ola::rdm::NackWithReason;
using ola::rdm::RDMRequest;
using ola::rdm::RDMResponse;
using ola::rdm::RDMSetRequest;
using std::unique_ptr;

class ModelTest : public testing::Test {
 public:
  ModelTest()
      : m_controller_uid(0x7a70, 0x00000000),
        m_our_uid(TEST_UID) {
  }

 protected:
  UID m_controller_uid;
  UID m_our_uid;

  static const uint8_t TEST_UID[UID_LENGTH];

  unique_ptr<RDMRequest> BuildGetRequest(
      uint16_t pid,
      const uint8_t *param_data = nullptr,
      unsigned int param_data_size = 0) {
    return unique_ptr<RDMGetRequest>(new RDMGetRequest(
        m_controller_uid, m_our_uid, 0, 0, 0, pid, param_data,
        param_data_size));
  }

  unique_ptr<RDMRequest> BuildSetRequest(
      uint16_t pid,
      const uint8_t *param_data = nullptr,
      unsigned int param_data_size = 0) {
    return unique_ptr<RDMSetRequest>(new RDMSetRequest(
        m_controller_uid, m_our_uid, 0, 0, 0, pid, param_data,
        param_data_size));
  }

  int InvokeRDMHandler(const ola::rdm::RDMRequest *request) {
    ola::io::ByteString data;
    data.push_back(RDM_START_CODE);
    EXPECT_TRUE(ola::rdm::RDMCommandSerializer::Pack(*request, &data));

    return NETWORK_MODEL_ENTRY.request_fn(
        AsHeader(data.data()), request->ParamData());
  }
};

const uint8_t ModelTest::TEST_UID[] = {
  0x7a, 0x70, 0x12, 0x34, 0x56, 0x78
};


class NetworkModelTest : public ModelTest {
 public:
  NetworkModelTest() : ModelTest() {}

  void SetUp() {
    RDMResponder_Initialize(TEST_UID);
    NetworkModel_Initialize();
    NETWORK_MODEL_ENTRY.activate_fn();
  }
};

TEST_F(NetworkModelTest, testLifecycle) {
  EXPECT_EQ(NETWORK_MODEL_ID, NETWORK_MODEL_ENTRY.model_id);
  NETWORK_MODEL_ENTRY.tasks_fn();
  NETWORK_MODEL_ENTRY.deactivate_fn();
}

TEST_F(NetworkModelTest, listInterfaces) {
  unique_ptr<RDMRequest> request = BuildGetRequest(PID_LIST_INTERFACES);

  const uint8_t expected_response[] = {
    0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x03, 0x00, 0x1f,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x01,
  };

  unique_ptr<RDMResponse> response(GetResponseFromData(
        request.get(), expected_response, arraysize(expected_response)));

  int size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));
}

TEST_F(NetworkModelTest, getInterfaceLabel) {
  uint32_t interface_id = HostToNetwork(1);
  unique_ptr<RDMRequest> request = BuildGetRequest(PID_INTERFACE_LABEL,
      reinterpret_cast<uint8_t*>(&interface_id), sizeof(interface_id));

  const uint8_t expected_response[] = {
    0x00, 0x00, 0x00, 0x01,
    'e', 't', 'h', '0'
  };

  unique_ptr<RDMResponse> response(GetResponseFromData(
        request.get(), expected_response, arraysize(expected_response)));

  int size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));

  // test the NACK
  interface_id = HostToNetwork(2);
  request = BuildGetRequest(PID_INTERFACE_LABEL,
      reinterpret_cast<uint8_t*>(&interface_id), sizeof(interface_id));

  response.reset(NackWithReason(request.get(), ola::rdm::NR_DATA_OUT_OF_RANGE));
  size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));
}

TEST_F(NetworkModelTest, getHardwareAddress) {
  uint32_t interface_id = HostToNetwork(1);
  unique_ptr<RDMRequest> request = BuildGetRequest(
      PID_INTERFACE_HARDWARE_ADDRESS_TYPE1,
      reinterpret_cast<uint8_t*>(&interface_id), sizeof(interface_id));

  const uint8_t expected_response[] = {
    0x00, 0x00, 0x00, 0x01,
    0x52, 0x12, 0x34, 0x56, 0x78, 0x9a
  };

  unique_ptr<RDMResponse> response(GetResponseFromData(
        request.get(), expected_response, arraysize(expected_response)));

  int size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));

  // test the NACK
  interface_id = HostToNetwork(5);
  request = BuildGetRequest(PID_INTERFACE_HARDWARE_ADDRESS_TYPE1,
      reinterpret_cast<uint8_t*>(&interface_id), sizeof(interface_id));

  response.reset(NackWithReason(request.get(), ola::rdm::NR_DATA_OUT_OF_RANGE));
  size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));
}

TEST_F(NetworkModelTest, getDHCPMode) {
  uint32_t interface_id = HostToNetwork(1);
  unique_ptr<RDMRequest> request = BuildGetRequest(
      PID_IPV4_DHCP_MODE,
      reinterpret_cast<uint8_t*>(&interface_id), sizeof(interface_id));

  const uint8_t expected_response[] = {
    0x00, 0x00, 0x00, 0x01, 0x00
  };

  unique_ptr<RDMResponse> response(GetResponseFromData(
        request.get(), expected_response, arraysize(expected_response)));

  int size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));

  // test the NACK
  interface_id = HostToNetwork(4);
  request = BuildGetRequest(PID_IPV4_DHCP_MODE,
      reinterpret_cast<uint8_t*>(&interface_id), sizeof(interface_id));

  const uint8_t expected_response2[] = {
    0x00, 0x00, 0x00, 0x04, 0x01
  };
  response.reset(GetResponseFromData(
        request.get(), expected_response2, arraysize(expected_response2)));

  size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));
}

TEST_F(NetworkModelTest, getZeroconfMode) {
  uint32_t interface_id = HostToNetwork(1);
  unique_ptr<RDMRequest> request = BuildGetRequest(
      PID_IPV4_ZEROCONF_MODE,
      reinterpret_cast<uint8_t*>(&interface_id), sizeof(interface_id));

  const uint8_t expected_response[] = {
    0x00, 0x00, 0x00, 0x01, 0x00
  };

  unique_ptr<RDMResponse> response(GetResponseFromData(
        request.get(), expected_response, arraysize(expected_response)));

  int size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));

  // test the NACK
  interface_id = HostToNetwork(4);
  request = BuildGetRequest(PID_IPV4_ZEROCONF_MODE,
      reinterpret_cast<uint8_t*>(&interface_id), sizeof(interface_id));

  const uint8_t expected_response2[] = {
    0x00, 0x00, 0x00, 0x04, 0x01
  };
  response.reset(GetResponseFromData(
        request.get(), expected_response2, arraysize(expected_response2)));

  size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));
}

TEST_F(NetworkModelTest, defaultRoute) {
  const uint8_t param_data[] = {
    0x00, 0x00, 0x00, 0x00,
    0x0a, 0x0a, 0x1, 0x2
  };

  unique_ptr<RDMRequest> request = BuildSetRequest(
      PID_IPV4_DEFAULT_ROUTE, param_data, arraysize(param_data));

  unique_ptr<RDMResponse> response(GetResponseFromData(request.get()));

  int size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));

  request = BuildGetRequest(PID_IPV4_DEFAULT_ROUTE);
  response.reset(GetResponseFromData(
        request.get(), param_data, arraysize(param_data)));

  size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));
}

TEST_F(NetworkModelTest, nameservers) {
  const uint8_t ip[] = {0x1, 0x0a, 0x0a, 0x1, 0x2};

  unique_ptr<RDMRequest> request = BuildSetRequest(
      PID_DNS_NAME_SERVER, ip, arraysize(ip));

  unique_ptr<RDMResponse> response(GetResponseFromData(request.get()));

  int size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));

  uint8_t index = 1;
  request = BuildGetRequest(PID_DNS_NAME_SERVER, &index, sizeof(uint8_t));
  response.reset(GetResponseFromData(request.get(), ip, arraysize(ip)));

  size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));

  // Check we get a NACK for an out of range.
  index = 3;
  request = BuildGetRequest(PID_DNS_NAME_SERVER, &index, sizeof(uint8_t));
  response.reset(NackWithReason(request.get(), ola::rdm::NR_DATA_OUT_OF_RANGE));

  size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));
}

TEST_F(NetworkModelTest, hostname) {
  const char hostname[] = "foo";
  unique_ptr<RDMRequest> request = BuildSetRequest(
      PID_DNS_HOSTNAME,
      reinterpret_cast<const uint8_t*>(hostname), strlen(hostname));

  unique_ptr<RDMResponse> response(GetResponseFromData(request.get()));

  int size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));

  request = BuildGetRequest(PID_DNS_HOSTNAME);
  response.reset(GetResponseFromData(
        request.get(), reinterpret_cast<const uint8_t*>(hostname),
        strlen(hostname)));

  size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));
}

TEST_F(NetworkModelTest, domainName) {
  const char domain_name[] = "myco.co.nz";
  unique_ptr<RDMRequest> request = BuildSetRequest(
      PID_DNS_DOMAIN_NAME,
      reinterpret_cast<const uint8_t*>(domain_name), strlen(domain_name));

  unique_ptr<RDMResponse> response(GetResponseFromData(request.get()));

  int size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));

  request = BuildGetRequest(PID_DNS_DOMAIN_NAME);

  response.reset(GetResponseFromData(
        request.get(), reinterpret_cast<const uint8_t*>(domain_name),
        strlen(domain_name)));

  size = InvokeRDMHandler(request.get());
  EXPECT_THAT(ArrayTuple(g_rdm_buffer, size), ResponseIs(response.get()));
}
