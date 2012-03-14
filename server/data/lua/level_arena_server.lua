---------------------- Level  ----------------------



---------------------- Entity ----------------------
local clientId = 0;

function Entity:init()
  self:setCurrentScore(0);
  --print(self:getName());
  --print("entity init");
  --self:setStartPointId(clientId+getSessionId()*2);
  clientId = clientId + 1;
  self:setMaxLinearVelocity(0.2);
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
  pos = entity:getStartPointPos();
  entity:setPos(pos);
  --entity:setOpenCloseCount(0);
end

function Module:collide( entity )
end




