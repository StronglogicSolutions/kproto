#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>
#include <thread>
#include <future>
#include <zmq.hpp>

namespace kiq {
using external_log_fn = std::function<void(const char*)>;
namespace
{
  static void noop(const char*) { (void)"NOOP"; }
  external_log_fn log_fn = noop;
} // ns

inline void set_log_fn(external_log_fn fn)
{
  log_fn = fn;
}
/**
            ┌───────────────────────────────────────────────┐
            │░░░░░░░░░░░░░░░ PROTOCOL ░░░░░░░░░░░░░░░░░░░░░░░│
            │░░░░░░░░        1. Empty              ░░░░░░░░░░░│
            │░░░░░░░░        2. Type               ░░░░░░░░░░░│
            │░░░░░░░░        3. Data               ░░░░░░░░░░░│
            │░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
            └───────────────────────────────────────────────┘
 */

namespace constants {
static const uint8_t IPC_OK_TYPE         {0x00};
static const uint8_t IPC_KEEPALIVE_TYPE  {0x01};
static const uint8_t IPC_KIQ_MESSAGE     {0x02};
static const uint8_t IPC_PLATFORM_TYPE   {0x03};
static const uint8_t IPC_PLATFORM_ERROR  {0x04};
static const uint8_t IPC_PLATFORM_REQUEST{0x05};
static const uint8_t IPC_PLATFORM_INFO   {0x06};
static const uint8_t IPC_FAIL_TYPE       {0x07};
static const uint8_t IPC_STATUS          {0x08};
static const uint8_t IPC_TASK_TYPE       {0x09};

static const std::unordered_map<uint8_t, const char*> IPC_MESSAGE_NAMES{
  {IPC_OK_TYPE,          "IPC_OK_TYPE"},
  {IPC_KEEPALIVE_TYPE,   "IPC_KEEPALIVE_TYPE"},
  {IPC_KIQ_MESSAGE,      "IPC_KIQ_MESSAGE"},
  {IPC_PLATFORM_TYPE,    "IPC_PLATFORM_TYPE"},
  {IPC_PLATFORM_ERROR,   "IPC_PLATFORM_ERROR"},
  {IPC_PLATFORM_REQUEST, "IPC_PLATFORM_REQUEST"},
  {IPC_PLATFORM_INFO,    "IPC_PLATFORM_INFO"},
  {IPC_FAIL_TYPE,        "IPC_FAIL_TYPE"},
  {IPC_STATUS,           "IPC_STATUS"},
  {IPC_TASK_TYPE,        "IPC_TASK_TYPE"}
};

static const std::unordered_map<std::string, uint8_t> IPC_MESSAGE_VALUES{
  {"IPC_OK_TYPE",          IPC_OK_TYPE},
  {"IPC_KEEPALIVE_TYPE",   IPC_KEEPALIVE_TYPE},
  {"IPC_KIQ_MESSAGE",      IPC_KIQ_MESSAGE},
  {"IPC_PLATFORM_TYPE",    IPC_PLATFORM_TYPE},
  {"IPC_PLATFORM_ERROR",   IPC_PLATFORM_ERROR},
  {"IPC_PLATFORM_REQUEST", IPC_PLATFORM_REQUEST},
  {"IPC_PLATFORM_INFO",    IPC_PLATFORM_INFO},
  {"IPC_FAIL_TYPE",        IPC_FAIL_TYPE},
  {"IPC_STATUS",           IPC_STATUS}
};

namespace index {
static const uint8_t EMPTY     = 0x00;
static const uint8_t TYPE      = 0x01;
static const uint8_t PLATFORM  = 0x02;
static const uint8_t ID        = 0x03;
static const uint8_t INFO      = 0x04;
static const uint8_t INFO_TYPE = 0x05;
static const uint8_t USER      = 0x04;
static const uint8_t DATA      = 0x05;
static const uint8_t URLS      = 0x06;
static const uint8_t REQ_ARGS  = 0x06;
static const uint8_t REPOST    = 0x07;
static const uint8_t ARGS      = 0x08;
static const uint8_t CMD       = 0x09;
static const uint8_t TIME      = 0x0A;
static const uint8_t KIQ_DATA  = 0x03;
static const uint8_t ERROR     = 0x05;
static const uint8_t DESCRIPT  = 0x04;
static const uint8_t TASK_TYPE = 0x05;
static const uint8_t TECH      = 0x06;
static const uint8_t LOGS      = 0x07;
} // namespace index

static const uint8_t TELEGRAM_COMMAND_INDEX = 0x00;
static const uint8_t MASTODON_COMMAND_INDEX = 0x01;
static const uint8_t DISCORD_COMMAND_INDEX  = 0x02;
static const uint8_t YOUTUBE_COMMAND_INDEX  = 0x03;
static const uint8_t NO_COMMAND_INDEX       = 0x04;

static const char*   IPC_COMMANDS[]{
  "telegram:messages",
  "mastodon:comments",
  "discord:messages",
  "youtube:livestream",
  "no:command"
};

static const unsigned char KIQ_NAME[] = {'K', 'I', 'Q'};

} // namespace constants
inline auto IsKeepAlive = [](auto type) { return type == constants::IPC_KEEPALIVE_TYPE; };
//---------------------------------------------------------------------
class ipc_message
{
public:
using byte_buffer   = std::vector<uint8_t>;
using u_ipc_msg_ptr = std::unique_ptr<ipc_message>;

ipc_message() = default;
//--------------------
ipc_message(const ipc_message& msg)
{
  m_frames = msg.m_frames;
}
//--------------------
virtual ~ipc_message() {}
//--------------------
uint8_t type() const
{
  return m_frames.at(constants::index::TYPE).front();
}
//--------------------
std::vector<byte_buffer> data()
{
  return m_frames;
}
//--------------------
std::vector<byte_buffer> m_frames;
//--------------------
virtual std::string to_string() const
{
  return constants::IPC_MESSAGE_NAMES.at(type());
}
//--------------------
static u_ipc_msg_ptr clone(const ipc_message& msg)
{
  return std::make_unique<ipc_message>(msg);
}
};
//---------------------------------------------------------------------
class platform_error : public ipc_message
{
public:
platform_error(const std::string& name, const std::string& id, const std::string& user, const std::string& error)
{
  m_frames = {
    byte_buffer{},
    byte_buffer{constants::IPC_PLATFORM_ERROR},
    byte_buffer{name.data(), name.data() + name.size()},
    byte_buffer{id.data(), id.data() + id.size()},
    byte_buffer{user.data(), user.data() + user.size()},
    byte_buffer{error.data(), error.data() + error.size()}
  };
}
//---------------------------------------------------------------------
platform_error(const std::vector<byte_buffer>& data)
{
  m_frames = {
    byte_buffer{},
    byte_buffer{data.at(constants::index::TYPE)},
    byte_buffer{data.at(constants::index::PLATFORM)},
    byte_buffer{data.at(constants::index::ID)},
    byte_buffer{data.at(constants::index::USER)},
    byte_buffer{data.at(constants::index::ERROR)}
  };
}
//--------------------
const std::string name() const
{
  return std::string{
    reinterpret_cast<const char*>(m_frames.at(constants::index::PLATFORM).data()),
    m_frames.at(constants::index::PLATFORM).size()
  };
}
//--------------------
const std::string user() const
{
  return std::string{
    reinterpret_cast<const char*>(m_frames.at(constants::index::USER).data()),
    m_frames.at(constants::index::USER).size()
  };
}
//--------------------
const std::string error() const
{
  return std::string{
    reinterpret_cast<const char*>(m_frames.at(constants::index::ERROR).data()),
    m_frames.at(constants::index::ERROR).size()
  };
}
//--------------------
const std::string id() const
{
  return std::string{
    reinterpret_cast<const char*>(m_frames.at(constants::index::ID).data()),
    m_frames.at(constants::index::ID).size()
  };
}
//--------------------
std::string to_string() const override
{
  return  "(Type): "     + ipc_message::to_string() + ',' +
          "(Platform): " + name()                   + ',' +
          "(ID):"        + id()                     + ',' +
          "(User): "     + user()                   + ',' +
          "(Error):"     + error();
}
};
//---------------------------------------------------------------------
class okay_message : public ipc_message
{
public:
  okay_message(const std::string& platform = "", const std::string id = "")
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{constants::IPC_OK_TYPE},
      byte_buffer{platform.data(), platform.data() + platform.size()},
      byte_buffer{id.data(), id.data() + id.size() }};
  }
