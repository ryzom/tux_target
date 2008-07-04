---------------------- Level  ----------------------

--setMaxLevelSessionCount(3);
setLevelHasBonusTime(0);


---------------------- Entity ----------------------
CEntity = {}
CEntity_mt = {}
function CEntity:new(baseEntity)
  return setmetatable({base=baseEntity, team = 0, id = 0, startTargetTime = 0 }, CEntity_mt)
end

function CEntity:print()
  table.foreach(self,print);
end

CEntity_mt.__index = CEntity

local clientId = 0;

function CEntity:printTeam()
  if(self.team==0) then
	print("red");
  else
	print("blue");
  end;
end

function CEntity:team()
  return self.team;
end


function CEntity:setFinalScore()
    
end

function CEntity:setTeam( t )
  self.team = t;
end

function Entity:parent()
  return self:getUserData();
end

local targetName;

function Entity:init()
  self:enableOpenCloseCommand(0);
  self:setDensity(1);
  self:setDefaultAccel(0.000005);
  self:setDefaultFriction(10.0);
  local parent = CEntity:new(self);
  self:setOpenCloseMax(2);
  self:setCurrentScore(0);
  self:setUserData(parent);
  --self:setStartPointId(clientId+getSessionId()*2);
  if(clientId==0) then
    t = 0;
  else
    t = 1;
  end
  if(t==0) then
  	self:displayText(0,12,1,255,0,0,"You are the target",15);
  	targetName = self:name();
  	parent.startTargetTime = getLevelTimeout();
  	execLuaOnAllClient("entityByName(\""..self:name().."\"):setColor(255,255,0,255);");
    self:setDensity(5);
  else
  	self:displayText(0,12,1,255,0,0,"You have to hit : "..targetName,15);
  	execLuaOnAllClient("entityByName(\""..self:name().."\"):setColor(255,255,255,255);");
  end

  parent:setTeam(t);
  print(self:name());
  parent:printTeam();
  clientId = clientId + 1;
end

function Entity:preUpdate()
  --self:setCurrentScore(0);
end

function Entity:update()
end


function swapRabbit(oldRabbit, newRabbit)
  	oldRabbit:parent().team = 1;
  	newRabbit:parent().team = 0;
  	newRabbit:displayText(0,13,1,255,255,0,"you hit target",15);
  	oldRabbit:displayText(0,13,1,255,0,0,"you have been hit",15);
  	displayTextToAll(0,12,1,255,0,0,"new target : "..newRabbit:name(),10);
  	oldRabbit:setCurrentScore(oldRabbit:currentScore() + (oldRabbit:parent().startTargetTime - timeRemaining()) * 10);
  	execLuaOnAllClient("entityByName(\""..oldRabbit:name().."\"):setCurrentScore("..oldRabbit:currentScore()..");");
  	newRabbit:parent().startTargetTime = timeRemaining();
    oldRabbit:displayText(0,14,1,255,255,255,"score : "..oldRabbit:currentScore(),10);
  	execLuaOnAllClient("entityByName(\""..newRabbit:name().."\"):setColor(255,255,0,255);");
  	execLuaOnAllClient("entityByName(\""..oldRabbit:name().."\"):setColor(255,255,255,255);");
        oldRabbit:setDensity(1);
        newRabbit:setDensity(5);
end

function entityLeaveEvent ( entity )
  if(entity:parent().team==0) then
  local entityCount = entityCount()-1;
    print("player leave");
    print(entity:name());
    for i=0,entityCount do
        print(entity(i):getEid());
        print(entity(i):name());
      if(entity(i):getEid()~=entity:getEid()) then
        entity(i):parent().team = 0;
  	entity(i):parent().startTargetTime = timeRemaining();
  	displayTextToAll(0,12,1,255,0,0,"New target : "..entity(i):name(),15);
  	break;
      end
    end
  end
end

function entitySceneCollideEvent ( entity, module )
  module:collide(entity);
end

function entityEntityCollideEvent ( entity1, entity2 )
  if(entity1:parent().team==entity2:parent().team) then
    return
  end
    
  if(entity1:parent().team==0) then
    swapRabbit(entity1,entity2);
  else
    swapRabbit(entity2,entity1);
  end
end

function entityWaterCollideEvent ( entity )
  pos = entity:startPointPos();
  entity:setPosition(pos);
  --entity:setOpenCloseCount(0);
end

---------------------- Module ----------------------
CModule = {}
CModule_mt = {}
function CModule:new(s,t)
  return setmetatable({ team = t or 0, id = 0, son=s }, CModule_mt)
end

function CModule:team()
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
end


------------- Level -------------

function levelInit()
  clientId = 0;
  print("levelinit");

end

function levelPreUpdate()

end

function levelPostUpdate()

end


function levelEndSession()
  local entityCount = entityCount()-1;
  for i=0,entityCount do
    entity(i):parent():setFinalScore();
  end
end
