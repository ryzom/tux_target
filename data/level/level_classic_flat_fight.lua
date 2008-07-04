include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CEntity:init()
	--self:displayText(0,12,1,255,200,0,"In this level you can control pingoo on target", 60)
	self:setMaxOpenClose(3)
end

function CLevel:init()
	self:setInfo("LevelInfoFight")

	self:addExternalCamera(CVector(-0.156463, -15.406598, 2.840894), CAngleAxis(-0.015813, 0.005262, -0.315709, 0.948710))

	self:setRamp(0, 0, 5)

	self:addDefaultIsland()

	local m = self:addModuleNPS("snow_target_50_flat", CVector(-0.1, -15.3, 1), 50)
	m:setFriction(25) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_100_flat", CVector(-0.1, -15.35, 2.55), 100)
	m:setFriction(10) m:setColor(CRGBA(255,255,255))
	local m = self:addModuleNPS("snow_target_300_flat", CVector(-0.1, -15.33, 2.8), 300)
	m:setFriction(10) m:setColor(CRGBA(255,255,255))
end
