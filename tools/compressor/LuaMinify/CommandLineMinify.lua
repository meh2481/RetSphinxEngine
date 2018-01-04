--
-- CommandlineMinify.lua
--
-- A command line utility for minifying lua source code using the minifier.
--

package.path = package.path .. ';' .. 'tools/compressor/LuaMinify/' .. '?.lua'

local util = require'Util'
local Parser = require'ParseLua'
local Format_Mini = require'FormatMini'
local ParseLua = Parser.ParseLua
local PrintTable = util.PrintTable

local function setglobal(name, v)
	rawset(_G, name, v)
end
setglobal("setglobal", setglobal)

local function main_minify(filename)
	local inf = io.open(filename, 'r')
	if not inf then
		print("Failed to open `"..filename.."` for reading")
		return nil
	end
	--
	local sourceText = inf:read('*all')
	inf:close()
	--
	local st, ast = ParseLua(sourceText)
	if not st then
		--we failed to parse the file, show why
		print(ast)
		return nil
	end
	return Format_Mini(ast)
end
setglobal("main_minify", main_minify)
