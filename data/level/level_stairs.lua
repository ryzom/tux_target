include("utilities.lua")
include("level_stairs_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:setInfo("LevelInfoStairs")

	self:setClientBounceCoef(0)
	self:setClientBounceVel(1)

	self:addExternalCamera(CVector(-0.171097, -14.318943, 3.089481), CAngleAxis(0.023125, -0.132589, 0.976166, -0.170251))

	self:setRamp(0, 0, 5)

	self:addModuleNPSS("snow_box", CVector(0,-15.04,3.09), 0, CVector(9, 1, 30))

	for i = 0, 10 do
		local m = self:addModuleNPSS("snow_box", CVector(0,-15+i/100*3,3-i/100*2), 50, CVector(9, 3+3*i, 1))
		m:setAccel(0)
		m:setColor(CRGBA(255-i*25,255,255-i*25))
	end
end
