function sign(x)
	if x<0 then return -1 end
	if x>0 then return 1 end
	return 0
end
function sign2D(pt)
	return P(sign(pt.x),sign(pt.y))
end

function floor(pt)
	return P(math.floor(pt.x),math.floor(pt.y))
end

print "START"
local resize
function key_press(window, name)
	if name=="q" then window:close() end
	if name=="x" then resize=true end
end

function left_click(win, pos)
	win:focus()
end

local wsz
local wss
local corner
function move_start(win, start)
	wsz=win.size
	resize=false
	wss=win.pos
	win:focus()
	corner = sign2D(floor((start-win.pos)/win.size*P(3,3))-P(1,1))
	print(corner)
end

function move_during(w, winstart, s, e)
	--if not resize then win:move(winstart + (curr - start)) end
	if corner == P(0,0) then
		w:move(wss+(e-s))
	else
		--w:move(wss-(e-s)*P(math.min(corner.x,0),math.min(corner.y,0)), wsz+(e-s)*corner)
	end
end

function move_done(w, ws, s, e)
	--if resize then
	if corner == P(0,0) then
		w:move(wss+(e-s))
	else
		w:move(wss-(e-s)*P(math.min(corner.x,0),math.min(corner.y,0)), wsz+(e-s)*corner)
	end
	--end
	--print("move done", w, e-s)
end
