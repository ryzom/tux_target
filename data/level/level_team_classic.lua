include("utilities.lua")
include("level_team_server.lua")

ReleaseLevel = 3

function CLevel:init()
	self:addExternalCamera(CVector(-0.108815, -0.000864, 3.132983), CAngleAxis(-0.310493, 0.317307, -0.640444, 0.626691))

	self:setTeamRamp(0, 15, 5)

	self:setHasBonusTime(false)

	self:addDefaultIslandCenter()

	-- red team
	local m = self:addModuleNPSS("snow_box", CVector(0,0.40,3), 50, CVector(16, 16, 1)) m:setColor(CRGBA(255,128,128))
	m:setUserData(CModuleBase:new(0))
	local m = self:addModuleNPSS("snow_box", CVector(0,0.16,3), 100, CVector(8, 8, 1)) m:setColor(CRGBA(255,64,64))
	m:setUserData(CModuleBase:new(0))
	local m = self:addModuleNPSS("snow_box", CVector(0,0.04,3), 300, CVector(4, 4, 1)) m:setColor(CRGBA(255,0,0))
	m:setUserData(CModuleBase:new(0))

	-- blue team
	local m = self:addModuleNPSS("snow_box", CVector(0,-0.40,3), 50, CVector(16, 16, 1)) m:setColor(CRGBA(255,255,255))
	m:setUserData(CModuleBase:new(1))
	local m = self:addModuleNPSS("snow_box", CVector(0,-0.16,3), 100, CVector(8, 8, 1)) m:setColor(CRGBA(64,64,255))
	m:setUserData(CModuleBase:new(1))
	local m = self:addModuleNPSS("snow_box", CVector(0,-0.04,3), 300, CVector(4, 4, 1)) m:setColor(CRGBA(0,0,255))
	m:setUserData(CModuleBase:new(1))
end
