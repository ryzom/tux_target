
---------------------- Entity ----------------------

function CEntity:preUpdate()
	self:setCurrentScore(0, false)
end

function CEntity:collideWithModule(module)
	if(self:isOpen()==0 and module:score()>0 and module:parent()~=nil) then
		if(module:parent():team()==self:team()) then
			self:setCurrentScore(module:score(), false)
		else
			self:setCurrentScore(-module:score(), false)
		end
	end
end

---------------------- Module ----------------------

CModuleBase = {}
CModuleBase_mt = {}
function CModuleBase:new(t)
	return setmetatable({ teamid = t }, CModuleBase_mt)
end

function CModuleBase:team()
	return self.teamid
end

function CModuleBase:setTeam( t )
	self.teamid = t
end

CModuleBase_mt.__index = CModuleBase

function CModule:parent()
	return self:getUserData()
end
