include("utilities.lua")
include("level_paint_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(0.054396, -15.107917, 2.120903), CAngleAxis(-0.600528, 0.002221, -0.002957, 0.799595))

	self:setRamp(0, 0, 5)

	self:addDefaultIsland()

	for x = 0, 0.13, 0.04 do
		for y = -15.12, -15, 0.04 do
			local m = self:addModuleNPSS("snow_box", CVector(x, y, 2), 0, CVector(2,2,1))
			m:setBounce(0)
			m:setAccel(0.00001)
			m:setFriction(10)
			m:setUserData(CModulePaintBloc:new(m))
		end
	end
end
