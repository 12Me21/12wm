print "START"
function key_press(window, name)
	if name=="q" then window:close() end
end

function left_click(win, pos)
	win:focus()
end

function move_start(win, start)
	win:focus()
end

function move_during(win, winstart, start, curr)
	win:move(winstart + (curr - start))
end

function move_done(w, s, e)
	--print("move done", w, e-s)
end
