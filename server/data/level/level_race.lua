Name = "Race"
Author = "Skeet"
ServerLua = "level_race_server.lua"
ReleaseLevel = 0

skyShapeFileName = "sky.shape";
sunAmbientColor = CRGBA(82, 100, 133, 255);
sunDiffuseColor = CRGBA(255, 255, 255, 255);
sunSpecularColor = CRGBA(255, 255, 255, 255);
sunDirection = CVector(-1,0,-1);

clearColor = CRGBA(30, 45, 90, 0);

fogDistMin = 0;
fogDistMax = 150;
fogColor = clearColor;

cameraMinDistFromStartPointToMove = 0.5;

Cameras =
{
	CVector(-3, 0, 10),
}

StartPoints =
{
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),
	CVector(0.000000,0.000000,0.790971),

}

Modules =
{
	{ Position = CVector(1.237044,-5.196737,3.016793), Scale = CVector(10.000000, 30.000000, 10.000000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,0.000000), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(1.461541,-5.196831,2.886570), Scale = CVector(10.000000, 30.000000, 10.000000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,0.000000), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(0.007647,-0.001760,1.045297), Scale = CVector(10.000000, 30.000000, 10.000000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,0.000000), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(3.294367,-2.475501,0.886570), Scale = CVector(10.000000, 30.000000, 20.000000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,0.785300), Lua="race_check_point1", Shape="box_sol" },
	{ Position = CVector(0.000000,-0.001563,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,0.000000), Lua="race_lane", Shape="race_lane" },
	{ Position = CVector(0.903500,-0.179719,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,-0.785300), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(1.257156,-1.017967,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,2.356000), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(2.159279,-1.195915,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,0.000000), Lua="race_lane", Shape="race_lane" },
	{ Position = CVector(3.105563,-1.195915,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,0.000000), Lua="race_lane", Shape="race_lane" },
	{ Position = CVector(4.006260,-1.374733,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,-0.785300), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(4.008102,-2.220845,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,-2.356000), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(3.152957,-2.577263,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,-5.497500), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(2.798752,-3.427894,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,-2.356000), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(1.557217,-3.604819,0.886570), Scale = CVector(1.750000, 1.000000, 0.300000), Rotation = CAngleAxis(-10.000000,1.000000,0.000000,0.350000), Lua="race_lane", Shape="race_jump" },
	{ Position = CVector(0.312616,-3.428512,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,2.356000), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(-0.043281,-2.575691,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,5.497500), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(-0.888687,-2.221027,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,2.356000), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(-0.887673,-0.177824,0.886570), Scale = CVector(1.000000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,-5.497500), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(-1.065391,-1.196205,0.886570), Scale = CVector(1.250000, 1.000000, 0.300000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,1.570700), Lua="race_lane", Shape="race_lane" },
	{ Position = CVector(-0.027069,0.001463,0.886570), Scale = CVector(10.000000, 30.000000, 20.000000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,0.000000), Lua="race_finish", Shape="box_sol" },
}

Particles =
{
}

ExternalCameras =
{
}
