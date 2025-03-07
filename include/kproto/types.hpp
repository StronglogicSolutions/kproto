#pragma once

#include <string>
#include <cstring>

namespace kiq::Request {

const uint8_t REQUEST_TYPE_INDEX = 0x00;

enum RequestType {
  REGISTER_APPLICATION  = 0x00,
  UPDATE_APPLICATION    = 0x01,
  REMOVE_APPLICATION    = 0x02,
  GET_APPLICATION       = 0x03,
  FETCH_SCHEDULE        = 0x04,
  UPDATE_SCHEDULE       = 0x05,
  FETCH_SCHEDULE_TOKENS = 0x06,
  TRIGGER_CREATE        = 0x07,
  TASK_FLAGS            = 0x08,
  FETCH_FILE            = 0x09,
  FETCH_FILE_ACK        = 0x0A,
  FETCH_FILE_READY      = 0x0B,
  FETCH_TASK_DATA       = 0x0C,
  START_SESSION         = 0x0D,
  STOP_SESSION          = 0x0E,
  EXECUTE_PROCESS       = 0X0F,
  UPLOAD_FILE           = 0x10,
  SCHEDULE_TASK         = 0x11,
  IPC_REQUEST           = 0x12,
  FETCH_TERM_HITS       = 0x13,
  EXECUTE               = 0x14,
  FETCH_POSTS           = 0x15,
  UPDATE_POST           = 0x16,
  KIQ_STATUS            = 0x17,
  CONVERT_TASK          = 0x18,
  REPOST_PLATFORM       = 0x19,
  RECONNECT_IPC         = 0x1A,
  KAI_TASK              = 0x1B,
  UNKNOWN               = 0x1C
};

/**
 * int_to_request_type
 *
 * @param [in] {int} Signed integer should represent a byte value
 */
inline RequestType int_to_request_type(int byte)
{
  switch (byte)
  {
    case REGISTER_APPLICATION:  return REGISTER_APPLICATION;
    case UPDATE_APPLICATION:    return UPDATE_APPLICATION;
    case REMOVE_APPLICATION:    return REMOVE_APPLICATION;
    case GET_APPLICATION:       return GET_APPLICATION;
    case FETCH_SCHEDULE:        return FETCH_SCHEDULE;
    case UPDATE_SCHEDULE:       return UPDATE_SCHEDULE;
    case FETCH_SCHEDULE_TOKENS: return FETCH_SCHEDULE_TOKENS;
    case TRIGGER_CREATE:        return TRIGGER_CREATE;
    case TASK_FLAGS:            return TASK_FLAGS;
    case FETCH_FILE:            return FETCH_FILE;
    case FETCH_FILE_ACK:        return FETCH_FILE_ACK;
    case FETCH_FILE_READY:      return FETCH_FILE_READY;
    case FETCH_TASK_DATA:       return FETCH_TASK_DATA;
    case START_SESSION:         return START_SESSION;
    case STOP_SESSION:          return STOP_SESSION;
    case EXECUTE_PROCESS:       return EXECUTE_PROCESS;
    case UPLOAD_FILE:           return UPLOAD_FILE;
    case SCHEDULE_TASK:         return SCHEDULE_TASK;
    case IPC_REQUEST:           return IPC_REQUEST;
    case FETCH_TERM_HITS:       return FETCH_TERM_HITS;
    case EXECUTE:               return EXECUTE;
    case FETCH_POSTS:           return FETCH_POSTS;
    case UPDATE_POST:           return UPDATE_POST;
    case KIQ_STATUS:            return KIQ_STATUS;
    case CONVERT_TASK:          return CONVERT_TASK;
    case REPOST_PLATFORM:       return REPOST_PLATFORM;
    case RECONNECT_IPC:         return RECONNECT_IPC;
    case KAI_TASK:              return KAI_TASK;
  }
  return UNKNOWN;
}

inline std::string request_type_to_string(RequestType type)
{
  switch (type)
  {
    case REGISTER_APPLICATION:  return "REGISTER_APPLICATION";
    case UPDATE_APPLICATION:    return "UPDATE_APPLICATION";
    case REMOVE_APPLICATION:    return "REMOVE_APPLICATION";
    case GET_APPLICATION:       return "GET_APPLICATION";
    case FETCH_SCHEDULE:        return "FETCH_SCHEDULE";
    case UPDATE_SCHEDULE:       return "UPDATE_SCHEDULE";
    case FETCH_SCHEDULE_TOKENS: return "FETCH_SCHEDULE_TOKENS";
    case TRIGGER_CREATE:        return "TRIGGER_CREATE";
    case TASK_FLAGS:            return "TASK_FLAGS";
    case FETCH_FILE:            return "FETCH_FILE";
    case FETCH_FILE_ACK:        return "FETCH_FILE_ACK";
    case FETCH_FILE_READY:      return "FETCH_FILE_READY";
    case FETCH_TASK_DATA:       return "FETCH_TASK_DATA";
    case START_SESSION:         return "START_SESSION";
    case STOP_SESSION:          return "STOP_SESSION";
    case EXECUTE_PROCESS:       return "EXECUTE_PROCESS";
    case UPLOAD_FILE:           return "UPLOAD_FILE";
    case SCHEDULE_TASK:         return "SCHEDULE_TASK";
    case IPC_REQUEST:           return "IPC_REQUEST";
    case FETCH_TERM_HITS:       return "FETCH_TERM_HITS";
    case EXECUTE:               return "EXECUTE";
    case FETCH_POSTS:           return "FETCH_POSTS";
    case UPDATE_POST:           return "UPDATE_POST";
    case KIQ_STATUS:            return "KIQ_STATUS";
    case CONVERT_TASK:          return "CONVERT_TASK";
    case REPOST_PLATFORM:       return "REPOST_PLATFORM";
    case RECONNECT_IPC:         return "RECONNECT_IPC";
    case KAI_TASK:              return "KAI_TASK";
  }
  return "UNKNOWN";
}
} // ns kiq::Request
