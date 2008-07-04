---------------------- Entity ----------------------

function CEntity:init()
	--self:displayText(0,12,1,255,200,0,"warning : In this level you must hit the target in open mode",60)
	self:setCurrentScore(0)
end

function CEntity:collideWithModule(module)
	self:setCurrentScore(module:score(), false)
	if(self:currentScore() > 0 and self:arrivalTime() == 0) then
	    self:setArrivalTime()
    end
end
