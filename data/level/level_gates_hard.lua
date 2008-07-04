include("utilities.lua")
include("level_gates_server.lua")

ReleaseLevel = 5

function CLevel:init()
	self:addExternalCamera(CVector(1.734350, -11.628866, 0.076265), CAngleAxis(0.016375, -0.027098, -0.855443, 0.516928))
	self:setSunParams()
	self:setSunRamp(0, 0, 0)

	self:addGatePS(CVector(0, -10, 3), 50)

	self:addGatePS(CVector(2.45, -10.91, 1.86), 50)
	self:addGatePS(CVector(3.19, -13.3, 0.78), 50)
	self:addGatePS(CVector(1.85, -11.80, 0.12), 50)

	self:addSunModule("box", "sand", 0, CVector(1.85, -11.678, 0.043), CVector(6.7,10,1))
end
