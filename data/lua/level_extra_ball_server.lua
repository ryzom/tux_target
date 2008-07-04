
---------------------- Entity ----------------------

function CEntity:init()
	self:enableCrashInFly(0)
	self:setCurrentScore(0)
end

function CEntity:collideWithModule(module)
	if(self:isOpen()==0 and module:score()==0 and self:meanVelocity()<0.03) then
		self:collideWithWater()
		return
	end

	if(self:isOpen()==0 and module:score()>0 and self:meanVelocity()<0.03) then
		module:setEnabled(0)

		self:setCurrentScore(module:score()+self:currentScore())

		pos = self:startPointPos()
		self:setPosition(pos)
		self:setOpenCloseCount(0)

		local p = computeNewTargetPosition();
		module:setPosition(p)
		nlinfo("****new pos "..p:getX().." "..p:getY().." "..p:getZ())
		module:setEnabled(1)
		winnerStringMsg = "LevelExtraLanded|"..self:name()
		displayTextToAll(0,12,1,255,200,0,winnerStringMsg, 5)
		self:displayText(0,13,1,0,255,0,"LevelExtraBall", 2)
	end
end

function CEntity:collideWithWater()
	if(self:isOpen()==0) then
		pos = self:startPointPos()
		self:setPosition(pos)
		self:setOpenCloseCount(0)
		self:setFreezCommand(0)
		self:setIsOpen(0)
		self:displayText(0,13,1,0,255,0,"LevelExtraBall", 2)
	end
end
