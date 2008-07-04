include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(-0.199610, -15.302803, 2.095335), CAngleAxis(-0.502762, 0.491764, -0.497103, 0.508220))

	self:setRamp(0, 0, 5)

	self:addModuleNPSS("snow_box", CVector(0,-15.224501,1.051), 0, CVector(110, 1, 100))
	self:addModuleNPSS("snow_box", CVector(0,-15.384774,1.051), 0, CVector(110, 1, 100))
	
	self:addFunnel(-1, 50)
	self:addFunnel(-0.8, 0)
	self:addFunnel(-0.6, 100)
	self:addFunnel(-0.4, 0)
	self:addFunnel(-0.2, 300)
	self:addFunnel(0, 0)
	self:addFunnel(0.2, -300)
	self:addFunnel(0.4, 0)
	self:addFunnel(0.6, -100)
	self:addFunnel(0.8, 50)
	self:addFunnel(1, 50)
end

function CLevel:addFunnel(x, score)
	self:addModuleNP("snow_funnel", CVector(x,-15.3,2))
	self:addModuleNP("snow_tube", CVector(x,-15.3,1.9))
	self:addModuleNPSS("snow_box", CVector(x,-15.3,1.84), score, CVector(2,2,1))
end
