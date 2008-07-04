---------------------- Level  ----------------------



---------------------- Entity ----------------------
local clientId = 0;

function Entity:init()
  self:setCurrentScore(0);
  self:setOpenCloseMax(0);
  --print(self:name());
  --print("entity init");
  --self:setStartPointId(clientId+getSessionId()*2);
  clientId = clientId + 1;
end

function Entity:preUpdate()
  self:setCurrentScore(0);
end

function Entity:update()
end

function entitySceneCollideEvent ( entity, module )
  --print(entity:name());
  --print(module:name());
  --print("entitySceneCollideEvent");
  --print(entity:name());
  --print(module:score());
  
  module:collide(entity);
  --entity:setCurrentScore(module:score());
end

function entityEntityCollideEvent ( entity1, entity2 )
  --print(entity1:name());
  --print(entity2:name());
  --print("entityEntityCollideEvent");
end

function entityWaterCollideEvent ( entity )
  --print(entity:name());
  --print("entityWaterCollideEvent");
end

function Module:collide( entity )
  if(entity:isOpen()==0) then
    entity:setCurrentScore(self:score());
   end
end
