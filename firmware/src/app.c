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
 * app.c
 * Copyright (C) 2015 Simon Newton
 */

#include "app.h"

#include "sys/attribs.h"

#include "coarse_timer.h"
#include "logger.h"
#include "message_handler.h"
#include "rdm.h"
#include "rdm_responder.h"
#include "stream_decoder.h"
#include "syslog.h"
#include "system_definitions.h"
#include "system_settings.h"
#include "transceiver.h"
#include "usb_transport.h"

// TODO(simon): figure out how to allocate UIDs.
const uint8_t OUR_UID[UID_LENGTH] = {0x7a, 0x70, 0xff, 0xff, 0xfe, 0};

void __ISR(_TIMER_2_VECTOR, ipl6) TimerEvent() {
  CoarseTimer_TimerEvent();
}

void APP_Initialize(void) {
  CoarseTimer_Settings timer_settings = {
    .timer_id = TMR_ID_2,
    .interrupt_source = INT_SOURCE_TIMER_2
  };
  SYS_INT_VectorPrioritySet(INT_VECTOR_T2, INT_PRIORITY_LEVEL6);
  CoarseTimer_Initialize(&timer_settings);

  // Initialize the Logging system, bottom up
  USBTransport_Initialize(NULL);
  USBConsole_Initialize();
  SysLog_Initialize(NULL);

  // Initialize the DMX / RDM Transceiver
  TransceiverHardwareSettings transceiver_settings = {
    .usart = TRANSCEIVER_UART,
    .port = TRANSCEIVER_PORT,
    .break_bit = TRANSCEIVER_PORT_BIT,
    .rx_enable_bit = TRANSCEIVER_TX_ENABLE,
    .tx_enable_bit = TRANSCEIVER_RX_ENABLE,
  };
  Transceiver_Initialize(&transceiver_settings, NULL);

  RDMResponderSettings responder_settings = {
    .identify_port = RDM_RESPONDER_PORT,
    .identify_bit = RDM_RESPONDER_IDENTIFY_PORT_BIT,
    .mute_port = RDM_RESPONDER_PORT,
    .mute_bit = RDM_RESPONDER_MUTE_PORT_BIT
  };
  memcpy(responder_settings.uid, OUR_UID, UID_LENGTH);
  RDMResponder_Initialize(&responder_settings, NULL);

  // Initialize the Host message layers.
  MessageHandler_Initialize(NULL);
  StreamDecoder_Initialize(NULL);

  Flags_Initialize();
  // TODO(simon): remove this.
  Logger_Initialize(NULL, PAYLOAD_SIZE);
  Logger_SetState(true);
}

void APP_Tasks(void) {
  USBTransport_Tasks();
  Transceiver_Tasks();
  USBConsole_Tasks();
  RDMResponder_Tasks();
}

void APP_Reset() {
  Transceiver_Reset();
  SysLog_Message(SYSLOG_INFO, "Reset Device");
  USBTransport_SoftReset();
}
