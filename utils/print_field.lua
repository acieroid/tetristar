colors = {'\27[40m', '\27[45m', '\27[47m', '\27[44m', 
          '\27[46m', '\27[42m', '\27[41m', '\27[43m'}
width = 10
height = 22

dofile("../clients/gtk/plugins/utils.lua")

field = utils.parse_fieldspec(arg[1])

-- Create the matrix
matrix = {}

for x = 0, width-1 do
   matrix[x] = {}
   for y = 0, height-1 do
      matrix[x][y] = 0
   end
end

-- Fill it
for i, cell in pairs(field) do
   matrix[cell[1]][cell[2]] = cell[3]
end

-- Print it
for y = 0, height-1 do
   for x = 0, width-1 do
      io.write(colors[matrix[x][y]+1] .. " " .. colors[1])
   end
   io.write("\n")
end
