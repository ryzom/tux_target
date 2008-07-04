include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(-0.076742, -15.219810, 3.118468), CAngleAxis(-0.121932, 0.016329, -0.131726, 0.983623))

	self:setRamp(0, 0, 5)

	local m = self:addModuleNPSS("snow_box", CVector(0,-15,3), 50, CVector(14, 14, 0.5)) m:setFriction(10)
	local m = self:addModuleNPSS("snow_box", CVector(0,-15,3.01), 100, CVector(7, 7, 0.5)) m:setFriction(15)
	local m = self:addModuleNPSS("snow_box", CVector(0,-15,3.02), 300, CVector(3, 3, 0.5)) m:setFriction(40)
end
