Name = "Snow team mirror"
Author = "Skeet"
ServerLua = "level_team_server.lua"
ReleaseLevel = 2

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
	CVector(0, -3, 10),
	CVector(0, 3, 10),
}

StartPoints =
{
	CVector(-0.251090,-16.565340,9.689780),
	CVector(-0.411029,16.558670,9.689780),
	CVector(-0.570633,-16.552031,9.689780),
	CVector(0.073347,16.569500,9.689780),
	CVector(0.225997,-16.562820,9.689780),
	CVector(0.394393,16.556160,9.689780),
	CVector(0.554403,-16.549530,9.689780),
	CVector(-0.178982,16.380600,9.689780),
	CVector(-0.331207,-16.381969,9.689780),
	CVector(-0.491146,16.375311,9.689780),
	CVector(-0.650750,-16.368660,9.689780),
	CVector(-0.006771,16.386141,9.689780),
	CVector(0.145880,-16.379459,9.689780),
	CVector(0.314275,16.372801,9.689780),
	CVector(0.474286,-16.366159,9.689780),
	CVector(-0.098865,16.563971,9.689780),

}

Modules =
{
	{ Position = CVector(10.000000,-10.000000,3.500000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="snow_island", Shape="snow_island" },
	{ Position = CVector(6.000000,1.000000,2.800000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="snow_island2", Shape="snow_island2" },
	{ Position = CVector(-3.800000,-2.000000,2.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="snow_island3", Shape="snow_island3" },
	{ Position = CVector(0.400000,-0.393851,3.000000), Scale = CVector(3.000000, 3.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,0,0,255), Lua="team_target_300_red", Shape="box_sol" },
	{ Position = CVector(0.400000,-0.453794,3.000000), Scale = CVector(3.000000, 3.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,0,255,255), Lua="team_target_300_blue", Shape="box_sol" },
	{ Position = CVector(0.400000,-0.304152,3.000000), Scale = CVector(6.000000, 6.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,100,100,255), Lua="team_target_100_red", Shape="box_sol" },
	{ Position = CVector(0.400000,-0.543383,3.000000), Scale = CVector(6.000000, 6.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(100,100,255,255), Lua="team_target_100_blue", Shape="box_sol" },
	{ Position = CVector(0.400000,-0.166551,3.000000), Scale = CVector(8.000000, 8.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,200,200,255), Lua="team_target_50_red", Shape="box_sol" },
	{ Position = CVector(0.400000,-0.681852,3.000000), Scale = CVector(8.000000, 8.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(200,200,255,255), Lua="team_target_50_blue", Shape="box_sol" },
	{ Position = CVector(0.000000,15.000000,5.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="snow_ramp", Shape="snow_ramp" },
	{ Position = CVector(0.000000,-15.000000,5.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,3.141500), Color = CRGBA(255,255,255,255), Lua="snow_ramp", Shape="snow_ramp" },

}


Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(0.737141, -0.458361, 3.263562), Rotation = CAngleAxis(-0.183750, -0.169260, 0.656029, 0.712189) },
}
