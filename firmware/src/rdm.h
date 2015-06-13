/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * rdm.h
 * Copyright (C) 2015 Simon Newton
 */

/**
 * @defgroup rdm RDM
 * @brief Remote Device Management
 *
 * @addtogroup rdm
 * @{
 * @file rdm.h
 * @brief Remote Device Management
 */

#ifndef FIRMWARE_SRC_RDM_H_
#define FIRMWARE_SRC_RDM_H_

#include <stdbool.h>
#include <stdint.h>
#include "transceiver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The root sub device.
 */
static const uint16_t SUBDEVICE_ROOT = 0;

/**
 * @brief All sub devices.
 */
static const uint16_t SUBDEVICE_ALL = 0xffff;

/**
 * @brief The maximum index for a sub device.
 */
static const uint16_t SUBDEVICE_MAX = 0x0200;

/**
 * @brief The RDM Sub Start Code.
 */
static const uint8_t SUB_START_CODE = 0x01;

/**
 * @brief The minimum size of an RDM frame.
 */
static const uint8_t RDM_MIN_FRAME_SIZE = 26;

/**
 * @brief The location of the parameter data in a frame.
 */
static const uint8_t RDM_PARAM_DATA_OFFSET = 24;

/**
 * @brief The RDM version we support.
 */
static const uint16_t RDM_VERSION = 0x0100;

/**
 * @brief The length of UIDs.
 */
enum {
  UID_LENGTH = 6  //!< The size of a UID.
};

/**
 * @brief Maximum size of RDM Parameter Data.
 */
enum {
  MAX_PARAM_DATA_SIZE = 231  //!< Maximum size of RDM Parameter Data.
};

/**
 * @brief RDM Command Classes from E1.20.
 * @note See section 6.2.10 of ANSI E1.20 for more information.
 */
typedef enum {
  DISCOVER_COMMAND = 0x10, /**< Discovery Command */
  DISCOVER_COMMAND_RESPONSE = 0x11, /**< Discovery Response */
  GET_COMMAND = 0x20, /**< Get Command */
  GET_COMMAND_RESPONSE = 0x21, /**< Get Response */
  SET_COMMAND = 0x30, /**< Set Command */
  SET_COMMAND_RESPONSE = 0x31, /**< Set Response */
} RDMCommandClass;

/**
 * @brief RDM Command Classes from E1.20.
 * @note See section 6.2.10 of ANSI E1.20 for more information.
 */
typedef enum {
  ACK = 0x00, /**< ACK */
  ACK_TIMER = 0x01, /**< ACK Timer */
  NACK_REASON = 0x02, /**< NACK with reason */
  ACK_OVERFLOW = 0x03 /**< ACK OVERFLOW */
} RDMResponseType;

/**
 * @brief RDM Parameter IDs.
 * @note These come from E1.20, E1.37-1, E1.37-2 etc.
 */
