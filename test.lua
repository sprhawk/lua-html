require("html")

--print(HTMLParser)
--print(HTMLParser.newparser)

file = io.open("sample.html")
str = file:read("*all")

local doc = html.newdocument(str)
print(doc)
print(getmetatable(doc))

elem = doc:getElementById("password");

print(elem)
print(elem:getAttribute("id"))

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

