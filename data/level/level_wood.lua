include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 2

function CLevel:init()
	self:addExternalCamera(CVector(0.063327, -7.449109, 1.503763), CAngleAxis(-0.469483, -0.313699, 0.458532, 0.686241))

	-- custom ramp
	self:setCameras()
	self:setStartPoints(0, 1.881427, 3.16)
	local m = self:addModuleNP("wood_ramp", CVector(0, 0, 2)) m:setAccel(0.00012) m:setFriction(0) m:setBounce(0)

	self:addModuleNPSS("wood_target", CVector(0.027,-7.416,1.284), 300, CVector(0.5, 0.5, 0.6))
	self:addModuleNPSS("wood_target", CVector(-0.025,-7.363,1.226), 100, CVector(1.2, 1.2, 1.2))
	self:addModuleNPSS("wood_target", CVector(0,-7.577,1.335), 50, CVector(2, 2, 2))
	self:addModuleNPSS("wood_target", CVector(0.015,-7.454,1.313), 300, CVector(0.5, 0.5, 0.6))
	self:addModuleNPSS("wood_target", CVector(-0.065,-7.439,1.288), 100, CVector(1.2, 1.2, 1.2))
	self:addModuleNPSS("wood_target", CVector(0.106,-7.498,1.143), 50, CVector(2, 2, 2))
	self:addModuleNPSS("wood_target", CVector(0.054,-7.444,1.295), 300, CVector(0.5, 0.5, 0.6))
	self:addModuleNPSS("wood_target", CVector(0.075,-7.358,1.244), 100, CVector(1.2, 1.2, 1.2))
end
