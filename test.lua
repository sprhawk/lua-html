require("html")

--print(HTMLParser)
--print(HTMLParser.newparser)

file = io.open("sample.html")
str = file:read("*all")

local doc = html.newdocument(str)
print(doc)
print(getmetatable(doc))

elem = doc:getElementById("userinfo_form");

print(elem)
print(elem:getAttribute("id"))

local child = elem
repeat
   if 1 == child:type() then
      print("node name:", child:name())
   end
   local n = child:firstChild()
   if nil == n then
      if child == elem then
         break
      end
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

