include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()

	self:setClientBounceCoef(0)
	self:setClientBounceVel(1)

	self:addExternalCamera(CVector(-0.102781, -15.413825, 2.875841), CAngleAxis(-0.305058, 0.000763, -0.002381, 0.952331))

	self:setRamp(0, 0, 5)

	self:addDefaultIsland()

	local m = self:addModuleNPSS("snow_box", CVector(-0.1, -15.33, 2.8), 200, CVector(6, 6, 1)) m:setColor(CRGBA(100,255,100))
	m:setFriction(10)
end
