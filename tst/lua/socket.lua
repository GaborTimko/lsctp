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
local sock4, error = sctp.socket(4)

if sock4 ~= nil and type(sock4) == "userdata" then
  printResult(true)
else
  printResult(false, error)
end

io.write("socket(ipv6): ")
local sock6, error = sctp.socket(6)

if sock6 ~= nil and type(sock6) == "userdata" then
  printResult(true)
else
  printResult(false, error)
end

io.write("socket(invalid): ")
local invalidSock, error = sctp.socket(1)

--[[
  TODO: also check the string "Address family not supported by this library: Invalid argument"
]]
if invalidSock == nil then
  printResult(true)
else
  printResult(false, error)
end
