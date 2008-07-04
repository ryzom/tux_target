include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(-0.020185, -30.473835, 1.236753), CAngleAxis(-0.099024, -0.022244, 0.218036, 0.970649))

	self:setRamp(0, 0, 5)

--	self:addModuleNPSS("snow_box", CVector(0,-20,3.5), 0, CVector(10, 500, 1))
	local m = self:addModuleNPSS("snow_box10", CVector(0,-20,3), 0, CVector(0.20, 10, 0.1))
	m:setBounce(0)
	m:setFriction(0)

	local m = self:addModuleNPS("snow_target_50", CVector(-0.1, -30.3, -0.573801), 50) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_100", CVector(-0.1, -30.35, 1), 100) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_300", CVector(-0.1, -30.33, 1.172893), 300) m:setColor(CRGBA(255,255,255))
end

function CEntity:init()
	self:setMaxOpenClose(4)
end
