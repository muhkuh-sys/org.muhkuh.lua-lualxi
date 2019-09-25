-- Set the search path for LUA plugins and modules.
package.cpath = package.cpath .. ";lua_plugins/?.so"
package.path = package.path .. ";lua/?.lua;lua/?/init.lua"


-- A simple log callback function.
local function fnMessageCallback(a,b)
  print('LOG: ' .. a, b)
end

-- Create the LXI context and register the logging callback.
local Lxi = require 'lualxi'
local tLxi = Lxi.Lxi()
tLxi:set_callback(fnMessageCallback, 1234)

-- Discover all devices with the VXI11 protocol and a timeout of 1000ms.
-- Please note that the current version of the plugin does not support avahi.
local tResult, strError = tLxi:discover(1000, Lxi.DISCOVER_VXI11)
if tResult==nil then
  error('Failed to discover devices: ' .. strError)
end
-- Show all detected devices.
local atDetectedDevices = tResult
print('Discovered devices:')
for strAddress, strId in pairs(tResult) do
  print(string.format('  %s: %s', strAddress, strId))
end

-- Look for a Keysight 34465A device.
local strKeysightAddress = nil
for strAddress, strId in pairs(tResult) do
  if string.sub(strId, 1, 28)=='Keysight Technologies,34465A' then
    strKeysightAddress = strAddress
    break
  end
end
if strKeysightAddress==nil then
  error('No keysight 34465A device found.')
end

-- Connect to the device with the VXI11 protocol.
local uiTimeoutInMs = 1000
assert(tLxi:connect(strKeysightAddress, 0, 'inst0', uiTimeoutInMs, Lxi.VXI11))

-- Send a demo command and receive the response.
local strCommand = '*IDN?'
print(string.format('Sending command "%s".', strCommand))
local tResult, strError = tLxi:send(strCommand, uiTimeoutInMs)
if tResult==nil then
  error('Send failed: ' .. tostring(strError))
end
local tResult, strError = tLxi:receive(65536, uiTimeoutInMs)
if tResult==nil then
  error('Receive failed: ' .. tostring(strError))
end
print('Received: ' .. tostring(tResult))

-- Close the connection.
assert(tLxi:disconnect())
