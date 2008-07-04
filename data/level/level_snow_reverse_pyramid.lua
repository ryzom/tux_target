include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 6

function CLevel:init()

	--self:setAdvancedLevel(true)

	self:addExternalCamera(CVector(-0.074286, -15.216643, 3.229653), CAngleAxis(-0.112392, 0.012555, -0.110309, 0.987442))

	self:setRamp(0, 0, 5)

	local m = self:addModuleNPSS("snow_box", CVector(0,-15,3.2), 50, CVector(14, 14, 0.5)) m:setFriction(10)
	local m = self:addModuleNPSS("snow_box", CVector(0,-15,3.1), 100, CVector(7, 7, 0.5)) m:setFriction(15)
	local m = self:addModuleNPSS("snow_box", CVector(0,-15,3.0), 300, CVector(3, 3, 0.5)) m:setFriction(40)
end
