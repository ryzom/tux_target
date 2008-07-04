include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(-0.238540, -14.858599, 3.179905), CAngleAxis(-0.107183, 0.165332, -0.822650, 0.533314))

	self:setClientBounceCoef(0)
	self:setClientBounceVel(1)

	self:setRamp(0, 0, 5)

	local m = self:addModuleNPSS("snow_box", CVector(0,-15.07,3), 0, CVector(30, 1, 50)) m:setColor(CRGBA(254,254,254))
	local m = self:addModuleNPSS("snow_box", CVector(0,-15.08,2.99), 0, CVector(30, 30, 1)) m:setColor(CRGBA(254,254,254)) m:setRotation(CAngleAxis(1,0,0,-0.05))

	self:addModuleNPSS("snow_box", CVector(0,-15,3), 50, CVector(14, 14, 0.5))
	self:addModuleNPSS("snow_box", CVector(0,-15.0,3.01), 100, CVector(7, 7, 0.5))
	self:addModuleNPSS("snow_box", CVector(0,-15.04,3.02), 300, CVector(3, 3, 0.5))
end
