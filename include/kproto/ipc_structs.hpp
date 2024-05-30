#include <string>
#include <vector>
#include <unordered_map>

namespace kiq {

enum class TGCommand
{
  message     = 0x00,
  poll        = 0x01,
  poll_stop   = 0x02,
  poll_result = 0x03,
  unknown     = 0x04
};
//----------------------------------------------------
static const char* REQUEST_MESSAGE            {"message"};
static const char* REQUEST_CREATE_POLL        {"poll"};
static const char* REQUEST_SCHEDULE_POLL_STOP {"poll stop"};
static const char* REQUEST_PROCESS_POLL_RESULT{"poll result"};
static const char* REQUEST_PROCESS_ROOMS      {"process rooms"};
static const char* REQUEST_GENERATE_AI        {"generate"};
//----------------------------------------------------
struct PlatformIPC
{
std::string platform;
TGCommand   command;
std::string id;
bool        complete{false};
};
//----------------------------------------------------
struct IPCSendEvent
{
int32_t                  event;
std::vector<std::string> data;

void append_msg(const std::string& s)
{
  if (data.size() > 4)
    data[4] += s;
}
};
//----------------------------------------------------
static const std::unordered_map<std::string, uint32_t> IPC_CMD_CODES
{
  { REQUEST_MESSAGE,             static_cast<uint32_t>(TGCommand::message    ) },
  { REQUEST_CREATE_POLL,         static_cast<uint32_t>(TGCommand::poll       ) },
  { REQUEST_SCHEDULE_POLL_STOP,  static_cast<uint32_t>(TGCommand::poll_stop  ) },
  { REQUEST_PROCESS_POLL_RESULT, static_cast<uint32_t>(TGCommand::poll_result) }
};
//----------------------------------------------------
static uint32_t get_ipc_cmd_code(std::string_view s)
{
  try
  {
    return IPC_CMD_CODES.at(s.data());
  }
  catch(const std::exception& e)
  {
    return static_cast<uint32_t>(TGCommand::unknown);
  }
}
//----------------------------------------------------
inline const TGCommand GetIPCCommand(const std::string& code_string)
{
  return static_cast<TGCommand>(IPC_CMD_CODES.at(code_string));
}

} // ns kiq
