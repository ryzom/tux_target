include("utilities.lua")
include("level_gates_server.lua")

ReleaseLevel = 5

function CLevel:init()
	self:addExternalCamera(CVector(-0.009602, -13.534696, 0.335430), CAngleAxis(-0.000000, -0.000000, 0.005334, 0.999986))
	self:setSunParams()
	self:setSunRamp(0, 0, 0)

	self:addGatePS(CVector(0, -2.75, 1.27), 100)
	self:addGatePS(CVector(0.6, -0.65, 3.66), 100)
	self:addGatePS(CVector(-0.6, -0.65, 3.66), 100)

	self:addGatePS(CVector(0, -13.28, 0.4), 100)
	self:addSunModule("box", "sand", 0, CVector(0, -13.4, 0.3), CVector(10,10,1))
end
