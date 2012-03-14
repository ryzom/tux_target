Name = "The Lan"
Author = "Skeet"
ServerLua = "level_default_server.lua"
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
ClientBounceCoef = 1;
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
	{ Position = CVector(0.003322,-15.228691,3.223083), Scale = CVector(30.000000, 1.000000, 30.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(254,254,254,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(0.000899,-15.084286,2.989382), Scale = CVector(30.000000, 30.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,-0.050000), Color = CRGBA(254,254,254,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(0.000000,0.000000,5.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="snow_ramp", Shape="snow_ramp" },
	{ Position = CVector(0.000000,-14.840000,3.000000), Scale = CVector(3.000000, 6.000000, 0.500000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="lane_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,-14.940000,3.000000), Scale = CVector(1.500000, 4.000000, 0.500000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,0,255,255), Lua="lane_target_100", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.000000,3.000000), Scale = CVector(1.500000, 2.000000, 0.500000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,0,0,255), Lua="lane_target_300", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.060000,3.000000), Scale = CVector(1.500000, 4.000000, 0.500000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,0,255,255), Lua="lane_target_100", Shape="box_sol" },
	{ Position = CVector(0.000000,-15.160000,3.000000), Scale = CVector(3.000000, 6.000000, 0.500000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="lane_target_50", Shape="box_sol" },
	{ Position = CVector(-0.040000,-14.840000,3.000589), Scale = CVector(1.000000, 6.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(-0.025000,-14.940000,3.000589), Scale = CVector(1.000000, 4.000000, 6.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(-0.035003,-15.000000,3.000589), Scale = CVector(1.000000, 2.000000, 6.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(-0.025000,-15.060000,3.000589), Scale = CVector(1.000000, 4.000000, 6.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(-0.040000,-15.160000,3.000589), Scale = CVector(1.000000, 6.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(0.040000,-14.840000,3.000589), Scale = CVector(1.000000, 6.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(0.025000,-14.940000,3.000589), Scale = CVector(1.000000, 4.000000, 6.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(0.031365,-15.000000,3.000589), Scale = CVector(1.000000, 2.000000, 6.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(0.025000,-15.060000,3.000589), Scale = CVector(1.000000, 4.000000, 6.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(0.040000,-15.160000,3.000589), Scale = CVector(1.000000, 6.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="box_sol", Shape="box_sol" },

}


Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(0.007927, -15.196621, 3.059711), Rotation = CAngleAxis(-0.102049, -0.007446, 0.072386, 0.992114) },
}
