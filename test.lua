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
--print(doc)
--print(getmetatable(doc))

local node = doc:getDocTree()
--local elem = doc:getElementById("userinfo_form");

--print("elem:", elem)
--print(elem:getAttribute("id"))

--elem = elem:copy()
--local child = elem:firstChild()


function traverse(node, func)
   local child = node
   repeat
      local cont = 1
      if 1 == child:type() then
         if func and type(func) == "function" then
            cont = func(child)
         end
      end
      local n = nil
      if 1==cont then
         n = child:firstChild()
      end
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
end   

local forms = {}

traverse(node, 
         function(node)
            local name = node:name()
            if "form" == name then
               n = node:copy()
               forms[#forms+1] = n
               return 0
            end
            return 1
         end)

for k,v in pairs(forms) do
   print(k, ":", v)
   local id = v:getAttribute("id")
   local name = v:getAttribute("name")
   local method = v:getAttribute("method")
   local action = v:getAttribute("action")
   print("id:",id, "name:",name,"method:", method,"action:", action)

   traverse(v,
            function(node)
               local name = node:name()
               if "input" == name then
                  local vname = node:getAttribute("name")
                  local type = node:getAttribute("type")
                  local value = node:getAttribute("value")
                  print(name .. "(" .. (type or "") .. ")" .. "->" .. (vname or "") .. ":" .. (value or ""))
               end
            end
           )

end
