Name = "Snow funnel"
Author = "Ace"
ServerLua = "level_snow_dual_ramp_server.lua"
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
	CVector(0, 3, 10),
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
	{ Position = CVector(0.000000,0.000000,5.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_ramp", Shape="snow_ramp" },
	{ Position = CVector(0.000000,-20.000000,3.000000), Scale = CVector(10.000000, 500.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_box" },
	{ Position = CVector(-0.100000,-30.299999,-0.573801), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_50", Shape="snow_target_50" },
	{ Position = CVector(-0.100000,-30.350000,1.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_100", Shape="snow_target_100" },
	{ Position = CVector(-0.100000,-30.330000,1.172893), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_300", Shape="snow_target_300" },
}

Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(-0.020185, -30.473835, 1.236753), Rotation = CAngleAxis(-0.099024, -0.022244, 0.218036, 0.970649) },
}
