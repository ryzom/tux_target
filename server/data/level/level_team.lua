Name = "Snow team"
Author = "Skeet"
ServerLua = "level_team_server.lua"
ReleaseLevel = 1

skyShapeFileName = "sky.shape"

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
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10)
}

StartPoints =
{
	CVector(-0.098865, 16.56397, 9.68978),
	CVector(-0.25109,  -16.56534, 9.68978),
	CVector(-0.411029, 16.55867, 9.68978),
	CVector(-0.570633, -16.55203, 9.68978),
	CVector(0.0733468, 16.5695, 9.68978),
	CVector(0.225997,  -16.56282, 9.68978),
	CVector(0.394393,  16.55616, 9.68978),
	CVector(0.554403,  -16.54953, 9.68978),
	CVector(-0.178982, 16.3806, 9.68978),
	CVector(-0.331207, -16.38197, 9.68978),
	CVector(-0.491146, 16.37531, 9.68978),
	CVector(-0.65075,  -16.36866, 9.68978),
	CVector(-0.00677051, 16.38614, 9.68978),
	CVector(0.14588,  -16.37946, 9.68978),
	CVector(0.314275, 16.3728, 9.68978),
	CVector(0.474286, -16.36616, 9.68978)
}

Modules =
{
	{ Position = CVector(10.000000,-10.000000,3.500000), Scale = CVector(1, 1, 1) ,Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_island", Shape="snow_island" },
	{ Position = CVector(6.000000,1.000000,2.800000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_island2", Shape="snow_island2" },
	{ Position = CVector(-3.800000,-2.000000,2.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_island3", Shape="snow_island3" },
	{ Position = CVector(0.398453,-0.032595,3.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="team_target_50_red", Shape="team_target_50_red" },
	{ Position = CVector(0.400000,-0.257311,3.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="team_target_100_red", Shape="team_target_100_red" },
	{ Position = CVector(0.400843,-0.369995,3.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="team_target_300_red", Shape="team_target_300_red" },
	{ Position = CVector(0.400000,-0.782447,3.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="team_target_50_blue", Shape="team_target_50_blue" },
	{ Position = CVector(0.400000,-0.558576,3.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="team_target_100_blue", Shape="team_target_100_blue" },
	{ Position = CVector(0.400000,-0.445013,3.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="team_target_300_blue", Shape="team_target_300_blue" },
	{ Position = CVector(0.000000,15.000000,5.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_ramp", Shape="snow_ramp" },
	{ Position = CVector(0.000000,-15.000000,5.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(0.000000,0.000000,1.000000,3.141500), Lua="snow_ramp", Shape="snow_ramp" }
}


Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(0.737141, -0.458361, 3.263562), Rotation = CAngleAxis(-0.183750, -0.169260, 0.656029, 0.712189) },
}
