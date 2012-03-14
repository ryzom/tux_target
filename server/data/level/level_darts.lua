Name = "Snow darts"
Author = "Ace"
ServerLua = "level_darts_server.lua"
ReleaseLevel = 2

skyShapeFileName = "sky.shape";
sunAmbientColor = CRGBA(82, 100, 133, 255);
sunDiffuseColor = CRGBA(255, 255, 255, 255);
sunSpecularColor = CRGBA(255, 255, 255, 255);
sunDirection = CVector(-1,0,-1);

clearColor = CRGBA(30, 45, 90, 0);

fogDistMin = 0;
fogDistMax = 150;
fogColor = clearColor;

cameraMinDistFromStartPointToMove = 100.0;
cameraMinDistFromStartPointToMoveVerticaly = 100.0;

Cameras =
{
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10),
	CVector(0, 3, 10)
}

StartPoints =
{
	CVector(0.00,1.5,7.1),
	CVector(0.03,1.5,7.1),
	CVector(0.06,1.5,7.1),
	CVector(0.09,1.5,7.1),
	CVector(0.12,1.5,7.1),
	CVector(0.15,1.5,7.1),
	CVector(0.18,1.5,7.1),
	CVector(0.21,1.5,7.1),
	CVector(0.24,1.5,7.1),
	CVector(0.27,1.5,7.1),
	CVector(0.30,1.5,7.1),
	CVector(0.33,1.5,7.1),
	CVector(0.36,1.5,7.1),
	CVector(0.39,1.5,7.1),
	CVector(0.42,1.5,7.1),
	CVector(0.45,1.5,7.1),
}

Modules =
{
	{ Position = CVector(0.24,-3.574639,7.000000), Scale = CVector(50.000000, 500.000000, 2.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,0,0,0), Lua="snow_darts_accel", Shape="snow_box" },
	{ Position = CVector(-0.100000,-40.300000,1.000000), Scale = CVector(50.000000, 1.000000, 50.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="snow_target_50", Shape="snow_box" },
	{ Position = CVector(-0.100000,-40.284740,1.000000), Scale = CVector(10.000000, 1.000000, 10.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,0,255,255), Lua="snow_target_100", Shape="snow_box" },
	{ Position = CVector(-0.100000,-40.275701,1.000000), Scale = CVector(2.000000, 1.000000, 2.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,0,0,255), Lua="snow_target_300", Shape="snow_box" },

}

Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(-0.028859, -40.226116, 1.064289), Rotation = CAngleAxis(-0.094677, -0.228571, 0.895158, 0.370787) },
}
