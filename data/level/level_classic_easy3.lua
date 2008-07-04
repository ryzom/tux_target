include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(-0.097794, -15.170495, 2.524882), CAngleAxis(-0.104804, 0.000819, -0.007771, 0.994462))

	self:setRamp(0, 0, 5)

	self:addDefaultIsland()

	local m = self:addModuleNPS("snow_target_50_flat", CVector(-0.1, -15.5, 1), 50)
	m:setFriction(15) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_100_flat", CVector(-0.1, -15, 2.4), 100)
	m:setFriction(10) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_300_flat", CVector(-0.1, -14.80, 2.45), 300)
	m:setFriction(10) m:setColor(CRGBA(255,255,255))
end
