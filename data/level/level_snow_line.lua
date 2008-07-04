include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 5

function CLevel:init()
	self:addExternalCamera(CVector(-0.001414, -15.083941, 2.546129), CAngleAxis(-0.084898, -0.000053, 0.000623, 0.996390))

	self:setRamp(0, 0, 5)
	self:addDefaultIsland()

	local y=0
	self:addSunModule("box", "ice", 300, CVector(0, -15+y/10, 2.5), CVector(3,5,1)); y=y+1
	self:addSunModule("box", "ice", 100, CVector(0, -15+y/10, 2.5), CVector(3,5,1)); y=y+1
	self:addSunModule("box", "ice", 100, CVector(0, -15+y/10, 2.5), CVector(3,5,1)); y=y+1
	self:addSunModule("box", "ice",  50, CVector(0, -15+y/10, 2.5), CVector(3,5,1)); y=y+1
	self:addSunModule("box", "ice",  50, CVector(0, -15+y/10, 2.5), CVector(3,5,1)); y=y+1
	self:addSunModule("box", "ice",  50, CVector(0, -15+y/10, 2.5), CVector(3,5,1)); y=y+1
end
