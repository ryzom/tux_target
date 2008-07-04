include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(-0.144745, -15.145563, 3.033414), CAngleAxis(0.167747, -0.027609, -0.160039, 0.972361))

	self:setRamp(0, 0, 5)

	self:addDefaultIsland()

	local m = self:addModuleNPSS("snow_box", CVector(-0.1,-14.912012,3.037934), 0, CVector(2, 10, 1)) m:setFriction(3)
	self:addModuleNPSS("snow_box", CVector(-0.174782,-15.078223,3.061672), 0, CVector(1, 6, 6))
	self:addModuleNPSS("snow_box", CVector(-0.098757,-15.011895,3.024662), 0, CVector(6, 0.5, 2))
	self:addModuleNPSS("snow_box", CVector(-0.1,-15.148162,3.060950), 0, CVector(6, 1, 6))
	self:addModuleNPSS("snow_box", CVector(-0.026166,-15.073698,3.064236), 0, CVector(1, 6, 6))
	self:addModuleNPSS("snow_box", CVector(-0.1,-15.081491,3.102471), 0, CVector(6, 6, 2))
	local m = self:addModuleNPSS("snow_box", CVector(-0.097495,-15.04,3.02), 300, CVector(2, 2, 0.5)) m:setFriction(20)
	local m = self:addModuleNPSS("snow_box", CVector(-0.098685,-15.06,3.01), 100, CVector(4, 4, 0.5)) m:setFriction(15)
	local m = self:addModuleNPSS("snow_box", CVector(-0.1,-15.08,3), 50, CVector(6, 6, 0.5)) m:setFriction(10)

end
