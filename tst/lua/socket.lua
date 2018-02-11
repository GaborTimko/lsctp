local sctp = require "sctp"

local colors = {
  green = "\27[1;32m",
  red   = "\27[1;31m",
  reset = "\27[0m"
}

local function printResult(succeeded, error)
  if succeeded then
    io.write(colors.green, "OK", colors.reset, '\n')
  else
    io.write(colors.red, "FAILED: ", colors.reset, error, '\n')
  end
end

-- local successCount, failureCount = 0, 0

--[[
  I could use assert here to shorten the code,
  but that would prevent the other tests to run
]]

io.write("socket(ipv4): ")
local sock4, error = sctp.server.socket4()
printResult(sock4 ~= nil and type(sock4) == "userdata", error)

io.write("bind(ipv4SH): ")
local success, error = sock4:bind(12345, "127.0.0.1")
printResult(success, error)
sock4:close()

local sock4MH = sctp.server.socket4()
io.write("bind(ipv4MH): ")
local success, error = sock4MH:bind(12345, "127.0.0.1", "127.0.0.2", "127.1.1.1")
printResult(success, error)
sock4MH:close()

io.write("socket(ipv6): ")
local sock6, error = sctp.server.socket6()
printResult(sock6 ~= nil and type(sock6) == "userdata", error)

io.write("bind(ipv6SH): ")
local success, error = sock6:bind(12345, "::1")
printResult(success, error)
sock6:close()

--[[io.write("bind(ipv6MH): ")
local success, error = sock6:bind(12345, "::1", "::2")
printResult(success, error)]]

--I really should create/use a testing framework
io.write("send/receive: ")
local server = sctp.server.socket4()
server:bind(12345, "127.1.1.1", "127.3.3.3")
server:listen()

local client = sctp.client.socket4()
client:bind(12345, "127.2.2.2", "127.4.4.4")
client:connect(12345, "127.1.1.1", "127.3.3.3")

local client2 = server:accept()

client:send(string.pack("ii", 10, 100))
local count, msg = client2:recv()
local x, y = string.unpack("ii", msg)
printResult(x == 10 and y == 100, error)
server:close()
client:close()
client2:close()