//--------------------
  okay_message(const std::vector<byte_buffer>& data)
  {
    m_frames = {
    byte_buffer{},
    byte_buffer{data.at(constants::index::TYPE)},
    byte_buffer{data.at(constants::index::PLATFORM)},
    byte_buffer{data.at(constants::index::ID)}};
  }
//--------------------
  virtual ~okay_message() override {}
//--------------------
  std::string id() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::ID).data()),
      m_frames.at(constants::index::ID).size()
    };
  }
};
//---------------------------------------------------------------------
class fail_message : public ipc_message
{
public:
  fail_message(const std::string& platform = "", const std::string id = "")
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{constants::IPC_FAIL_TYPE},
      byte_buffer{platform.data(), platform.data() + platform.size()},
      byte_buffer{id.data(), id.data() + id.size() }};
  }
//--------------------
  fail_message(const std::vector<byte_buffer>& data)
  {
    m_frames = {
    byte_buffer{},
    byte_buffer{data.at(constants::index::TYPE)},
    byte_buffer{data.at(constants::index::PLATFORM)},
    byte_buffer{data.at(constants::index::ID)}};
  }
//--------------------
  virtual ~fail_message() override {}
//--------------------
  std::string id() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::ID).data()),
      m_frames.at(constants::index::ID).size()
    };
  }
};
//---------------------------------------------------------------------
class keepalive : public ipc_message
{
public:
  keepalive()
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{constants::IPC_KEEPALIVE_TYPE}
    };
  }
