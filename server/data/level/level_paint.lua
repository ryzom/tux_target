Name = "Paint"
Author = "Skeet"
ServerLua = "level_paint_server.lua"
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
	{ Position = CVector(0.000000,0.000000,5.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,55,55,255), Lua="snow_ramp", Shape="snow_ramp" },

	{ Position = CVector(0.000000,-15.00,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.040000,-15.00,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.080000,-15.00,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.120000,-15.00,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },

	{ Position = CVector(0.000000,-15.04,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.040000,-15.04,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.080000,-15.04,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.120000,-15.04,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },

	{ Position = CVector(0.000000,-15.08,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.040000,-15.08,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.080000,-15.08,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.120000,-15.08,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },

	{ Position = CVector(0.000000,-15.12,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.040000,-15.12,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.080000,-15.12,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
	{ Position = CVector(0.120000,-15.12,2), Scale = CVector(2.000000, 2.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="paint_bloc", Shape="box_sol" },
}


Particles =
{
--	{ Position = CVector(0.000000,0.000000,0.790971), Scale = CVector(10.000000, 30.000000, 10.000000), Rotation = CAngleAxis(0.000000,0.000000,1.000000,0.000000), Show = 1, Started = 1, Name = "toto", FileName = "logo" },
}

ExternalCameras =
{
	{ Position = CVector(-0.025223, -15.367354, 2.191720), Rotation = CAngleAxis(-0.126061, 0.008206, -0.064434, 0.989894) },
}
