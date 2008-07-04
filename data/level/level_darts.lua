include("utilities.lua")
include("level_darts_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:setInfo("LevelInfoDarts")

	self:addExternalCamera(CVector(-0.028859, -40.226116, 1.064289), CAngleAxis(-0.094677, -0.228571, 0.895158, 0.370787))

	self:setCameras()

	self:setTimeout(20)

	self:setCameraMinDistFromStartPointToMove(100)
	self:setCameraMinDistFromStartPointToMoveVerticaly(100)

	local m = self:addModuleNPSS("snow_box", CVector(0.24,-3.574639,7), 0, CVector(50, 510, 2))
	m:setColor(CRGBA(28,28,28,255))
	m:setBounce(0)
	m:setAccel(0.004)
	m:setFriction(0)
	for i = 0.0, 0.45, 0.03 do
		self:addStartPoint(CVector(i, 1.5, 7.1))
	end

	self:addModuleNPSS("snow_box", CVector(-0.1,-40.3,1), 50, CVector(50, 1, 50))
	self:addModuleNPSS("snow_box", CVector(-0.1,-40.284740,1), 100, CVector(10, 1, 10))
	self:addModuleNPSS("snow_box", CVector(-0.1,-40.275701,1), 300, CVector(2, 1, 2))
end