//--------------------
  virtual ~keepalive() override {}
};
//---------------------------------------------------------------------
class kiq_message : public ipc_message
{
public:
  kiq_message(const std::string& payload, const std::string& platform = "")
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{constants::IPC_KIQ_MESSAGE},
      byte_buffer{platform.data(), platform.data() + platform.size()},
      byte_buffer{payload.data(), payload.data() + payload.size()}
    };
  }
//--------------------
  kiq_message(const std::vector<byte_buffer>& data)
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{data.at(constants::index::TYPE)},
      byte_buffer{data.at(constants::index::PLATFORM)},
      byte_buffer{data.at(constants::index::KIQ_DATA)}
    };
  }
  //--------------------
  const std::string platform() const
  {
    return {reinterpret_cast<const char*>(m_frames.at(constants::index::PLATFORM).data()),
            m_frames.at(constants::index::PLATFORM).size()};
  }
  //--------------------
  const std::string payload() const
  {
    return {reinterpret_cast<const char*>(m_frames.at(constants::index::KIQ_DATA).data()),
            m_frames.at(constants::index::KIQ_DATA).size()};
  }
//--------------------
  std::string to_string() const override
  {
    return  "(Type): "     + ipc_message::to_string() + ',' +
            "(Platform): " + platform() + ',' +
            "(Payload): "  + payload();
  }

};
//---------------------------------------------------------------------
class task : public ipc_message
{
public:
  task(const std::string& id, const std::string& desc, const std::string& type, const std::string& tech, const std::string& logs)
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{constants::IPC_TASK_TYPE},
      byte_buffer{constants::KIQ_NAME, constants::KIQ_NAME + 3},
      byte_buffer{id.data(), id.data() + id.size()},
      byte_buffer{desc.data(), desc.data() + desc.size()},
      byte_buffer{type.data(), type.data() + type.size()},
      byte_buffer{tech.data(),    tech.data() + tech.size()},
      byte_buffer{logs.data(), logs.data() + logs.size()}
    };
  }
