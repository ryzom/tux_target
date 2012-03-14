Name = "Snow tube land"
Author = "Ace"
ServerLua = "level_snow_tube_server.lua"
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
	{ Position = CVector(-0.091699,-15.224501,0.990530), Scale = CVector(100.000000, 1.000000, 100.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_box" },
	{ Position = CVector(-0.107331,-15.384774,0.990401), Scale = CVector(100.000000, 1.000000, 100.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_box" },

	{ Position = CVector(-1.0,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(-1.0,-15.300000,1.902271), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_tube" },
	{ Position = CVector(-1.0,-15.300000,1.834467), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_50", Shape="snow_torus" },

	{ Position = CVector(-0.8,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },

	{ Position = CVector(-0.6,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(-0.6,-15.300000,1.902271), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_tube" },
	{ Position = CVector(-0.6,-15.300000,1.834467), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_100", Shape="snow_torus" },

	{ Position = CVector(-0.4,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(-0.4,-15.300000,1.902271), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_tube" },

	{ Position = CVector(-0.2,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(-0.2,-15.300000,1.902271), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_tube" },
	{ Position = CVector(-0.2,-15.300000,1.834467), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_300", Shape="snow_torus" },

	{ Position = CVector(0.0,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(0.0,-15.300000,1.902271), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_tube" },

	{ Position = CVector(0.2,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(0.2,-15.300000,1.902271), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_tube" },
	{ Position = CVector(0.2,-15.300000,1.834467), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_n300", Shape="snow_torus" },

	{ Position = CVector(0.4,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },

	{ Position = CVector(0.6,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(1.0,-15.300000,1.902271), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_tube" },
	{ Position = CVector(1.0,-15.300000,1.834467), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_n100", Shape="snow_torus" },

	{ Position = CVector(0.8,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(1.0,-15.300000,1.902271), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_tube" },
	{ Position = CVector(1.0,-15.300000,1.834467), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_50", Shape="snow_torus" },

	{ Position = CVector(1.0,-15.300000,2.000812), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(1.0,-15.300000,1.902271), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_neutral", Shape="snow_tube" },
	{ Position = CVector(1.0,-15.300000,1.834467), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_target_50", Shape="snow_torus" },
}

Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(0.281585, -15.540110, 2.569168), Rotation = CAngleAxis(-0.312678, -0.198093, 0.497164, 0.784742) },
}
