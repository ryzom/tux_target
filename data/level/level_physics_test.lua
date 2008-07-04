include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 200

function CLevel:init()
--	self:setRamp(0, 0, 5)
--	for i = -0.5, 0.5, 0.05 do
--		self:addStartPoint(CVector(i+0.025, 1.6, 5+4.38))
--		self:addStartPoint(CVector(i, 1.575, 5+4.36))
--		self:addStartPoint(CVector(i+0.025, 1.575, 5+4.36))
--		self:addStartPoint(CVector(i, 1.55, 5+4.34))
--		self:addStartPoint(CVector(i+0.025, 1.55, 5+4.34))
--	end

	for x = 0.01, 0.18, 0.021 do
		for y = 0.01, 0.18, 0.021 do
			self:addStartPoint(CVector(x-0.1,-15.3+y,2.5))
			self:addStartPoint(CVector(x-0.1,-15.3+y,2.45))
		end
	end

	self:addFunnel(0, 0)

	self:addModuleNPSS("snow_funnel", CVector(0,-15.3,2.20), 0, CVector(4, 4, 3))

end

function CLevel:addFunnel(x, score)
	self:addModuleNP("snow_funnel", CVector(x,-15.3,2))
	self:addModuleNPSS("snow_tube", CVector(x,-15.3,1.8), 0, CVector(1,1,3))
	self:addModuleNPSS("snow_box", CVector(x,-15.3,1.65), 300, CVector(2,2,1))
end
