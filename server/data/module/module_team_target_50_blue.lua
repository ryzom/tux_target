module:setBounce(1);
module:setScore(50);
module:setAccel(0.0001);
module:setFriction(5.0);
module:setShapeName("team_target_50_blue.shape");

module:setUserData(CModule:new(module,1)); --new CModule , init team to 0
