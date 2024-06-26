const fs = require('fs')

const IPC_OK_TYPE          = 0x00
const IPC_KEEPALIVE_TYPE   = 0x01
const IPC_KIQ_MESSAGE      = 0x02
const IPC_PLATFORM_TYPE    = 0x03
const IPC_PLATFORM_ERROR   = 0x04
const IPC_PLATFORM_REQUEST = 0x05
const IPC_PLATFORM_INFO    = 0x06
const IPC_FAIL_TYPE        = 0x07
const IPC_STATUS           = 0x08
const encoder              = new TextEncoder()
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
function create_ipc_message(type, payload, platform, id = '')
{
  const frames = []
  let   data   = []

  const processor = { "loadurl"   : function() { data = ["", IPC_PLATFORM_INFO, platform, id, payload, type  ] },
                      "analysis"  : function() { data = ["", IPC_PLATFORM_INFO, platform, id, payload, type  ] },
                      "generate"  : function() { data = ["", IPC_PLATFORM_INFO, platform, id, payload, type  ] },
                      "ok"        : function() { data = ["", IPC_OK_TYPE,                                  ""] },
                      "keepalive" : function() { data = ["", IPC_KEEPALIVE_TYPE,                           ""] },
                      "kiq"       : function() { data = ["", IPC_KIQ_MESSAGE,                              ""] },
                      "platform"  : function() { data = ["", IPC_PLATFORM_TYPE,                            ""] },
                      "error"     : function() { data = ["", IPC_PLATFORM_ERROR,                           ""] },
                      "request"   : function() { data = ["", IPC_PLATFORM_REQUEST,                         ""] },
                      "info"      : function() { data = ["", IPC_PLATFORM_INFO, platform, id, payload, type  ] },
                      "fail"      : function() { data = ["", IPC_FAIL_TYPE,                                ""] },
                      "status"    : function() { data = ["", IPC_STATUS,                                   ""] }}

  processor[type](payload)
  for (const part of data)
    frames.push(encoder.encode((typeof part === 'number') ?
                                String.fromCharCode(part) : part))
  return frames
}
//---------------------------------------------------------------------------------------------------------------
function deserialize_ipc(data)
{
  const type = data[1].charCodeAt(0)
  if (type === IPC_PLATFORM_INFO)
    return data[4].replaceAll('%2C', ',')

  return data[3]
}
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
module.exports.kproto      = create_ipc_message
module.exports.default     = create_ipc_message
module.exports.deserialize = deserialize_ipc