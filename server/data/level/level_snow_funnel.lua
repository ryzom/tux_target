Name = "Snow funnel"
Author = "Ace"
ServerLua = "level_snow_funnel_server.lua"
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

Cameras =
{
	CVector(0, 1, 10),
}

StartPoints =
{
	CVector(0.50000,0.000000,3.100000),
	CVector(-0.50000,0.000000,3.100000),
	CVector(0.50000,0.000000,3.100000),
	CVector(-0.50000,0.000000,3.100000),
	CVector(0.25000,0.25000,3.100000),
	CVector(-0.25000,0.25000,3.100000),
	CVector(0.25000,-0.25000,3.100000),
	CVector(-0.25000,-0.25000,3.100000),
	CVector(0.50000,0.000000,3.200001),
	CVector(-0.50000,0.000000,3.200001),
	CVector(0.50000,0.000000,3.200001),
	CVector(-0.50000,0.000000,3.200001),
	CVector(0.25000,0.25000,3.200001),
	CVector(-0.25000,0.25000,3.200001),
	CVector(0.25000,-0.25000,3.200001),
	CVector(-0.25000,-0.25000,3.200001),
}

Modules =
{
	{ Position = CVector(0.000000,0.000000,3.000000), Scale = CVector(10.000000, 10.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(0.000000,0.000000,2.950859), Scale = CVector(3.000000, 3.000000, 0.300000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(0.000000,0.000000,2.938401), Scale = CVector(1.000000, 1.000000, 0.100000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(0.000000,0.000000,2.926), Scale = CVector(1.000000, 1.000000, 1.00000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_300", Shape="snow_torus" },

}
Particles =
{
}

ExternalCameras =
{
}
