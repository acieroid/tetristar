-- Copyright (c) 2013, Quentin Stievenart
-- All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions
-- are met:
--
-- Redistributions of source code must retain the above copyright
-- notice, this list of conditions and the following disclaimer.
--
-- Redistributions in binary form must reproduce the above copyright
-- notice, this list of conditions and the following disclaimer in the
-- documentation and/or other materials provided with the
-- distribution.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
-- "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
-- LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
-- FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
-- COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
-- INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
-- (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
-- SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
-- HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
-- OF THE POSSIBILITY OF SUCH DAMAGE.

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