---------------------- Level  ----------------------

--setMaxLevelSessionCount(3);
setLevelHasBonusTime(0);
setLevelRecordBest(0);
setLevelTimeout(60);

local teamRedScore = 0;
local teamBlueScore = 0;
local currentTeamRedScore = 0;
local currentTeamBlueScore = 0;

---------------------- Entity ----------------------
CEntity = {}
CEntity_mt = {}
function CEntity:new(baseEntity)
  return setmetatable({base=baseEntity, team = 0, id = 0, bonusTime = 0, waterBonusResetOnce = 0 }, CEntity_mt)
end

function CEntity:print()
  table.foreach(self,print);
end

CEntity_mt.__index = CEntity

local clientId = 0;
local teamACount = 0;
local teamBCount = 0;

function CEntity:printTeam()
  if(self.team==0) then
	print("red");
  else
	print("blue");
  end;
end

function CEntity:getTeam()
  return self.team;
end

function CEntity:setTeamScore( score )
  --print("setTeamScore");
  --print(self.base:getCurrentScore());
  --print(score);
  if(score==0) then
    self.bonusTime = 0;
  end
  if(self.base:getCurrentScore() and score==0) then
    self.bonusTime = 0;
    --self.base:displayText(0,5,1,255,255,0,"bonus time left : 0",15);
  end
  
  if(self.base:getCurrentScore()==0 and score~=0) then
    --self.bonusTime = getTimeRemaining() * 5;
    --local bonus_string = "bonus time left : " .. self.bonusTime;
    --self.base:displayText(0,4,1,255,255,0,bonus_string,15);
  end
  
  --if(self.base:getCurrentScore()==score) then
  --  return;
  --end
  
  if(self.team==0) then
    --if(currentTeamRedScore>=score) then
    --  return;
    --end
    --currentTeamRedScore = score;
    currentTeamRedScore = currentTeamRedScore + score;
    self.base:setCurrentScore(score);
  else
    --if(currentTeamBlueScore>=score) then
    --  return;
    --end
    --currentTeamBlueScore = score;
    currentTeamBlueScore = currentTeamBlueScore + score;
    self.base:setCurrentScore(score);
  end
  --end    
end

function CEntity:setFinalScore()
  if(self.team==0) then
    self.base:setCurrentScore(teamRedScore + self.bonusTime);
  else
    self.base:setCurrentScore(teamBlueScore + self.bonusTime);
  end
    
end

function CEntity:setTeam( t )
  self.team = t;
end

function Entity:parent()
  return self:getUserData();
end

function Entity:init()
  local parent = CEntity:new(self);
  self:setCurrentScore(0);
  self:setUserData(parent);
  --clientId = self:getEid();
  --print("clientId");
  --print(clientId);
  --local t = math.mod(clientId,2);
  local t = self:getTeam(2);
  if(t==0) then
    self:setStartPointId(teamACount*2+getSessionId()*2);
    teamACount = teamACount+1;
  else
    self:setStartPointId(teamBCount*2+1+getSessionId()*2);
    teamBCount = teamBCount+1;
  end

  self:displayText(0,5,1,255,255,255,"warning : Team share the score",10);
  if(t==0) then
  execLuaOnAllClient("getEntityByName(\""..self:getName().."\"):setColor(255,0,0,255);");
  	self:displayText(0,6,1,255,0,0,"You are in RED team",15);
  	self:displayText(0,7,1,255,0,0,"Land on RED target !",20);
  else
  execLuaOnAllClient("getEntityByName(\""..self:getName().."\"):setColor(0,0,255,255);");
  	self:displayText(0,6,1,100,100,255,"You are in BLUE team",15);
  	self:displayText(0,7,1,100,100,255,"Land on BLUE target !",20);
  end

  parent:setTeam(t);
  print(self:getName());
  parent:printTeam();
  clientId = clientId + 1;
end

function Entity:preUpdate()
  --self:setCurrentScore(0);
end

function Entity:update()
end


function entitySceneCollideEvent ( entity, module )
  module:collide(entity);
end

function entityEntityCollideEvent ( entity1, entity2 )
end

function entityWaterCollideEvent ( entity )
  pos = CVector(1,2,3.456);
  pos = entity:getStartPointPos();
  --entity:setPos(pos);
  --entity:setOpenCloseCount(0);
  if(entity:parent().waterBonusResetOnce == 0) then
    entity:parent():setTeamScore(0);
    entity:parent().waterBonusResetOnce = 1;
  end
end

---------------------- Module ----------------------
CModule = {}
CModule_mt = {}
function CModule:new(s,t)
  return setmetatable({ team = t or 0, id = 0, son=s }, CModule_mt)
end

function CModule:getTeam()
  return self.team;
end

function CModule:setTeam( t )
  self.team = t;
end

function CModule:disable()
  self.son:setEnabled(0);
end

CModule_mt.__index = CModule



function Module:parent()
  return self:getUserData();
end


function Module:collide( entity )
  if(entity:getIsOpen()==1) then
    entity:setCurrentScore(0)
  else
    if(self:parent():getTeam()==entity:parent():getTeam()) then
      entity:parent():setTeamScore(self:getScore())
    else
      entity:parent():setTeamScore(-self:getScore())
    end
  end
  --print(entity:getName());
  --print(entity:parent():getTeam());
  --entity:setCurrentScore(self:getScore());
  
  --self:parent():disable();
end


------------- Level -------------

function levelInit()
  teamRedScore = 0;
  teamBlueScore = 0;
  clientId = 0;
  teamACount = 0;
  teamBCount = 0;
  print("levelinit");

end

function levelPreUpdate()

  currentTeamRedScore = 0;
  currentTeamBlueScore = 0;

end

function levelPostUpdate()

  if(currentTeamRedScore~=teamRedScore) then
    displayTextToAll(0,7,1,255,0,0,currentTeamRedScore,40);
    teamRedScore = currentTeamRedScore;
  end

  if(currentTeamBlueScore~=teamBlueScore) then
    displayTextToAll(0,8,1,100,100,255,currentTeamBlueScore,40);
    teamBlueScore = currentTeamBlueScore;
  end

end


function levelEndSession()
  local entityCount = getEntityCount();
  for i=0,entityCount do
    entity = getEntity(i);
    entity:parent():setFinalScore();
    --TODO we can remove the following line when version >= 1.2.2 is out
    execLuaOnAllClient("getEntityByName(\""..entity:getName().."\"):setColor(255,255,255,255);");
  end

end

