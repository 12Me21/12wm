print "START"
return function(name, window)
	print("Lua: got key: ["..name.."] from window: "..window)
	if name=="q" then
		kill(window)
	end
end
