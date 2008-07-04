include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(-0.173854, -15.405266, 2.845124), CAngleAxis(-0.007905, 0.002636, -0.316335, 0.948611))

	self:setRamp(0, 0, 5)

	self:addDefaultIsland()

	local m = self:addModuleNPS("snow_target_50_flat", CVector(-0.1, -15.3, 1), 50)
	m:setFriction(15) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_100_flat", CVector(-0.1, -15.35, 2.55), 100)
	m:setFriction(10) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_300_flat", CVector(-0.1, -15.33, 2.8), 300)
	m:setFriction(10) m:setColor(CRGBA(255,255,255))
end
