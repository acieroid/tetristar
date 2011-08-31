utils = {}

function utils.identity(x)
   return x
end

function utils.split(str, sep, head_fun, tail_fun)
   local first_sep = string.find(str, sep)
   local head = string.sub(str, 0, first_sep - 1)
   local tail = string.sub(str, first_sep + 1)
   return head_fun(head), tail_fun(tail)
end

function utils.parse_fieldspec(str)
   local fieldspec = {}
   local i = 0
   local cell = ""
   while not (str == "") do
      if string.find(str, ":") == nil then
         cell = str
         str = ""
      else
         cell, str = utils.split(str, ":", utils.identity, utils.identity)
      end
      local x, rest = utils.split(cell, ",", tonumber, utils.identity)
      local y, type = utils.split(rest, ",", tonumber, tonumber)
      fieldspec[i] = { x, y, type }
      i = i+1
   end
   return fieldspec
end