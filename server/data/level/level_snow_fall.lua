Name = "Snow fall"
Author = "Ace"
ServerLua = "level_snow_fall_server.lua"
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

CrashedClientCollide = 0;

Cameras =
{
	CVector(0, 1, 10),
}

StartPoints =
{
	CVector(0.050000,0.000000,30.100000),
	CVector(-0.050000,0.000000,30.100000),
	CVector(0.050000,0.000000,30.100000),
	CVector(-0.050000,0.000000,30.100000),
	CVector(0.025000,0.025000,30.100000),
	CVector(-0.025000,0.025000,30.100000),
	CVector(0.025000,-0.025000,30.100000),
	CVector(-0.025000,-0.025000,30.100000),
	CVector(0.050000,0.000000,30.200001),
	CVector(-0.050000,0.000000,30.200001),
	CVector(0.050000,0.000000,30.200001),
	CVector(-0.050000,0.000000,30.200001),
	CVector(0.025000,0.025000,30.200001),
	CVector(-0.025000,0.025000,30.200001),
	CVector(0.025000,-0.025000,30.200001),
	CVector(-0.025000,-0.025000,30.200001),

}

Modules =
{
	{ Position = CVector(0.000000,0.000000,30.000000), Scale = CVector(1.000000, 1.000000, 4.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(0.000000,0.000000,29.298079), Scale = CVector(1.000000, 1.000000, 10.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_tube" },
	{ Position = CVector(0.000000,0.000000,20.000000), Scale = CVector(40.000000, 80.000000, 40.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_torus" },
	{ Position = CVector(0.000000,0.000000,10.000000), Scale = CVector(80.000000, 40.000000, 40.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_torus" },
	{ Position = CVector(0.000000,0.000000,0.300000), Scale = CVector(2.000000, 2.000000, 2.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_300", Shape="snow_target_300" },

}

Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(-0.009918, -0.232358, 0.377324), Rotation = CAngleAxis(0.044152, 0.000166, 0.003746, 0.999018) },
}
