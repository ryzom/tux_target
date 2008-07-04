include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 6

function CEntity:init()
	self:setMaxOpenClose(6)
end

function CLevel:init()

	--self:setAdvancedLevel(true)

	self:addExternalCamera(CVector(-1.207512, -8.511978, 2.723826), CAngleAxis(-0.098957, 0.098385, -0.698157, 0.702214))

	self:setRamp(0, 7, 8)

	self:addModuleNP("snow_island", CVector(10, -25, 3.5))
	local m = self:addModuleNP("snow_island2", CVector(15,-12,2)) m:setRotation(CAngleAxis(0,0,1,1.58))
	local m = self:addModuleNP("snow_island3", CVector(8,-6,2)) m:setRotation(CAngleAxis(0,0,1,2.58))
	local m = self:addModuleNP("snow_island2", CVector(-10,-20,2)) m:setRotation(CAngleAxis(0,0,1,-1.58))
	local m = self:addModuleNP("snow_island3", CVector(10,-20,2)) m:setRotation(CAngleAxis(0,0,1,-2.58))

	-- first vertical donuts
	local m = self:addModuleNPSS("snow_torus", CVector(2.5,-6,6), 0, CVector(200, 100, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(2.5,-6,6), 0, CVector(400, 200, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(2.5,-6,6), 0, CVector(800, 400, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))

	-- right wall to avoid cheat (thanks bady)
	self:addModuleNPSS("snow_box", CVector(-1.2,-8.2, 4.5), 0, CVector(1, 225, 50))

	-- second vertical donuts
	local m = self:addModuleNPSS("snow_torus", CVector(1,-8,5), 0, CVector(200, 100, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(1,-8,5), 0, CVector(400, 200, 20)) m:setRotation(CAngleAxis(10,0,0,1.57))

	-- vertical tube
	local m = self:addModuleNPSS("snow_funnel", CVector(2,-8.5,3.865), 0, CVector(4, 4, 2.1))
	self:addModuleNPSS("snow_tube", CVector(2,-8.5,3.51), 0, CVector(3,3,5))
	local m = self:addModuleNPSS("snow_torus", CVector(2,-8.5,3.965), 0, CVector(100, 100, 20)) m:setRotation(CAngleAxis(0,0,1,1.57))
	local m = self:addModuleNPSS("snow_torus", CVector(2,-8.5,4.06), 0, CVector(300, 300, 20)) m:setRotation(CAngleAxis(0,0,1,1.57))

	-- plateform
	self:addSunModule("box", "wood", 0, CVector(2,-8.8, 3.22), CVector(30, 1, 1))
	self:addSunModule("box", "wood", 0, CVector(2,-8.2, 3.22), CVector(30, 1, 1))
	self:addSunModule("box", "wood", 0, CVector(2,-8.5, 3.2), CVector(30, 30, 1))

	-- horizontal tube
	local m = self:addModuleNPSS("snow_tube", CVector(1.2,-8.5,3.25), 0, CVector(3,3,10))
	m:setRotation(CAngleAxis(0,10,0,1.57))
	m:setFriction(0)

	local m = self:addModuleNPSS("snow_torus", CVector(0,-8.5,3), 0, CVector(25, 100, 10))
	m:setRotation(CAngleAxis(0,10,0,1.57))

	self:addModuleNPSS("snow_box", CVector(-0.5,-8.5, 2.7), 50, CVector(50, 20, 1))
	m:setFriction(2)

	local m = self:addModuleNPSS("snow_torus", CVector(1.70,-8.5,3.25), 0, CVector(20, 30, 2))
	m:setRotation(CAngleAxis(0,1,0,1.57))

	-- vertical wall
	self:addModuleNPSS("snow_box", CVector(-1.2,-8.5, 2.6), 0, CVector(1, 10, 10))

	self:addModuleNPSS("snow_box", CVector(-0.0,-8.3, 2.6), 0, CVector(150, 1, 25))
	self:addModuleNPSS("snow_box", CVector(-0.0,-8.7, 2.6), 0, CVector(150, 1, 25))

	local m = self:addModuleNPSS("snow_box", CVector(-0.7,-8.5, 2.50), 100, CVector(50, 10, 1))
	m:setFriction(2)

	local m = self:addModuleNPSS("snow_box", CVector(-0.6,-8.5, 2.4), 300, CVector(50, 5, 1))
	m:setFriction(2)

end
