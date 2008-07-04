
---------------------- Entity ----------------------

function CEntity:preUpdate()
	self:setCurrentScore(0, false)
end

function CEntity:collideWithModule(module)
	if(module:parent()) then
		module:parent():onCollide(self)
	end
end

---------------------- Level  ----------------------

function CLevel:postUpdate()
	local moduleCount = moduleCount()-1
	for i=0,moduleCount do
		if(module(i):parent()) then
			module(i):parent():postUpdate()
			module(i):parent():updateScore()
		end
	end
end

---------------------- Module ----------------------

function CModule:parent()
	return self:getUserData()
end

CModulePaintBloc = {}
CModulePaintBloc_mt = {}

function CModulePaintBloc:new(s)
	return setmetatable({ son=s, touchEntityId=-1, oldTouchEntityId=-1, oldTouchCount=0, touchCount=0 }, CModulePaintBloc_mt)
end

CModulePaintBloc_mt.__index = CModulePaintBloc

function CModulePaintBloc:postUpdate()
	if(self.touchCount~=self.oldTouchCount and self.touchCount ~= 0) then
		if(self.touchCount == 1) then
			if(level():teamMode()) then
				if(entityById(self.touchEntityId):team() == 0) then
					execLuaOnAllClient("moduleById("..self.son:getId().."):setColor(CRGBA(255,0,0))")
				else
					execLuaOnAllClient("moduleById("..self.son:getId().."):setColor(CRGBA(0,0,255))")
				end
			else
				execLuaOnAllButOneClient(self.touchEntityId,"moduleById("..self.son:getId().."):setColor(CRGBA(255,0,0))")
				execLuaOnOneClient(self.touchEntityId,"moduleById("..self.son:getId().."):setColor(CRGBA(0,255,0))")
			end
		else
			execLuaOnAllClient("moduleById("..self.son:getId().."):setColor(CRGBA(255,255,255))")
		end
	end
	self.oldTouchEntityId = self.touchEntityId
	self.oldTouchCount    = self.touchCount
end

function CModulePaintBloc:onCollide( entity )
	if(entity:isOpen()==0) then
		if(self.touchEntityId==-1) then
			self.touchEntityId=entity:getEid()
			self.touchCount = 1
		end
	end
end

function CModulePaintBloc:updateScore()
	if(self.touchCount == 1) then
		local e = entityById(self.touchEntityId)
		-- check if the entity wasn't disconnected
		if(e) then
			e:setCurrentScore(e:currentScore() + 100, false)
		end
	end
end
