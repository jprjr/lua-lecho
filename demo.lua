local lecho = require'lecho'

if lecho:status() then
  print("Terminal echo is on")
else
  print("Terminal echo is off - turning on")
  if not lecho:on() then
    print("Warning - unable to enable terminal echo")
  end
end

print("Type some data - this should be visible")
local clear_data = io.read()

print("Turning off terminal echo")

if not lecho:off() then
  print("Warning - unable to disable terminal echo")
end

print("Type some data - this should be invisible")
local invis_data = io.read()

if not lecho:on() then
  print("Warning - unable to enable terminal echo")
end

print("Data read: " .. invis_data)

