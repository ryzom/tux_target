---------------------- Level  ----------------------



---------------------- Entity ----------------------
local clientId = 0;

function Entity:init()
  self:setCurrentScore(0);
  --self:setStartPointId(clientId+getSessionId()*2);
  clientId = clientId + 1;
end

function Entity:preUpdate()
  self:setCurrentScore(0);
end

function Entity:update()
end

function entitySceneCollideEvent ( entity, module )
  module:collide(entity);
end

function entityEntityCollideEvent ( entity1, entity2 )
end

function entityWaterCollideEvent ( entity )
  entity:setCurrentScore(0);
end

function Module:collide( entity )
  if(entity:getIsOpen()==0 and self:getScore()~=0) then
    entity:setCurrentScore(self:getScore());
   end
end