//--------------------
  task(const std::vector<byte_buffer>& data)
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{data.at(constants::index::TYPE)},
      byte_buffer{data.at(constants::index::PLATFORM)},
      byte_buffer{data.at(constants::index::ID)},
      byte_buffer{data.at(constants::index::DESCRIPT)},
      byte_buffer{data.at(constants::index::INFO_TYPE)},
      byte_buffer{data.at(constants::index::TECH)},
      byte_buffer{data.at(constants::index::LOGS)}
    };
  }
//--------------------
  virtual ~task() override {}
//--------------------
  std::string platform() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::PLATFORM).data()),
      m_frames.at(constants::index::PLATFORM).size()
    };
  }
//--------------------
  std::string id() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::ID).data()),
      m_frames.at(constants::index::ID).size()
    };
  }
//--------------------
  std::string description() const // TODO: Change tehse methods
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::DESCRIPT).data()),
      m_frames.at(constants::index::DESCRIPT).size()
    };
  }
//--------------------
  std::string task_type() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::INFO_TYPE).data()),
      m_frames.at(constants::index::INFO_TYPE).size()
    };
  }
//--------------------
  std::string tech() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::TECH).data()),
      m_frames.at(constants::index::TECH).size()
    };
  }
//--------------------
  std::string logs() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::LOGS).data()),
      m_frames.at(constants::index::LOGS).size()
    };
  }
//--------------------
  std::string to_string() const override
  {
    return  "(Type):"        + ipc_message::to_string()    + ',' +
            "(Platform):"    + platform()             + ',' +
            "(ID):"          + id()                   + ',' +
            "(Description):" + description()          + ',' +
            "(TYPE):"        + task_type()            + ',' +
            "(TECH:):"       + tech()                 + ',' +
            "(LOGS):"        + logs();
  }
};
//---------------------------------------------------------------------
class platform_message : public ipc_message
{
public:
  platform_message(const std::string& platform, const std::string& id, const std::string& user, const std::string& content, const std::string& urls, const bool repost = false, uint32_t cmd = 0x00, const std::string& args = "", const std::string& time = "")
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{constants::IPC_PLATFORM_TYPE},
      byte_buffer{platform.data(), platform.data() + platform.size()},
      byte_buffer{id.data(), id.data() + id.size()},
      byte_buffer{user.data(), user.data() + user.size()},
      byte_buffer{content.data(), content.data() + content.size()},
      byte_buffer{urls.data(),    urls.data() + urls.size()},
      byte_buffer{static_cast<uint8_t>(repost)},
      byte_buffer{args.data(), args.data() + args.size()},
      byte_buffer{static_cast<unsigned char>((cmd >> 24) & 0xFF),
                  static_cast<unsigned char>((cmd >> 16) & 0xFF),
                  static_cast<unsigned char>((cmd >> 8 ) & 0xFF),
                  static_cast<unsigned char>((cmd      ) & 0xFF)},
      byte_buffer{time.data(), time.data() + time.size()},
    };
  }
//--------------------
  platform_message(const std::vector<byte_buffer>& data)
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{data.at(constants::index::TYPE)},
      byte_buffer{data.at(constants::index::PLATFORM)},
      byte_buffer{data.at(constants::index::ID)},
      byte_buffer{data.at(constants::index::USER)},
      byte_buffer{data.at(constants::index::DATA)},
      byte_buffer{data.at(constants::index::URLS)},
      byte_buffer{data.at(constants::index::REPOST)},
      byte_buffer{data.at(constants::index::ARGS)},
      byte_buffer{data.at(constants::index::CMD)},
      byte_buffer{data.at(constants::index::TIME)}
    };
  }
//--------------------
  virtual ~platform_message() override {}
//--------------------
  std::string platform() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::PLATFORM).data()),
      m_frames.at(constants::index::PLATFORM).size()
    };
  }
