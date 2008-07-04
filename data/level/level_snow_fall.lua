include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(-0.009918, -0.232358, 0.377324), CAngleAxis(0.044152, 0.000166, 0.003746, 0.999018))

	self:setCrashedClientCollide(false)

	for i = 0, 16 do
		self:addCamera(CVector(0, 1, 10))
	end

	for x = 0.01, 0.08, 0.021 do
		for y = 0.01, 0.08, 0.021 do
			self:addStartPoint(CVector(x-0.04,y-0.04,30.2))
		end
	end

	self:addModuleNPSS("snow_funnel", CVector(0,0,30), 0, CVector(1,1,4))
	self:addModuleNPSS("snow_tube", CVector(0,0,29.3), 0, CVector(1,1,10))
	self:addModuleNPSS("snow_torus", CVector(0,0,20), 0, CVector(40,80,40))
	self:addModuleNPSS("snow_torus", CVector(0,0,10), 0, CVector(80,40,80))

	local m = self:addModuleNPSS("snow_target_300", CVector(0,0,0.3), 300, CVector(2,2,2)) m:setColor(CRGBA(255,255,255))
end
