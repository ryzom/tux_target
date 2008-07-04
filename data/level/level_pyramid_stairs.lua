include("utilities.lua")
include("level_stairs_server.lua")

ReleaseLevel = 6

function CLevel:init()

	self:addExternalCamera(CVector(-0.033472, -15.974096, 3.567634), CAngleAxis(-0.281957, 0.000176, -0.000600, 0.959427))

	--self:setAdvancedLevel(true)

	self:setInfo("LevelInfoStairs")

	self:setClientBounceCoef(0)
	self:setClientBounceVel(1)

	self:setRamp(0, 0, 5)

	for i = 0, 10 do
		local m = self:addModuleNPSS("snow_box", CVector(0,-15, 3-i/100*2), 50, CVector(3+6*i, 3+6*i, 1))
		m:setAccel(0)
		m:setColor(CRGBA(250-i*25,255,250-i*25))
	end

end
