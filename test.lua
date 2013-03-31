require("html")

--print(HTMLParser)
--print(HTMLParser.newparser)

if #arg == 0 then
   print("uasge:" .. arg[-1] .. " " .. arg[0] .. " sample.html")
   return 
end

file = io.open(arg[1])
str = file:read("*all")

local doc = html.newdocument(str)
print(doc)
print(getmetatable(doc))

elem = doc:getElementById("userinfo_form");

print(elem)
print(elem:getAttribute("id"))

local elem = elem:copy()
local child = elem:firstChild()
repeat
   if 1 == child:type() then
      print("node name:", child:name())
   end
   local n = child:firstChild()
   if nil == n then
      n = child:nextSibling()
      if nil == n then
         repeat 
            child = child:parent()
            if child then
               n = child:nextSibling()
            end
         until nil == child or nil ~= n
      end
   end
   if child then
      child = n
   end
until nil == child 

--[[
print(parser)

t = getmetatable(parser)
print(t)

print(t.__gc)
]]

--[[
for k,v in pairs(parser) do
   print(k)
   end
]]
