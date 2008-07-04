include("utilities.lua")
include("level_gates_server.lua")

ReleaseLevel = 5

function CLevel:init()
	self:addExternalCamera(CVector(-0.126548, -18.097857, 0.239454), CAngleAxis(-0.028337, 0.009835, -0.327733, 0.944294))
	self:setSunParams()
	self:setSunRamp(0, 0, 0)

	self:addGate90PS(CVector(0, -13, 2), 50)
	self:addGate90PS(CVector(0, -14, 2-0.35), 50)
	self:addGate90PS(CVector(0, -15, 2-2*0.35), 50)
	self:addGate90PS(CVector(0, -16, 2-3*0.35), 50)
	self:addGate90PS(CVector(0, -17, 2-4*0.35), 50)
	self:addGate90PS(CVector(0, -18, 2-5*0.35), 50)

	self:addSunModule("box", "sand", 0, CVector(0, -18, 0.192), CVector(10,6.7,1))
end
