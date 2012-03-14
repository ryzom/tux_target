---------------------- Level  ----------------------

--setMaxLevelSessionCount(3);
--setLevelHasBonusTime(0);
setLevelRecordBest(0);
setLevelTimeout(60);

local arrivedCount = 0;
---------------------- Entity ----------------------
CEntity = {}
CEntity_mt = {}
function CEntity:new(baseEntity)
  return setmetatable({base=baseEntity, team = 0, id = 0, startTargetTime = 0, lastPos, lastPosTime=60 }, CEntity_mt)
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

function CEntity:getTeam()
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
  self:setDefaultAccel(0.000001);
  self:setDefaultFriction(0.0);
  local parent = CEntity:new(self);
  self:setOpenCloseMax(2);
  self:setCurrentScore(0);
  self:setUserData(parent);
  --self:setStartPointId(clientId+getSessionId()*2);
  clientId = clientId + 1;
  self:setMaxLinearVelocity(1);
  parent.team = 0;
end

function Entity:onOpenCloseSwitch()
  self:setOpenCloseCount(0);
end

function Entity:preUpdate()
  --self:setCurrentScore(0);
end

function Entity:update()
end



function entityLeaveEvent ( entity )
end

function entitySceneCollideEvent ( entity, module )
  if(entity:parent().lastPosTime > (getTimeRemaining()+1)) then
    entity:parent().lastPos = entity:getPos();
    entity:parent().lastPosTime = getTimeRemaining();
  end
  module:parent():onCollide(entity);
end

function entityEntityCollideEvent ( entity1, entity2 )
end

function entityWaterCollideEvent ( entity )
  pos = entity:getStartPointPos();
  pos = entity:parent().lastPos;
  entity:setPos(pos);
  entity:setOpenCloseCount(0);
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

function Module:init()
  self:parent():onInit();
end


--function Module:collide( entity )
-- self:parent():onCollide(entity);
--end


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
  local entityCount = getEntityCount();
  for i=0,entityCount do
    getEntity(i):parent():setFinalScore();
  end

end

