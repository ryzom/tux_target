Name = "Team All on one"
Author = "Skeet"
ServerLua = "level_team_all_on_one_server.lua"
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

ClientBounce = 1;
ClientBounceCoef = 0;
ClientBounceVel = 1;

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
	CVector(-0.098865,1.563970,9.689780),
	CVector(-0.251090,1.565340,9.689780),
	CVector(-0.411029,1.558670,9.689780),
	CVector(-0.570633,1.552030,9.689780),
	CVector(0.073347,1.569500,9.689780),
	CVector(0.225997,1.562820,9.689780),
	CVector(0.394393,1.556160,9.689780),
	CVector(0.554403,1.549530,9.689780),
	CVector(-0.178982,1.380600,9.689780),
	CVector(-0.331207,1.381970,9.689780),
	CVector(-0.491146,1.375310,9.689780),
	CVector(-0.650750,1.368660,9.689780),
	CVector(-0.006771,1.386140,9.689780),
	CVector(0.145880,1.379460,9.689780),
	CVector(0.314275,1.372800,9.689780),
	CVector(0.474286,1.366160,9.689780),

}

Modules =
{
	{ Position = CVector(-0.105469,0.000000,5.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="snow_ramp", Shape="snow_ramp" },
	{ Position = CVector(10, -25, 3.5), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1,0,0,0), Lua = "snow_island", Shape="snow_island" },
	{ Position = CVector(6, -14, 2.8), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1,0,0,0), Lua = "snow_island2", Shape="snow_island2" },
	{ Position = CVector(-3.8, -17.0, 2), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1,0,0,0),	Lua = "snow_island3", Shape="snow_island3" },
	{ Position = CVector(-0.1, -15.33, 2.8), Scale = CVector(6, 6, 1), Rotation = CAngleAxis(1,0,0,0), Color = CRGBA(100,255,100,255), Lua = "snow_target_100_flat", Shape="box_sol" },
}


Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(-0.114005, -15.741055, 2.953115), Rotation = CAngleAxis(0.048297, -0.001298, -0.026842, 0.998471) },
}
