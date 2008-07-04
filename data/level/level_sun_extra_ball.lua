include("utilities.lua")
include("level_sun_extra_ball_server.lua")

ReleaseLevel = 5

function CLevel:init()
	self:setSunParams()
	self:setSunRamp(0, 0, 0)

	self:addGatePS(CVector(0, -9, 3), 300)

	self:setHasBonusTime(false)
	math.randomseed(os.time())
end

function computeNewTargetPosition()
	return CVector(math.random(100)/100*8-4, -7-math.random(100)/100*7, math.random(100)/100*2.5+0.05)
end
