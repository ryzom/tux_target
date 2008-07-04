
---------------------- Entity ----------------------

function CEntity:preUpdate()
	self:setCurrentScore(0, false)
end

function CEntity:collideWithModule(module)
	if(self:isOpen() == 0 and module:score() ~= 0) then
		self:setCurrentScore(module:score(), false)
	end
end
