include("utilities.lua")
include("level_paint_server.lua")

ReleaseLevel = 5

function CLevel:init()
	self:addExternalCamera(CVector(0.118395, -15.095260, 0.653841), CAngleAxis(-0.561198, -0.000700, 0.001032, 0.827681))
	self:setSunParams()
	self:setSunRamp(0, 0, 0)

	for x = 0, 6 do
		for y = 0, 6 do
			if math.mod(x,2) == 0 or math.mod(y,2) == 0 then
				local m = self:addModuleNPSS("snow_box", CVector(x*0.04, y*0.04-15.12, 0.5), 0, CVector(2,2,1))
				m:setColor(CRGBA(255,255,120))
				m:setBounce(0)
				m:setAccel(0.00001)
				m:setFriction(10)
				m:setUserData(CModulePaintBloc:new(m))
			end
		end
	end
end
