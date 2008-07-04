---------------------- Entity ----------------------

function CEntity:init()
	--self:displayText(0,12,1,255,200,0,"Get closest to the green bar", 60)
	--self:setMaxOpenClose(3) -- to be able to move on the target
end

function CEntity:preUpdate()
	self:setCurrentScore(0, false)
end

function CEntity:collideWithModule(module)
	if(self:isOpen()==0 and module:score()>0) then
		local pos = self:position()
		--print(self:name().." "..pos:getX().." "..pos:getY().." "..pos:getZ())
		local center = CVector(0.0, -15.35, 0.0)
		local x = center:getX() - pos:getX()
		local y = center:getY() - pos:getY()
		local dist = math.sqrt(x*x+y*y)
		score = 10.0*math.floor((400.0-dist*400.0/0.25)/10.0)
		--print(x.." "..y.." "..dist.." score:"..score.." "..self:currentScore())

		if score > 400 then
			score = 400
		elseif score < -200 then
			score = -200
		end

		self:setCurrentScore(score, false)
	end
end
