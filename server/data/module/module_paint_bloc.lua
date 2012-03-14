module:setBounce(0);
module:setScore(0);
module:setAccel(0.00001);
module:setFriction(10.0);

CModulePaintBloc = {}
CModulePaintBloc_mt = {}
function CModulePaintBloc:new(s)
  return setmetatable({ son=s, touchEntityId=-1, oldTouchEntityId=-1, oldTouchCount=0, touchCount=0 }, CModulePaintBloc_mt)
end


CModulePaintBloc_mt.__index = CModulePaintBloc

function CModulePaintBloc:preUdate()
  --self.touchEntityId = -1;
  --self.touchCount=0;
end

function CModulePaintBloc:postUdate()
  if(self.touchCount~=self.oldTouchCount and self.touchCount ~= 0) then
    if(self.touchCount == 1) then
      execLuaOnAllButOneClient(self.touchEntityId,"getModuleById("..self.son:getId().."):setColor(255,0,0,255);");
      execLuaOnOneClient(self.touchEntityId,"getModuleById("..self.son:getId().."):setColor(0,255,0,255);");
    else
      execLuaOnAllClient("getModuleById("..self.son:getId().."):setColor(255,255,255,255);");
    end
  end
    self.oldTouchEntityId = self.touchEntityId;
    self.oldTouchCount    = self.touchCount;
end

function CModulePaintBloc:onCollide( entity )
--  if(self.touchEntityId~=entity:getEid()) then
  if(self.touchEntityId==-1) then
    self.touchEntityId=entity:getEid();
    --self.touchCount = self.touchCount + 1;
    self.touchCount = 1;
  end
end

function CModulePaintBloc:finalScore()
  if(self.touchCount == 1) then
    local score = getEntityById(self.touchEntityId):getCurrentScore();
    getEntityById(self.touchEntityId):setCurrentScore(score + 100);
  end
end

function CModulePaintBloc:onInit()
end


module:setUserData(CModulePaintBloc:new(module)); --new CModulePaintBloc
