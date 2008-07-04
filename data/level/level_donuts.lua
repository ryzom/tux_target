include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(-0.872610, -15.804185, 1.853076), CAngleAxis(-0.181733, 0.184104, -0.687447, 0.678598))

	self:setRamp(-0.1, 7, 8)

	self:addModuleNP("snow_island", CVector(10, -25, 3.5))
	local m = self:addModuleNP("snow_island2", CVector(15,-12,2)) m:setRotation(CAngleAxis(0,0,1,1.58))
	local m = self:addModuleNP("snow_island3", CVector(8,-6,2)) m:setRotation(CAngleAxis(0,0,1,2.58))
	local m = self:addModuleNP("snow_island2", CVector(-10,-20,2)) m:setRotation(CAngleAxis(0,0,1,-1.58))
	local m = self:addModuleNP("snow_island3", CVector(10,-20,2)) m:setRotation(CAngleAxis(0,0,1,-2.58))

	local m = self:addModuleNPSS("snow_torus", CVector(1.5,-6,6), 0, CVector(200, 100, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(1.5,-6,6), 0, CVector(400, 200, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(1.5,-6,6), 0, CVector(800, 400, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(-0.1,-8,5), 0, CVector(200, 100, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(-0.1,-8,5), 0, CVector(400, 100, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(-0.5,-10,4.5), 0, CVector(30, 15, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(-0.5,-10,4.5), 0, CVector(125, 70, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(-0.5,-10,4.5), 0, CVector(250, 150, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(-0.5,-12,3.9), 0, CVector(250, 100, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(-0.5,-12,3.9), 0, CVector(500, 200, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(-0.5,-12,3.9), 0, CVector(1000, 400, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(0.5,-11,4.5), 0, CVector(100, 800, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(0.5,-11,4.5), 0, CVector(200, 1600, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_funnel", CVector(-0.8,-15.8,2), 0, CVector(4, 4, 2.1))

	local m = self:addModuleNPSS("snow_bowl", CVector(-0.8,-15.8,1.9), 300, CVector(0.4, 0.4, 0.3))

end
