module:setBounce(0);
module:setCollide(0);
module:setScore(0);
module:setAccel(0.00001);
module:setFriction(1.0);

CModuleCP1 = {}
CModuleCP1_mt = {}
function CModuleCP1:new(s)
  return setmetatable({ son=s }, CModuleCP1_mt)
end


CModuleCP1_mt.__index = CModuleCP1

function CModuleCP1:onCollide( entity )
  if(entity:parent().team == 0) then
    entity:parent().team = 1;
    entity:displayText(0,5,1,255,255,0,"Check point",3);
    end
end

function CModuleCP1:onInit()
  self.son:setVisible(0);
end

module:setUserData(CModuleCP1:new(module)); --new CModule
