/*
 * File:   transport.h
 * Author: Simon Newton
 */

/**
 * @defgroup transport Transport
 * @brief The Host <-> Device communication transport.
 *
 * This contains the types used to transfer messages between the Host
 * (typically a machine running a full OS) and the PIC device.
 *
 * @addtogroup transport
 * @{
 * @file transport.h
 * @brief The Host <-> Device communication transport.
 */

#ifndef FIRMWARE_SRC_TRANSPORT_H_
#define FIRMWARE_SRC_TRANSPORT_H_

#include <stdint.h>
#include "constants.h"
#include "iovec.h"

/**
 * @brief Flags use in a response message.
 */
typedef enum {
  TRANSPORT_LOGS_PENDING = 0x01,  //!< Log messages are pending
  TRANSPORT_FLAGS_CHANGED = 0x02,  //!< Flags have changed
  TRANSPORT_MSG_TRUNCATED = 0x04  //!< The message has been truncated.
} TransportFlags;

/**
 * @brief A function pointer to send a message to the host
 * @param token The frame token, this should match the request.
 * @param command the Command indentifier to send
 * @param rc The 8-bit return code.
 * @param iov A pointer to an array of IOVec structures. The data will be
 *   copied.
 * @param iov_count The number of IOVec structures in the array.
 */
typedef bool (*TransportTXFunction)(uint8_t, Command, uint8_t, const IOVec*,
                                    unsigned int);

/**
 * @brief A function pointer to call when data is received from the host.
 * @param data A pointer to the new data.
 * @param size The size of the data received.
 */
typedef void (*TransportRxFunction)(const uint8_t*, unsigned int);

#endif  // FIRMWARE_SRC_TRANSPORT_H_

/**
 * @}
 */