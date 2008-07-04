
---------------------- Entity ----------------------

function CEntity:collideWithModule(module)
	if(self:isOpen()==0 and module:score()>0 and self:parent().newModuleId~=module:getId()) then
		self:parent().newModuleId = module:getId()
	end
end

function CEntity:collideWithWater()
	self:setCurrentScore(0)
	--self:displayText(0,14,1,100,255,100,"0",5)
end

CEntityBase = {}
CEntityBase_mt = {}
function CEntityBase:new(baseEntity)
	return setmetatable({base=baseEntity, currentModuleId = 0, newModuleId = 0 }, CEntityBase_mt)
end

CEntityBase_mt.__index = CEntityBase

function CEntity:parent()
	return self:getUserData()
end

function CEntity:init()
	local parent = CEntityBase:new(self)
	self:setUserData(parent)
	self:setCurrentScore(0)
	--self:displayText(0,12,1,255,200,0,"Go down the stairs : 1 step = 50 Points",60)
end

function CEntity:update()
	if(self:parent().currentModuleId ~= self:parent().newModuleId) then
		self:setCurrentScore(self:currentScore()+50)
		--self:displayText(0,14,1,100,255,100," "..self:currentScore(),5)
		self:parent().currentModuleId = self:parent().newModuleId
	end
end
