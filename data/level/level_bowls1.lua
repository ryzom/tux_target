include("utilities.lua")
include("level_bowls1_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:setInfo("LevelInfoBowls1")

	self:addExternalCamera(CVector(0.001387, -15.348904, 0.919935), CAngleAxis(-0.697909, -0.064740, 0.065881, 0.710205))

	self:setRamp(0, 0, 3)

	self:addModuleNP("snow_island", CVector(6, -20, 2.8))
	self:addModuleNP("snow_island2", CVector(-3.8, -17, 2))
	local m = self:addModuleNP("snow_island3", CVector(-0.3, -12, 2.4))
	m:setRotation(CAngleAxis(0,0,1,0.7))

	local m = self:addModuleNPSS("snow_box10", CVector(0, -15.35, 0.55), 1, CVector(1.5, 1.5, 0.1)) m:setRotation(CAngleAxis(0,0,1,0.785))
	local m = self:addModuleNPSS("snow_box", CVector(0, -15.35, 0.65), 1, CVector(1, 1, 10)) m:setRotation(CAngleAxis(0,0,1,0.785)) m:setColor(CRGBA(0,255,0))
end