typedef enum {
  // Discovery
  PID_DISC_UNIQUE_BRANCH = 0x0001,
  PID_DISC_MUTE = 0x0002,
  PID_DISC_UN_MUTE = 0x0003,
  // network management
  PID_PROXIED_DEVICES = 0x0010,
  PID_PROXIED_DEVICE_COUNT = 0x0011,
  PID_COMMS_STATUS = 0x0015,
  // status collection
  PID_QUEUED_MESSAGE = 0x0020,
  PID_STATUS_MESSAGES = 0x0030,
  PID_STATUS_ID_DESCRIPTION = 0x0031,
  PID_CLEAR_STATUS_ID = 0x0032,
  PID_SUB_DEVICE_STATUS_REPORT_THRESHOLD = 0x0033,
  // RDM information
  PID_SUPPORTED_PARAMETERS = 0x0050,
  PID_PARAMETER_DESCRIPTION = 0x0051,
  // production information
  PID_DEVICE_INFO = 0x0060,
  PID_PRODUCT_DETAIL_ID_LIST = 0x0070,
  PID_DEVICE_MODEL_DESCRIPTION = 0x0080,
  PID_MANUFACTURER_LABEL = 0x0081,
  PID_DEVICE_LABEL = 0x0082,
  PID_FACTORY_DEFAULTS = 0x0090,
  PID_LANGUAGE_CAPABILITIES = 0x00a0,
  PID_LANGUAGE = 0x00b0,
  PID_SOFTWARE_VERSION_LABEL = 0x00c0,
  PID_BOOT_SOFTWARE_VERSION_ID = 0x00c1,
  PID_BOOT_SOFTWARE_VERSION_LABEL = 0x00c2,
  // DMX512
  PID_DMX_PERSONALITY = 0x00e0,
  PID_DMX_PERSONALITY_DESCRIPTION = 0x00e1,
  PID_DMX_START_ADDRESS = 0x00f0,
  PID_SLOT_INFO = 0x0120,
  PID_SLOT_DESCRIPTION = 0x0121,
  PID_DEFAULT_SLOT_VALUE = 0x0122,
  // sensors
  PID_SENSOR_DEFINITION = 0x0200,
  PID_SENSOR_VALUE = 0x0201,
  PID_RECORD_SENSORS = 0x0202,
  // power/lamp settings
  PID_DEVICE_HOURS = 0x0400,
  PID_LAMP_HOURS = 0x0401,
  PID_LAMP_STRIKES = 0x0402,
  PID_LAMP_STATE = 0x0403,
  PID_LAMP_ON_MODE = 0x0404,
  PID_DEVICE_POWER_CYCLES = 0x0405,
  // display settings
  PID_DISPLAY_INVERT = 0x0500,
  PID_DISPLAY_LEVEL = 0x0501,
  // configuration
  PID_PAN_INVERT = 0x0600,
  PID_TILT_INVERT = 0x0601,
  PID_PAN_TILT_SWAP = 0x0602,
  PID_REAL_TIME_CLOCK = 0x0603,
  // control
  PID_IDENTIFY_DEVICE = 0x1000,
  PID_RESET_DEVICE = 0x1001,
  PID_POWER_STATE = 0x1010,
  PID_PERFORM_SELFTEST = 0x1020,
  PID_SELF_TEST_DESCRIPTION = 0x1021,
  PID_CAPTURE_PRESET = 0x1030,
  PID_PRESET_PLAYBACK = 0x1031,

  // E1.37-1 PIDS
  // DMX512 setup
  PID_DMX_BLOCK_ADDRESS = 0x0140,
  PID_DMX_FAIL_MODE = 0x0141,
  PID_DMX_STARTUP_MODE = 0x0142,

  // Dimmer Settings
  PID_DIMMER_INFO = 0x0340,
  PID_MINIMUM_LEVEL = 0x0341,
  PID_MAXIMUM_LEVEL = 0x0342,
  PID_CURVE = 0x0343,
  PID_CURVE_DESCRIPTION = 0x0344,

  // Control
  PID_OUTPUT_RESPONSE_TIME = 0x0345,
  PID_OUTPUT_RESPONSE_TIME_DESCRIPTION = 0x0346,
  PID_MODULATION_FREQUENCY = 0x0347,
  PID_MODULATION_FREQUENCY_DESCRIPTION = 0x0348,

  // Power/Lamp Settings
  PID_BURN_IN = 0x0440,

  // Configuration
  PID_LOCK_PIN = 0x0640,
  PID_LOCK_STATE = 0x0641,
  PID_LOCK_STATE_DESCRIPTION = 0x0642,
  PID_IDENTIFY_MODE = 0x1040,
  PID_PRESET_INFO = 0x1041,
  PID_PRESET_STATUS = 0x1042,
  PID_PRESET_MERGEMODE = 0x1043,
  PID_POWER_ON_SELF_TEST = 0x1044,

  // E1.37-2 PIDs
  PID_LIST_INTERFACES = 0x0700,
  PID_INTERFACE_LABEL = 0x0701,
  PID_INTERFACE_HARDWARE_ADDRESS_TYPE1 = 0x0702,
  PID_IPV4_DHCP_MODE = 0x0703,
  PID_IPV4_ZEROCONF_MODE = 0x0704,
  PID_IPV4_CURRENT_ADDRESS = 0x0705,
  PID_IPV4_STATIC_ADDRESS = 0x0706,
  PID_INTERFACE_RENEW_DHCP = 0x0707,
  PID_INTERFACE_RELEASE_DHCP = 0x0708,
  PID_INTERFACE_APPLY_CONFIGURATION = 0x0709,
  PID_IPV4_DEFAULT_ROUTE = 0x070a,
  PID_DNS_NAME_SERVER = 0x070b,
  PID_DNS_HOSTNAME = 0x070c,
  PID_DNS_DOMAIN_NAME = 0x070d,
} RDMPid;


/**
 * @brief RDM NACK reason codes from E1.20.
 * @note See Table A-17 from ANSI E1.20 for more information.
 */
typedef enum {
  NR_UNKNOWN_PID = 0x0000,
  NR_FORMAT_ERROR = 0x0001,
  NR_HARDWARE_FAULT = 0x0002,
  NR_PROXY_REJECT = 0x0003,
  NR_WRITE_PROTECT = 0x0004,
  NR_UNSUPPORTED_COMMAND_CLASS = 0x0005,
  NR_DATA_OUT_OF_RANGE = 0x0006,
  NR_BUFFER_FULL = 0x0007,
  NR_PACKET_SIZE_UNSUPPORTED = 0x0008,
  NR_SUB_DEVICE_OUT_OF_RANGE = 0x0009,
  NR_PROXY_BUFFER_FULL = 0x000a,
  NR_ACTION_NOT_SUPPORTED = 0x000b,
  NR_ENDPOINT_NUMBER_INVALID = 0x0011
} RDMNackReason;


/**
 * @brief RDM Product Category codes from E1.20.
 * @note See Table A-5 from ANSI E1.20 for more information.
 */