//--------------------
  std::string id() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::ID).data()),
      m_frames.at(constants::index::ID).size()
    };
  }
//--------------------
  std::string user() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::USER).data()),
      m_frames.at(constants::index::USER).size()
    };
  }
//--------------------
  std::string content() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::DATA).data()),
      m_frames.at(constants::index::DATA).size()
    };
  }
//--------------------
  std::string urls() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::URLS).data()),
      m_frames.at(constants::index::URLS).size()
    };
  }
//--------------------
  bool repost() const
  {
    return (m_frames.at(constants::index::REPOST).front() != 0x00);
  }
//--------------------
  std::string args() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::ARGS).data()),
      m_frames.at(constants::index::ARGS).size()
    };
  }
//--------------------
  uint32_t cmd() const
  {
    auto bytes = m_frames.at(constants::index::CMD).data();
    auto cmd   = static_cast<uint32_t>(bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3]);

    return cmd;
  }
//--------------------
  std::string time() const
  {
    return std::string{reinterpret_cast<const char*>(m_frames.at(constants::index::TIME).data()),
                                                     m_frames.at(constants::index::TIME).size()};
  }
//--------------------
  std::string to_string() const override
  {
    auto text = content();
    if (text.size() > 120) text = text.substr(0, 120);
    return  "(Type):" + ipc_message::to_string()   + ',' +
            "(Platform):" + platform()             + ',' +
            "(ID):" + id()                         + ',' +
            "(User):" + user()                     + ',' +
            "(Content):" + text                    + ',' +
            "(URLS):" + urls()                     + ',' +
            "(Repost):" + std::to_string(repost()) + ',' +
            "(Args):" + args()                     + ',' +
            "(Cmd):" + std::to_string(cmd())       + ',' +
            "(Time):" + time();
  }
};
//---------------------------------------------------------------------
class platform_request : public ipc_message
{
public:
  platform_request(const std::string& platform, const std::string& id, const std::string& user, const std::string& data, const std::string& args)
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{constants::IPC_PLATFORM_REQUEST},
      byte_buffer{platform.data(), platform.data() + platform.size()},
      byte_buffer{id.data(), id.data() + id.size()},
      byte_buffer{user.data(), user.data() + user.size()},
      byte_buffer{data.data(), data.data() + data.size()},
      byte_buffer{args.data(), args.data() + args.size()}
    };
  }
//--------------------
  platform_request(const std::vector<byte_buffer>& data)
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{data.at(constants::index::TYPE)},
      byte_buffer{data.at(constants::index::PLATFORM)},
      byte_buffer{data.at(constants::index::ID)},
      byte_buffer{data.at(constants::index::USER)},
      byte_buffer{data.at(constants::index::DATA)},
      byte_buffer{data.at(constants::index::REQ_ARGS)}
    };
  }
//--------------------
  const std::string platform() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::PLATFORM).data()),
      m_frames.at(constants::index::PLATFORM).size()
    };
  }
//--------------------
  const std::string id() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::ID).data()),
      m_frames.at(constants::index::ID).size()
    };
  }
//--------------------
  const std::string user() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::USER).data()),
      m_frames.at(constants::index::USER).size()
    };
  }
//--------------------
  const std::string content() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::DATA).data()),
      m_frames.at(constants::index::DATA).size()
    };
  }
//--------------------
  const std::string args() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::REQ_ARGS).data()),
      m_frames.at(constants::index::REQ_ARGS).size()
    };
  }
//--------------------
  std::string to_string() const override
  {
    auto text = content();
    if (text.size() > 120) text = text.substr(0, 120);
    return  "(Type): "     + ipc_message::to_string() + ',' +
            "(Platform): " + platform()               + ',' +
            "(ID): "       + id()                     + ',' +
            "(User): "     + user()                   + ',' +
            "(Content): "  + text                     + ',' +
            "(Args): "     + args();
  }
};
//---------------------------------------------------------------------
class platform_info : public ipc_message
{
public:
  platform_info(const std::string& platform, const std::string& info, const std::string& type, const std::string& id)
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{constants::IPC_PLATFORM_INFO},
      byte_buffer{platform.data(), platform.data() + platform.size()},
      byte_buffer{id.data(),       id.data()       + id.size()},
      byte_buffer{info.data(),     info.data()     + info.size()},
      byte_buffer{type.data(),     type.data()     + type.size()}
    };
  }
