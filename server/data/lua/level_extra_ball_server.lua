---------------------- Level  ----------------------
setLevelHasBonusTime(0);
math.randomseed( os.time() )


---------------------- Entity ----------------------
local clientId = 0;

function Entity:init()
  self:enableCrashInFly(0);
  self:setCurrentScore(0);
  --self:setStartPointId(clientId+getSessionId());
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
  --entity:setCurrentScore(0);
  if(entity:getIsOpen()==0) then
    pos = entity:getStartPointPos();
    entity:setPos(pos);
    entity:setOpenCloseCount(0);
    entity:setFreezCommand(0);
    entity:displayText(0,6,1,0,255,0,"Extra Ball !!",40);
  end
end

function Module:init()
  if(self:getScore()>0) then
    newPos = CVector(2*(math.random(100)/50-0.5), -14.33, 2.8);
    self:setPos(newPos);
  end
end

function Module:collide( entity )
  if(entity:getIsOpen()==0 and self:getScore()>0 and entity:getMeanVelocity()<0.03) then
    self:setEnabled(0);
    entity:setCurrentScore(self:getScore()+entity:getCurrentScore());
    pos = entity:getStartPointPos();
    entity:setPos(pos);
    entity:setOpenCloseCount(0);
    newPos = CVector(2*(math.random(100)/50-0.5), -14.33, 2.8);
    self:setPos(newPos);
    self:setEnabled(1);
    winnerStringMsg = entity:getName().." lands on target and get 300 points";
    displayTextToAll(0,5,1,255,200,0,winnerStringMsg,40);
    entity:displayText(0,6,1,0,255,0,"Extra Ball !!",40);
   end
end