typedef enum {
  PRODUCT_CATEGORY_NOT_DECLARED = 0x0000,
  PRODUCT_CATEGORY_FIXTURE = 0x0100,
  PRODUCT_CATEGORY_FIXTURE_FIXED = 0x0101,
  PRODUCT_CATEGORY_FIXTURE_MOVING_YOKE = 0x0102,
  PRODUCT_CATEGORY_FIXTURE_MOVING_MIRROR = 0x0103,
  PRODUCT_CATEGORY_FIXTURE_OTHER = 0x01ff,
  PRODUCT_CATEGORY_FIXTURE_ACCESSORY = 0x0200,
  PRODUCT_CATEGORY_FIXTURE_ACCESSORY_COLOR = 0x0201,
  PRODUCT_CATEGORY_FIXTURE_ACCESSORY_YOKE = 0x0202,
  PRODUCT_CATEGORY_FIXTURE_ACCESSORY_MIRROR = 0x0203,
  PRODUCT_CATEGORY_FIXTURE_ACCESSORY_EFFECT = 0x0204,
  PRODUCT_CATEGORY_FIXTURE_ACCESSORY_BEAM = 0x0205,
  PRODUCT_CATEGORY_FIXTURE_ACCESSORY_OTHER = 0x02ff,
  PRODUCT_CATEGORY_PROJECTOR = 0x0300,
  PRODUCT_CATEGORY_PROJECTOR_FIXED = 0x0301,
  PRODUCT_CATEGORY_PROJECTOR_MOVING_YOKE = 0x0302,
  PRODUCT_CATEGORY_PROJECTOR_MOVING_MIRROR = 0x0303,
  PRODUCT_CATEGORY_PROJECTOR_OTHER = 0x03ff,
  PRODUCT_CATEGORY_ATMOSPHERIC = 0x0400,
  PRODUCT_CATEGORY_ATMOSPHERIC_EFFECT = 0x0401,
  PRODUCT_CATEGORY_ATMOSPHERIC_PYRO = 0x0402,
  PRODUCT_CATEGORY_ATMOSPHERIC_OTHER = 0x04ff,
  PRODUCT_CATEGORY_DIMMER = 0x0500,
  PRODUCT_CATEGORY_DIMMER_AC_INCANDESCENT = 0x0501,
  PRODUCT_CATEGORY_DIMMER_AC_FLUORESCENT = 0x0502,
  PRODUCT_CATEGORY_DIMMER_AC_COLDCATHODE = 0x0503,
  PRODUCT_CATEGORY_DIMMER_AC_NONDIM = 0x0504,
  PRODUCT_CATEGORY_DIMMER_AC_ELV = 0x0505,
  PRODUCT_CATEGORY_DIMMER_AC_OTHER = 0x0506,
  PRODUCT_CATEGORY_DIMMER_DC_LEVEL = 0x0507,
  PRODUCT_CATEGORY_DIMMER_DC_PWM = 0x0508,
  PRODUCT_CATEGORY_DIMMER_CS_LED = 0x0509,
  PRODUCT_CATEGORY_DIMMER_OTHER = 0x05ff,
  PRODUCT_CATEGORY_POWER = 0x0600,
  PRODUCT_CATEGORY_POWER_CONTROL = 0x0601,
  PRODUCT_CATEGORY_POWER_SOURCE = 0x0602,
  PRODUCT_CATEGORY_POWER_OTHER = 0x06ff,
  PRODUCT_CATEGORY_SCENIC = 0x0700,
  PRODUCT_CATEGORY_SCENIC_DRIVE = 0x0701,
  PRODUCT_CATEGORY_SCENIC_OTHER = 0x07ff,
  PRODUCT_CATEGORY_DATA = 0x0800,
  PRODUCT_CATEGORY_DATA_DISTRIBUTION = 0x0801,
  PRODUCT_CATEGORY_DATA_CONVERSION = 0x0802,
  PRODUCT_CATEGORY_DATA_OTHER = 0x08ff,
  PRODUCT_CATEGORY_AV = 0x0900,
  PRODUCT_CATEGORY_AV_AUDIO = 0x0901,
  PRODUCT_CATEGORY_AV_VIDEO = 0x0902,
  PRODUCT_CATEGORY_AV_OTHER = 0x09ff,
  PRODUCT_CATEGORY_MONITOR = 0x0a00,
  PRODUCT_CATEGORY_MONITOR_ACLINEPOWER = 0x0a01,
  PRODUCT_CATEGORY_MONITOR_DCPOWER = 0x0a02,
  PRODUCT_CATEGORY_MONITOR_ENVIRONMENTAL = 0x0a03,
  PRODUCT_CATEGORY_MONITOR_OTHER = 0x0aff,
  PRODUCT_CATEGORY_CONTROL = 0x7000,
  PRODUCT_CATEGORY_CONTROL_CONTROLLER = 0x7001,
  PRODUCT_CATEGORY_CONTROL_BACKUPDEVICE = 0x7002,
  PRODUCT_CATEGORY_CONTROL_OTHER = 0x70ff,
  PRODUCT_CATEGORY_TEST = 0x7100,
  PRODUCT_CATEGORY_TEST_EQUIPMENT = 0x7101,
  PRODUCT_CATEGORY_TEST_EQUIPMENT_OTHER = 0x71ff,
  PRODUCT_CATEGORY_OTHER = 0x7fff,
} RDMProductCategory;

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif  // FIRMWARE_SRC_RDM_H_