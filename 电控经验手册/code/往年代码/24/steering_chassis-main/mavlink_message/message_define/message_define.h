/** @file
 *  @brief MAVLink comm protocol generated from message_define.xml
 *  @see http://mavlink.org
 */
#pragma once
#ifndef MAVLINK_MESSAGE_DEFINE_H
#define MAVLINK_MESSAGE_DEFINE_H

#ifndef MAVLINK_H
    #error Wrong include order: MAVLINK_MESSAGE_DEFINE.H MUST NOT BE DIRECTLY USED. Include mavlink.h from the same directory instead or set ALL AND EVERY defines from MAVLINK.H manually accordingly, including the #define MAVLINK_H call.
#endif

#define MAVLINK_MESSAGE_DEFINE_XML_HASH 8443563384938388556

#ifdef __cplusplus
extern "C" {
#endif

// MESSAGE LENGTHS AND CRCS

#ifndef MAVLINK_MESSAGE_LENGTHS
#define MAVLINK_MESSAGE_LENGTHS {}
#endif

#ifndef MAVLINK_MESSAGE_CRCS
#define MAVLINK_MESSAGE_CRCS {{1, 51, 24, 24, 0, 0, 0}, {2, 112, 9, 9, 0, 0, 0}}
#endif

#include "../protocol.h"

#define MAVLINK_ENABLED_MESSAGE_DEFINE

// ENUM DEFINITIONS



// MAVLINK VERSION

#ifndef MAVLINK_VERSION
#define MAVLINK_VERSION 3
#endif

#if (MAVLINK_VERSION == 0)
#undef MAVLINK_VERSION
#define MAVLINK_VERSION 3
#endif

// MESSAGE DEFINITIONS
#include "./mavlink_msg_speed.h"
#include "./mavlink_msg_heartbeat.h"

// base include



#if MAVLINK_MESSAGE_DEFINE_XML_HASH == MAVLINK_PRIMARY_XML_HASH
# define MAVLINK_MESSAGE_INFO {MAVLINK_MESSAGE_INFO_SPEED, MAVLINK_MESSAGE_INFO_HEARTBEAT}
# define MAVLINK_MESSAGE_NAMES {{ "HEARTBEAT", 2 }, { "SPEED", 1 }}
# if MAVLINK_COMMAND_24BIT
#  include "../mavlink_get_info.h"
# endif
#endif

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MAVLINK_MESSAGE_DEFINE_H
