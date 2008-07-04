include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 1

function CLevel:init()
	self:addExternalCamera(CVector(0.032237, -15.300839, 3.112012), CAngleAxis(-0.001991, -0.500488, 0.865734, 0.003443))

	self:setRamp(0, 0, 5)

	self:addDefaultIsland()

	local m = self:addModuleNPS("snow_target_50_flat", CVector(0, -16, 1), 50)
	m:setFriction(30) m:setColor(CRGBA(255,255,255))

	local m = self:addModuleNPS("snow_target_100_flat", CVector(0, -15.5, 2.4), 100) m:setFriction(40) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_300_flat", CVector(0, -15.3, 2.45), 300) m:setFriction(50) m:setColor(CRGBA(255,255,255))

	local m = self:addModuleNPS("snow_target_100_flat", CVector(0, -16.5, 2.4), 100) m:setFriction(40) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_300_flat", CVector(0, -16.7, 2.45), 300) m:setFriction(50) m:setColor(CRGBA(255,255,255))

	local m = self:addModuleNPS("snow_target_100_flat", CVector(-0.5, -16, 2.4), 100) m:setFriction(40) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_300_flat", CVector(-0.7, -16, 2.45), 300) m:setFriction(50) m:setColor(CRGBA(255,255,255))

	local m = self:addModuleNPS("snow_target_100_flat", CVector(0.5, -16, 2.4), 100) m:setFriction(40) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_300_flat", CVector(0.7, -16, 2.45), 300) m:setFriction(50) m:setColor(CRGBA(255,255,255))

end
