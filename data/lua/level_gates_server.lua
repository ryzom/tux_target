
---------------------- Entity ----------------------

-- we don't reset the score every loop

function CEntity:init()
	self:setCurrentScore(0)
end

function CEntity:collideWithGate(gate)
	nlinfo("collide with a gate "..gate:score())
	self:setCurrentScore(self:currentScore()+gate:score())
	gate:setScore(gate:score()-10)
	if(gate:score() < 0) then
		gate:setScore(0)
	end
	return true
end

function CEntity:collideWithWater()
	self:setCurrentScore(0)
end
