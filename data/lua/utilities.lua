
--------------------------------------------------------------------
-- GENERIC FUNCTIONS

function CLevel:setStartPoints(px, py, pz)
	for i = -0.5, 0.5, 0.05 do
		self:addStartPoint(CVector(px+i, py, pz))
	end
end

function CLevel:setCameras()
	for i = 0, 16 do
		self:addCamera(CVector(0, 3, 10))
		--nlinfo("adding default camera")
	end
end

--------------------------------------------------------------------
-- SUN

function CLevel:setSunParams()
	self:setWater("sun")
	self:setSky("sun")
	self:setFog(50, 110, CRGBA(168,170,185))
	self:setSun(CRGBA(128,128,128), CRGBA(255,255,255), CRGBA(255,255,255), CVector(-1,0,-1))
end

function CLevel:setSunRamp(px, py, pz)
	local m = self:addModuleNP("sun_ramp", CVector(px,py,pz+2))
	m:setBounce(0)
	m:setFriction(0)
	self:setStartPoints(px+0, py+1.6, pz+2+4.38)
	self:setCameras()
end

function CLevel:addSunModule(name, mat, score, pos, scale)
	local m = self:addModule()
	if(mat == "ice") then
		m:setName(name.."_score_env")
	else
		m:setName(name.."_score")
	end
	m:setTexture(0, "material_"..mat)
	if(score == 0) then
		m:setTexture(1, "empty")
	else
		m:setTexture(1, "score_"..score)
	end
	m:setScore(score)
	m:setPosition(pos)
	m:setScale(scale)
	m:setColor(CRGBA(255,255,255))
	if(mat == "sand") then m:setFriction(15)
	elseif(mat == "wood") then m:setFriction(10)
	elseif(mat == "ice") then m:setFriction(5)
	end
	return m
end


--------------------------------------------------------------------
-- GATES

function CLevel:addGatePS(pos, score)
	local m = self:addModuleNP("gate", pos)
	local g = self:addGate()
	g:setPosition(pos)
	g:setScale(CVector(14,3,14))
	g:setScore(score)
	g:setModule(m)
	return g
end

function CLevel:addGate90PS(pos, score)
	local m = self:addModuleNP("gate", pos)
	m:setRotation(CAngleAxis(0,0,1,3.14/2))

	local g = self:addGate()
	g:setPosition(pos)
	g:setScale(CVector(3,14,14))
	g:setScore(score)
	return g
end


--------------------------------------------------------------------
-- SNOW

function CLevel:setRamp(px, py, pz)
	local m = self:addModuleNP("snow_ramp", CVector(px, py, pz))
	m:setBounce(0)
	m:setFriction(0)
	self:setStartPoints(px, py+1.6, pz+4.38)
	self:setCameras()
end

function CLevel:setTeamRamp(px, py, pz)
	local m = self:addModuleNP("snow_ramp", CVector(px, py, pz))
	m:setBounce(0)
	m:setFriction(0)
	m:setColor(CRGBA(255,200,200))

	local m = self:addModuleNP("snow_ramp", CVector(px, -py, pz))
	m:setBounce(0)
	m:setFriction(0)
	m:setColor(CRGBA(200,200,255))
	m:setRotation(CAngleAxis(0,0,1,3.1415))

	for i = -0.5, 0.5, 0.05 do
		self:addStartPoint(CVector(px+i, py+1.6, pz+4.38))
		self:addCamera(CVector(0, 3, 10))
		self:addStartPoint(CVector(px+i, -py-1.6, pz+4.38))
		self:addCamera(CVector(0, -3, 10))
	end
end

function CLevel:addModuleNPSS(name, pos, score, scale)
	local m = self:addModule()
	m:setName(name)
	m:setPosition(pos)
	m:setScale(scale)
	m:setScore(score)
	if(score == 300) then m:setColor(CRGBA(255,0,0))
	elseif(score == 100) then m:setColor(CRGBA(0,0,255))
	elseif(score == 50) then m:setColor(CRGBA(0,255,0))
	end
	return m
end

function CLevel:addModuleNPS(name, pos, score)
	return self:addModuleNPSS(name, pos, score, CVector(1,1,1))
end

function CLevel:addModuleNP(name, pos)
	return self:addModuleNPS(name, pos, 0)
end

function CLevel:addDefaultIsland()
	self:addModuleNP("snow_island", CVector(10, -25, 3.5))
	self:addModuleNP("snow_island2", CVector(6, -14, 1.8))
	self:addModuleNP("snow_island3", CVector(-3.8, -17, 2))
end

function CLevel:addDefaultIslandCenter()
	self:addModuleNP("snow_island", CVector(10,-10,3.5))
	self:addModuleNP("snow_island", CVector(-10,10,3.5))
	self:addModuleNP("snow_island2", CVector(6,3,1.8))
	self:addModuleNP("snow_island3", CVector(-3.8,-3,2))
end
