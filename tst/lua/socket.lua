local sctp = require "sctp"

local colors = {
  green = "\27[1;32m",
  red   = "\27[1;31m",
  reset = "\27[0m"
}

local function printResult(succeeded, error)
  if succeeded then
    io.write(colors.green, "OK", colors.reset,"\n")
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
local sock4, error = sctp.socket4()

if sock4 ~= nil and type(sock4) == "userdata" then
  printResult(true)
else
  printResult(false, error)
end

io.write("bind(ipv4SH): ")
local success, error = sock4:bind(12345, "127.0.0.1")
if success then
  printResult(true)
else
  printResult(false, error)
end
sock4:close()

local sock4MH = sctp.socket4()
io.write("bind(ipv4MH): ")
local success, error = sock4MH:bind(12345, "127.0.0.1", "127.0.0.2", "127.1.1.1")
if success then
  printResult(true)
else
  printResult(false, error)
end
sock4MH:close()

io.write("socket(ipv6): ")
local sock6, error = sctp.socket6()

if sock6 ~= nil and type(sock6) == "userdata" then
  printResult(true)
else
  printResult(false, error)
end

io.write("bind(ipv6SH): ")
local success, error = sock6:bind(12345, "::1")
if success then
  printResult(true)
else
  printResult(false, error)
end
sock6:close()

--[[io.write("bind(ipv6MH): ")
local success, error = sock6:bind(12345, "::1", "::2")
if success then
  printResult(true)
else
  printResult(false, error)
end]]
