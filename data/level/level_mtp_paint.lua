include("utilities.lua")
include("level_paint_server.lua")

ReleaseLevel = 6

function CLevel:init()

	--self:setAdvancedLevel(true)

	self:addExternalCamera(CVector(-0.305843, -15.203185, 0.845526), CAngleAxis(-0.002101, 0.711522, -0.702658, 0.002075))
	self:setSunParams()
	self:setSunRamp(0, 0, 0)

	self:addBox(0, -1) self:addBox(0, 0) self:addBox(0, 1) self:addBox(0, 2) self:addBox(0, 3) self:addBox(0, 4)
	self:addBox(1, 3) self:addBox(2, 2) self:addBox(3, 3)
	self:addBox(4, -1) self:addBox(4, 0) self:addBox(4, 1) self:addBox(4, 2) self:addBox(4, 3) self:addBox(4, 4)

	self:addBox(6, 4) self:addBox(7, 4) self:addBox(8, 4) self:addBox(9, 4) self:addBox(10, 4)
	self:addBox(8, -1) self:addBox(8, 0) self:addBox(8, 1) self:addBox(8, 2) self:addBox(8, 3)

	self:addBox(12, -1) self:addBox(12, 0) self:addBox(12, 1) self:addBox(12, 2) self:addBox(12, 3) self:addBox(12, 4)
	self:addBox(13, 4) self:addBox(14, 4)
	self:addBox(15, 2) self:addBox(15, 3) self:addBox(15, 4)
	self:addBox(13, 2) self:addBox(14, 2)

end

function CLevel:addBox(x, y)
	local m = self:addModuleNPSS("snow_box", CVector(-x*0.04, -y*0.04-15.12, 0.5), 0, CVector(2,2,1))
	m:setColor(CRGBA(255,255,120))
	m:setBounce(0)
	m:setAccel(0.00001)
	m:setFriction(10)
	m:setUserData(CModulePaintBloc:new(m))
end
