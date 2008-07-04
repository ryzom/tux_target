include("utilities.lua")
include("level_stairs_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:setInfo("LevelInfoStairs")

	self:setClientBounceCoef(0)
	self:setClientBounceVel(1)

	self:addExternalCamera(CVector(0.235616, -15.587805, 3.067564), CAngleAxis(-0.208756, -0.093624, 0.398360, 0.888237))

	self:setRamp(0, 0, 5)

	self:addModuleNPSS("snow_box", CVector(-0,-14.96,2.9), 0, CVector(2, 1, 11))

	for i = 0, 10 do
		local m = self:addModuleNPSS("snow_box", CVector(0,-15-i/100*3,3-i/100*2), 50, CVector(2, 3+3*i, 1))
		m:setAccel(0)
		m:setColor(CRGBA(250-i*25,255,250-i*25))
	end
end
