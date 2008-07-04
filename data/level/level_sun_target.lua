include("utilities.lua")
include("level_default_server.lua")

ReleaseLevel = 5

function CLevel:init()
	self:addExternalCamera(CVector(0.006473, -14.590682, 0.227834), CAngleAxis(0.000075, -0.436466, 0.899721, -0.000154))
	self:setSunParams()
	self:setSunRamp(0, 0, 0)

	for x = 0, 4 do
		for y = 0, 4 do
			if x == 0 or y == 0 or x == 4 or y == 4 then
				self:addSunModule("box", "sand", 50, CVector(-2/10+x/10, -15+y/10, 0.1), CVector(5,5,1))
			elseif x == 1 or y == 1 or x == 3 or y == 3 then
				self:addSunModule("box", "wood", 100, CVector(-2/10+x/10, -15+y/10, 0.1), CVector(5,5,1))
			else
				self:addSunModule("box", "ice", 300, CVector(-2/10+x/10, -15+y/10, 0.1), CVector(5,5,1))
			end
		end
	end
end