//--------------------
  platform_info(const std::vector<byte_buffer>& data)
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{data.at(constants::index::TYPE)},
      byte_buffer{data.at(constants::index::PLATFORM)},
      byte_buffer{data.at(constants::index::ID)},
      byte_buffer{data.at(constants::index::INFO)},
      byte_buffer{data.at(constants::index::INFO_TYPE)}
    };
  }
//--------------------
  const std::string platform() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::PLATFORM).data()),
      m_frames.at(constants::index::PLATFORM).size()
    };
  }
//--------------------
  std::string id() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::ID).data()),
      m_frames.at(constants::index::ID).size()
    };
  }
//--------------------
const std::string info() const
{
  return std::string{
    reinterpret_cast<const char*>(m_frames.at(constants::index::INFO).data()),
    m_frames.at(constants::index::INFO).size()
  };
}
//--------------------
  const std::string type() const
  {
    return std::string{
      reinterpret_cast<const char*>(m_frames.at(constants::index::INFO_TYPE).data()),
      m_frames.at(constants::index::INFO_TYPE).size()
    };
  }
//--------------------
  std::string to_string() const override
  {
    return  "(Type):"    + ipc_message::to_string() + ',' +
            "(Platform)" + platform()               + ',' +
            "(ID)"       + id()               + ',' +
            "(Type):"    + type()                   + ',' +
            "(Info):"    + info();
  }
};
//---------------------------------------------------------------------
class status_check : public ipc_message
{
public:
  status_check()
  {
    m_frames = {
      byte_buffer{},
      byte_buffer{constants::IPC_STATUS}
    };
  }
//--------------------
  virtual ~status_check() override = default;
};
//---------------------------------------------------------------------
inline ipc_message::u_ipc_msg_ptr DeserializeIPCMessage(std::vector<ipc_message::byte_buffer>&& data, bool no_fail = false)
{
  uint8_t message_type = *(data.at(constants::index::TYPE).data());
  switch (message_type)
  {
    case (constants::IPC_OK_TYPE):          return std::make_unique<okay_message>    (data);
    case (constants::IPC_KEEPALIVE_TYPE):   return std::make_unique<keepalive>       ();
    case (constants::IPC_KIQ_MESSAGE):      return std::make_unique<kiq_message>     (data);
    case (constants::IPC_PLATFORM_TYPE):    return std::make_unique<platform_message>(data);
    case (constants::IPC_PLATFORM_INFO):    return std::make_unique<platform_info>   (data);
    case (constants::IPC_PLATFORM_ERROR):   return std::make_unique<platform_error>  (data);
    case (constants::IPC_PLATFORM_REQUEST): return std::make_unique<platform_request>(data);
    case (constants::IPC_FAIL_TYPE):        return std::make_unique<fail_message>    (data);
    case (constants::IPC_STATUS):           return std::make_unique<status_check>    (    );
    default:
      if  (no_fail)
      {
        auto&& msg = std::make_unique<ipc_message>();
        auto&  frames = msg->m_frames;
        frames.insert(frames.end(), data.begin(), data.end());
        return std::move(msg);
      }
      return nullptr;
  }
}
//---------------------------------------------------------------------
using timepoint = std::chrono::time_point<std::chrono::system_clock>;
using duration  = std::chrono::milliseconds;
static const duration time_limit = std::chrono::milliseconds(6000);
static const duration hb_rate    = std::chrono::milliseconds(300);
class session_daemon {
public:
  using hbtime_t = std::pair<timepoint, duration>;
  session_daemon()
  : m_active(false),
    m_valid(true)
  {
    m_future = std::async(std::launch::async, [this] { while (true) loop(); });
  }
//--------------------
  ~session_daemon()
  {
    if (m_future.valid())
      m_future.wait();
  }
//--------------------
  void add_observer(std::string_view peer, std::function<void()> callback)
  {
    log_fn("Added peer: "); log_fn(peer.data());
    observer_t observer{hbtime_t{}, callback};
    m_observers.try_emplace(peer, observer);
    m_observers.at(peer).first.first = std::chrono::system_clock::now();
  }
//--------------------
  void reset()
  {
    if (!m_active) m_active = true;
    m_tp = std::chrono::system_clock::now();
  }
//--------------------
  static void update_time(hbtime_t& hb_time)
  {
    timepoint& tpoint   = hb_time.first;
    duration & interval = hb_time.second;
    const auto now      = std::chrono::system_clock::now();
               interval = std::chrono::duration_cast<duration>(now - tpoint);
               tpoint   = now;
  }
//--------------------
  bool validate(std::string_view peer)
  {
    if (m_active)
    {
      if (auto it = m_observers.find(peer); it != m_observers.end())
      {
        hbtime_t& time = it->second.first;
        update_time(time);
        if (time.second < time_limit)
          return true;
        else
          it->second.second();
      }
      else
      log_fn("Peer does not exist");

    }
    else
      log_fn("Session daemon not active yet");
    return false;
  }
//--------------------
  void stop()
  {
    m_active = false;
    m_valid  = true;
  }
//--------------------
  bool active() const
  {
    return m_active;
  }
//--------------------
  bool has_observer(std::string_view peer) const
  {
    return m_observers.find(peer) != m_observers.end();
  }
//--------------------
  void loop()
  {
    using namespace std::chrono;
    for (auto it = m_observers.begin(); it != m_observers.end();)
    {
      auto& observer = it->second;
      if ((system_clock::now() - observer.first.first) > time_limit)
      {
        observer.second();
        it = m_observers.erase(it);
      }
      else
        it++;
      std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }
  }

private:
  using observer_t  = std::pair<hbtime_t, std::function<void()>>;
  using observers_t = std::map<std::string_view, observer_t>;

