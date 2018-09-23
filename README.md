# lsctp
A simple Lua module that binds some features of the SCTP api.

Limitations:
- One-to-one sockets only
- Mixing IPv4 and IPv6 addresses are not supported
- Usage of deprecated sctp_sendmsg() and sctp_recvmsg() functions (because of lksctp-tools)

Example usage:
```lua
sctp = require "sctp"

local server = sctp.server.socket4()
server:bind(12345, "127.0.0.1")
server:listen()

local client = sctp.client.socket4()
client:connect(12345, "127.0.0.1")

local peer = server:accept()

client:send(string.pack("iii", 1, 2, 3))
local size, msg = peer:recv()

print(string.unpack("iii", msg))

peer:close()
client:close()
server:close()

```
