include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 0

-- test the fun and if ok, code the ending of the game (one left, or one team left)

function CLevel:init()
	self:addExternalCamera(CVector(-0.004323, 0.241951, 5.040887), CAngleAxis(-0.000963, -0.199484, 0.979889, 0.004732))
	self:setSunParams()
	for x = 0, 3 do
		for y = 0, 3 do
			self:addStartPoint(CVector(x/30-1/30, y/30-1/30, 5.12))
		end
	end
	self:setCameras()

	for i = 0, 12 do
		self:addSunModule("cylinder", "sand", 0, CVector(0, 0, 5-i/80), CVector(24-i*2,24-i*2,1))
	end

end

function CModule:update()
	local id = self:getId()
	local p = 11-timeRemaining()/5
	if(p > id) then
		self:setEnabled(false)
	end
end