  timepoint         m_tp;
  duration          m_duration;
  bool              m_active;
  bool              m_valid;
  observers_t       m_observers;
  std::future<void> m_future;

};

class IPCTransmitterInterface
{
public:
  virtual ~IPCTransmitterInterface() = default;
//--------------------
  void send_ipc_message(ipc_message::u_ipc_msg_ptr message)
  {
    const auto     payload   = message->data();
    const size_t   frame_num = payload.size();

    for (int i = 0; i < frame_num; i++)
    {
      const auto     flag = i == (frame_num - 1) ? zmq::send_flags::none : zmq::send_flags::sndmore;
      const auto     data  = payload.at(i);
      zmq::message_t message{data.size()};
      std::memcpy(message.data(), data.data(), data.size());
      socket().send(message, flag);
    }
    on_done();
  }

protected:
  virtual zmq::socket_t& socket()  = 0;
  virtual void           on_done() = 0;
};
//---------------------------------------------------------------------
class IPCBrokerInterface
{
public:
  virtual ~IPCBrokerInterface() = default;
  virtual void on_heartbeat(std::string_view peer) = 0;
  virtual void process_message(ipc_message::u_ipc_msg_ptr) = 0;
};
//---------------------------------------------------------------------
class MessageHandlerInterface
{
public:
  virtual ~MessageHandlerInterface() = default;
  virtual void process_message(ipc_message::u_ipc_msg_ptr) = 0;
};

//---------------------------------------------------------------------
class IPCHandlerInterface : public MessageHandlerInterface,
                            public IPCTransmitterInterface
{
public:
  ~IPCHandlerInterface() override = default;
  std::string get_addr()
  {
    return socket().get(zmq::sockopt::last_endpoint);
  }
};
//---------------------------------------------------------------------
using client_handlers_t = std::map<std::string_view, IPCHandlerInterface*>;
} // ns kiq
