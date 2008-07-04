include("utilities.lua")
include("level_extra_ball_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:setRamp(0, 0, 5)

	self:addModuleNP("snow_island", CVector(10, -25, 3.5))
	self:addModuleNP("snow_island2", CVector(7, -14, 1.8))
	self:addModuleNP("snow_island3", CVector(-3.8, -17, 2))

	local m = self:addModuleNPSS("snow_box", CVector(0,-8,4), 300, CVector(6,6,1))
	m:setFriction(15)
	m:setColor(CRGBA(255,200,0))

	self:setHasBonusTime(false)
	math.randomseed(os.time())

end

function computeNewTargetPosition()
	return CVector(math.random(100)/100*8-4, -7-math.random(100)/100*7, math.random(100)/100*4.5+0.5)
end
