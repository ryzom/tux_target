Name = "Race"
ReleaseLevel = 0

Cameras =
{
	CVector(-3, 0, 10),
}

StartPoints =
{
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),
	CVector(0,0,0.790971),

}

Modules =
{
	{ Position = CVector(1.237044,-5.196737,3.016793), Scale = CVector(10, 30, 10), Shape="snow_box" },
	{ Position = CVector(1.461541,-5.196831,2.886570), Scale = CVector(10, 30, 10), Shape="snow_box" },
	{ Position = CVector(0.007647,-0.001760,1.045297), Scale = CVector(10, 30, 10), Shape="snow_box" },
	{ Position = CVector(3.294367,-2.475501,0.886570), Scale = CVector(10, 30, 20), Rotation = CAngleAxis(0,0,1,0.785300), Lua="race_check_point1", Shape="snow_box" },
	{ Position = CVector(0,-0.001563,0.886570), Scale = CVector(1, 1, 0.3), Shape="race_lane" },
	{ Position = CVector(0.903500,-0.179719,0.886570), Scale = CVector(1, 1, 0.3), Rotation = CAngleAxis(0,0,1,-0.785300), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(1.257156,-1.017967,0.886570), Scale = CVector(1, 1, 0.3), Rotation = CAngleAxis(0,0,1,2.356000), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(2.159279,-1.195915,0.886570), Scale = CVector(1, 1, 0.3), Shape="race_lane" },
	{ Position = CVector(3.105563,-1.195915,0.886570), Scale = CVector(1, 1, 0.3), Shape="race_lane" },
	{ Position = CVector(4.006260,-1.374733,0.886570), Scale = CVector(1, 1, 0.3), Rotation = CAngleAxis(0,0,1,-0.785300), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(4.008102,-2.220845,0.886570), Scale = CVector(1, 1, 0.3), Rotation = CAngleAxis(0,0,1,-2.356000), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(3.152957,-2.577263,0.886570), Scale = CVector(1, 1, 0.3), Rotation = CAngleAxis(0,0,1,-5.497500), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(2.798752,-3.427894,0.886570), Scale = CVector(1, 1, 0.3), Rotation = CAngleAxis(0,0,1,-2.356000), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(1.557217,-3.604819,0.886570), Scale = CVector(1.750000, 1, 0.3), Rotation = CAngleAxis(-10,1,0,0.350000), Lua="race_lane", Shape="race_jump" },
	{ Position = CVector(0.312616,-3.428512,0.886570), Scale = CVector(1, 1, 0.3), Rotation = CAngleAxis(0,0,1,2.356000), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(-0.043281,-2.575691,0.886570), Scale = CVector(1, 1, 0.3), Rotation = CAngleAxis(0,0,1,5.497500), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(-0.888687,-2.221027,0.886570), Scale = CVector(1, 1, 0.3), Rotation = CAngleAxis(0,0,1,2.356000), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(-0.887673,-0.177824,0.886570), Scale = CVector(1, 1, 0.3), Rotation = CAngleAxis(0,0,1,-5.497500), Lua="race_lane", Shape="race_turn_90" },
	{ Position = CVector(-1.065391,-1.196205,0.886570), Scale = CVector(1.250000, 1, 0.3), Rotation = CAngleAxis(0,0,1,1.570700), Lua="race_lane", Shape="race_lane" },
	{ Position = CVector(-0.027069,0.001463,0.886570), Scale = CVector(10, 30, 20), Lua="race_finish", Shape="snow_box" },
}

ExternalCameras =
{
}
