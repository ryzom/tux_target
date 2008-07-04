
---------------------- Entity ----------------------

function CEntity:init()
	self:enableCrashInFly(0)
	self:setCurrentScore(0)
end

function CEntity:collideWithGate(gate)
	if(gate:score()>0) then
		gate:setEnabled(0)
		self:setCurrentScore(gate:score()+self:currentScore())
		pos = self:startPointPos()
		local p = computeNewTargetPosition();
		gate:setPosition(p)
		nlinfo("****new pos "..p:getX().." "..p:getY().." "..p:getZ())
		gate:setEnabled(1)
		winnerStringMsg = "LevelExtraLanded|"..self:name()
		displayTextToAll(0,12,1,255,200,0,winnerStringMsg, 5)
		self:displayText(0,13,1,0,255,0,"LevelExtraBall", 2)
	end
	return false
end

function CEntity:collideWithWater()
	pos = self:startPointPos()
	self:setPosition(pos)
	self:setOpenCloseCount(0)
	self:setFreezCommand(0)
	self:setIsOpen(0)
	self:displayText(0,13,1,0,255,0,"LevelExtraBall", 2)
end
