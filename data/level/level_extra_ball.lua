include("utilities.lua")
include("level_extra_ball_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:setRamp(0, 0, 5)

	self:addDefaultIsland()

	local m = self:addModuleNPS("snow_target_300_flat", CVector(0.12,-14.33,2.8), 300)
	m:setFriction(8)
	m:setColor(CRGBA(255,255,255))

	self:setHasBonusTime(false)
	math.randomseed(os.time())

end

function computeNewTargetPosition()
	return CVector(2*(math.random(100)/50-0.5), -14.33, 2.8)
end
