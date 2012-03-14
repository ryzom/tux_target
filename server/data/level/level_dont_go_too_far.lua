Name = "Dont go too far"
Author = "Skeet"
ServerLua = "level_dont_go_too_far_server.lua"
ReleaseLevel = 1

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
	{ Position = CVector(10.000000,-25.000000,3.500000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_island", Shape="snow_island" },
	{ Position = CVector(6.000000,-14.000000,2.800000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_island2", Shape="snow_island2" },
	{ Position = CVector(-3.800000,-17.000000,2.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_island3", Shape="snow_island3" },
	{ Position = CVector(-0.100000,-14.912012,3.037934), Scale = CVector(2.000000, 10.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="dont_go_too_far_lane", Shape="box_sol" },
	{ Position = CVector(-0.097495,-15.040000,3.020000), Scale = CVector(2.000000, 2.000000, 0.500000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="dont_go_too_far_target_300", Shape="box_sol" },
	{ Position = CVector(-0.098685,-15.060000,3.010000), Scale = CVector(4.000000, 4.000000, 0.500000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="dont_go_too_far_target_100", Shape="box_sol" },
	{ Position = CVector(-0.100000,-15.080000,3.000000), Scale = CVector(6.000000, 6.000000, 0.500000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="dont_go_too_far_target_50", Shape="box_sol" },
	{ Position = CVector(0.000000,0.000000,5.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_ramp", Shape="snow_ramp" },
	{ Position = CVector(-0.174782,-15.078223,3.061672), Scale = CVector(1.000000, 6.000000, 6.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(-0.098757,-15.011895,3.024662), Scale = CVector(6.000000, 0.500000, 2.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(-0.100000,-15.148162,3.060950), Scale = CVector(6.000000, 1.000000, 6.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(-0.026166,-15.073698,3.064236), Scale = CVector(1.000000, 6.000000, 6.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="box_sol", Shape="box_sol" },
	{ Position = CVector(-0.100000,-15.081491,3.102471), Scale = CVector(6.000000, 6.000000, 2.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="box_sol", Shape="box_sol" },

}


Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(-0.144745, -15.145563, 3.033414), Rotation = CAngleAxis(0.167747, -0.027609, -0.160039, 0.972361) },
}
