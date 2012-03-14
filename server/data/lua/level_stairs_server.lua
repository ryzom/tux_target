---------------------- Level  ----------------------



---------------------- Entity ----------------------
local clientId = 0;

CEntity = {}
CEntity_mt = {}
function CEntity:new(baseEntity)
  return setmetatable({base=baseEntity, currentModuleId = 0, newModuleId = 0, newModuleScore = 0 }, CEntity_mt)
end

function CEntity:print()
  table.foreach(self,print);
end

CEntity_mt.__index = CEntity

function Entity:parent()
  return self:getUserData();
end




function Entity:init()
  local parent = CEntity:new(self);
  self:setUserData(parent);
  self:setCurrentScore(0);
  --self:setStartPointId(clientId+getSessionId()*2);
  clientId = clientId + 1;
  self:displayText(0,5,1,255,200,0,"go down the stairs : 1 step = 50 Points",60);
end

function Entity:preUpdate()
  --self:setCurrentScore(0);
end

function Entity:update()
  if(self:parent()==nil) then
    return;
  end
  
  if(self:parent().currentModuleId~=self:parent().newModuleId) then
    local score = self:getCurrentScore();
    self:setCurrentScore(score+50);
    self:displayText(0,7,1,100,255,100," "..self:getCurrentScore(),5);
    self:parent().currentModuleId=self:parent().newModuleId;
  end
end

function entitySceneCollideEvent ( entity, module )
  module:collide(entity);
end

function entityEntityCollideEvent ( entity1, entity2 )
end

function entityWaterCollideEvent ( entity )
  entity:setCurrentScore(0);
  entity:displayText(0,7,1,100,255,100,"0",5);
end

function Module:collide( entity )
  --entity:displayText(0,8,1,100,255,100,self:getId().."/"..entity:parent().newModuleId,20);
  if(entity:getIsOpen()==0 and self:getScore()~=0 and entity:parent().newModuleId~=self:getId()) then
    entity:parent().newModuleId = self:getId();
    entity:parent().newModuleScore = self:getScore();
  end
end




