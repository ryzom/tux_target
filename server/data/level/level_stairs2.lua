Name = "Stairs 2"
Author = "Skeet"
ServerLua = "level_stairs_server.lua"
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
	{ Position = CVector(0.000000,-15.301329,2.800000), Scale = CVector(2.000000, 33.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.271274,2.820000), Scale = CVector(2.000000, 30.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.241201,2.840000), Scale = CVector(2.000000, 27.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.210726,2.860000), Scale = CVector(2.000000, 24.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.181026,2.880000), Scale = CVector(2.000000, 21.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.151276,2.900000), Scale = CVector(2.000000, 18.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.120431,2.920000), Scale = CVector(2.000000, 15.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.089981,2.940000), Scale = CVector(2.000000, 12.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.059729,2.960000), Scale = CVector(2.000000, 9.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.030267,2.980000), Scale = CVector(2.000000, 6.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.000000,3.000000), Scale = CVector(2.000000, 3.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="stairs_target_50", Shape="box_sol" },
	{ Position = CVector(-0.000000,-14.960159,2.810742), Scale = CVector(2.000000, 1.000000, 20.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="stairs_target_0", Shape="box_sol" },

}


Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(0.235616, -15.587805, 3.067564), Rotation = CAngleAxis(-0.208756, -0.093624, 0.398360, 0.888237) },
}
