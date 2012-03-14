Name = "Snow classic Fight"
Author = "Ace"
ServerLua = "level_classic_fight_server.lua"
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
	CVector(-0.098865, 1.56397, 9.68978),
	CVector(-0.25109, 1.56534, 9.68978),
	CVector(-0.411029, 1.55867, 9.68978),
	CVector(-0.570633, 1.55203, 9.68978),
	CVector(0.0733468, 1.5695, 9.68978),
	CVector(0.225997, 1.56282, 9.68978),
	CVector(0.394393, 1.55616, 9.68978),
	CVector(0.554403, 1.54953, 9.68978),
	CVector(-0.178982, 1.3806, 9.68978),
	CVector(-0.331207, 1.38197, 9.68978),
	CVector(-0.491146, 1.37531, 9.68978),
	CVector(-0.65075, 1.36866, 9.68978),
	CVector(-0.00677051, 1.38614, 9.68978),
	CVector(0.14588, 1.37946, 9.68978),
	CVector(0.314275, 1.3728, 9.68978),
	CVector(0.474286, 1.36616, 9.68978)
}

Modules =
{
	{ Position = CVector(10, -25, 3.5), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1,0,0,0), Lua = "snow_island", Shape="snow_island" },
	{ Position = CVector(6, -14, 2.8), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1,0,0,0), Lua = "snow_island2", Shape="snow_island2" },
	{ Position = CVector(-3.8, -17.0, 2), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1,0,0,0),	Lua = "snow_island3", Shape="snow_island3" },
	{ Position = CVector(-0.1, -15.3, 1), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1,0,0,0),	Lua = "snow_target_50", Shape="snow_target_50" },
	{ Position = CVector(-0.1, -15.35, 2.55), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1,0,0,0), Lua = "snow_target_100", Shape="snow_target_100" },
	{ Position = CVector(-0.1, -15.33, 2.8), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1,0,0,0), Lua = "snow_target_300", Shape="snow_target_300" },
	{ Position = CVector(0, 0, 5), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1,0,0,0), Lua = "snow_ramp", Shape="snow_ramp" }
}


Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(-0.156463, -15.406598, 2.840894), Rotation = CAngleAxis(-0.015813, 0.005262, -0.315709, 0.948710) },
}
