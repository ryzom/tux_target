include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 6

function CLevel:init()

	--self:setAdvancedLevel(true)

	self:addExternalCamera(CVector(0.335299, -14.652275, 0.184275), CAngleAxis(-0.094143, -0.255560, 0.902884, 0.332605))

	self:setSunParams()
	self:setSunRamp(0, 0, 0)

	self:addSunModule("sign", "wood", 300, CVector(0.02,-15.02,0.12), CVector(2.5,2.5,2.5))
	self:addSunModule("sign", "wood", 100, CVector(0.22,-15.02,0.12), CVector(2.5,2.5,2.5))
	self:addSunModule("sign", "wood", 50, CVector(0.32,-15.02,0.12), CVector(2.5,2.5,2.5))

	local m = self:addSunModule("box", "ice", 0, CVector(0,-15,0.11), CVector(2.5,2.5,1))
	m:setScore(300)

	local m = self:addSunModule("box", "wood", 0, CVector(0,-15.125,0.11), CVector(2.5,10,1))
	m:setScore(100)
	local m = self:addSunModule("box", "wood", 0, CVector(0,-14.875,0.11), CVector(2.5,10,1))
	m:setScore(100)
	local m = self:addSunModule("box", "wood", 0, CVector(-0.125,-15,0.11), CVector(10,2.5,1))
	m:setScore(100)
	local m = self:addSunModule("box", "wood", 0, CVector(0.125,-15,0.11), CVector(10,2.5,1))
	m:setScore(100)

	local m = self:addSunModule("box", "sand", 0, CVector(0.275,-15,0.11), CVector(5,32.5,1))
	m:setScore(50)
	local m = self:addSunModule("box", "sand", 0, CVector(-0.275,-15,0.11), CVector(5,32.5,1))
	m:setScore(50)
	local m = self:addSunModule("box", "sand", 0, CVector(0,-15.275,0.11), CVector(22.5,5,1))
	m:setScore(50)
	local m = self:addSunModule("box", "sand", 0, CVector(0,-14.725,0.11), CVector(22.5,5,1))
	m:setScore(50)

end
