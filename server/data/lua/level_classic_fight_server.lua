---------------------- Level  ----------------------



---------------------- Entity ----------------------
local clientId = 0;

function Entity:init()
  self:displayText(0,5,1,255,200,0,"warning : In this level you can control pingoo on target",60);
  self:setCurrentScore(0);
  self:setOpenCloseMax(3);
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
end

function Module:collide( entity )
  if(entity:getIsOpen()==0) then
    entity:setCurrentScore(self:getScore());
   end
end




