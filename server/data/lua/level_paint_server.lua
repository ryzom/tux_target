---------------------- Global  ----------------------



---------------------- Entity ----------------------
local clientId = 0;

function Entity:init()
  self:displayText(0,5,1,255,255,255,"paint in green as many block as possible",10);
  self:displayText(0,6,1,255,255,255,"other players paint in red",10);
  self:setCurrentScore(0);
  --self:setStartPointId(clientId+getSessionId()*2);
  clientId = clientId + 1;
  --clientId = math.random(2);
  --self:setMaxLinearVelocity(0.2);
end

function Entity:preUpdate()
  self:setCurrentScore(0);
end

function Entity:update()
end

function entitySceneCollideEvent ( entity, module )
  module:parent():onCollide(entity);
end

function entityEntityCollideEvent ( entity1, entity2 )
end

function entityWaterCollideEvent ( entity )
  pos = entity:getStartPointPos();
  --entity:setPos(pos);
  --entity:setOpenCloseCount(0);
end

---------------------- Module ----------------------

function Module:parent()
  return self:getUserData();
end


---------------------- Level  ----------------------

function levelPreUpdate()
  local moduleCount = getModuleCount();
  for i=0,moduleCount-1 do
    if(getModule(i):parent()) then
      getModule(i):parent():preUdate();
    end
  end
end

function levelPostUpdate()
  local moduleCount = getModuleCount();
  for i=0,moduleCount-1 do
    if(getModule(i):parent()) then
      getModule(i):parent():postUdate();
    end
  end
end

function levelEndSession()
  local moduleCount = getModuleCount();
  for i=0,moduleCount-1 do
    if(getModule(i):parent()) then
      getModule(i):parent():finalScore();
    end
  end
end


