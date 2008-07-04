include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(0.007927, -15.196621, 3.059711), CAngleAxis(-0.102049, -0.007446, 0.072386, 0.992114))

	self:setClientBounceVel(1)

	self:setRamp(0, 0, 5)

	local m = self:addModuleNPSS("snow_box", CVector(0.003322,-15.228691,3.223083), 0, CVector(30, 1, 30)) m:setColor(CRGBA(254,254,254))
	local m = self:addModuleNPSS("snow_box", CVector(0.000899,-15.084286,2.989382), 0, CVector(30, 30, 1)) m:setRotation(CAngleAxis(1,0,0,-0.05)) m:setColor(CRGBA(254,254,254))

	self:addModuleNPSS("snow_box", CVector(0,-14.84,3), 50, CVector(3, 6, 0.5))
	self:addModuleNPSS("snow_box", CVector(0,-14.94,3), 100, CVector(1.5, 4, 0.5))
	self:addModuleNPSS("snow_box", CVector(0,-15,3), 300, CVector(1.5, 2, 0.5))
	self:addModuleNPSS("snow_box", CVector(0,-15.06,3), 100, CVector(1.5, 4, 0.5))
	self:addModuleNPSS("snow_box", CVector(0,-15.16,3), 50, CVector(3, 6, 0.5))

	self:addModuleNPSS("snow_box", CVector(-0.040000,-14.840000,3.00058), 0, CVector(1, 6, 1))
	self:addModuleNPSS("snow_box", CVector(-0.025000,-14.940000,3.000589), 0, CVector(1, 4, 6))
	self:addModuleNPSS("snow_box", CVector(-0.035003,-15,3.000589), 0, CVector(1, 2, 6))
	self:addModuleNPSS("snow_box", CVector(-0.025000,-15.060000,3.000589), 0, CVector(1, 4, 6))
	self:addModuleNPSS("snow_box", CVector(-0.040000,-15.160000,3.000589), 0, CVector(1, 6, 1))
	self:addModuleNPSS("snow_box", CVector(0.040000,-14.840000,3.000589), 0, CVector(1, 6, 1))
	self:addModuleNPSS("snow_box", CVector(0.025000,-14.940000,3.000589), 0, CVector(1, 4, 6))
	self:addModuleNPSS("snow_box", CVector(0.031365,-15,3.000589), 0, CVector(1, 2, 6))
	self:addModuleNPSS("snow_box", CVector(0.025000,-15.060000,3.000589), 0, CVector(1, 4, 6))
	self:addModuleNPSS("snow_box", CVector(0.040000,-15.160000,3.000589), 0, CVector(1, 6, 1))

end
