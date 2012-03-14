Name = "Wood"
Author = "Wedgee and Skeet"
ServerLua = "level_wood_server.lua"
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
	CVector(-0.290483,1.881427,3.201761),
	CVector(0.161550,1.881427,3.201761),
	CVector(0.245382,1.881427,3.201761),
	CVector(0.350083,1.881427,3.201761),
	CVector(0.463934,1.881427,3.201761),
	CVector(0.583921,1.881427,3.201761),
	CVector(0.698803,1.881427,3.201761),
	CVector(0.008052,1.881427,3.201761),
	CVector(0.086501,1.881427,3.201761),
	CVector(-0.076241,1.881427,3.201761),
	CVector(-0.683568,1.881427,3.201761),
	CVector(-0.177508,1.881427,3.201761),
	CVector(-0.383970,1.881427,3.201761),
	CVector(-0.479768,1.881427,3.201761),
	CVector(-0.579851,1.881427,3.201761),
	CVector(-0.772570,1.855249,3.201761),

}

Modules =
{
	{ Position = CVector(0.000000,0.000000,2.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,255,255,255), Lua="wood_ramp", Shape="wood_ramp" },
	{ Position = CVector(0.027931,-7.416007,1.384758), Scale = CVector(0.500000, 0.500000, 0.600000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,0,0,255), Lua="default_target_300", Shape="wood_target" },
	{ Position = CVector(-0.025300,-7.363374,1.326524), Scale = CVector(1.200000, 1.200000, 1.200000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,0,255,255), Lua="default_target_100", Shape="wood_target" },
	{ Position = CVector(0.000000,-7.577335,1.435483), Scale = CVector(2.000000, 2.000000, 2.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="default_target_50", Shape="wood_target" },
	{ Position = CVector(0.015634,-7.454659,1.413185), Scale = CVector(0.500000, 0.500000, 0.600000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,0,0,255), Lua="default_target_300", Shape="wood_target" },
	{ Position = CVector(-0.065166,-7.439917,1.388419), Scale = CVector(1.200000, 1.200000, 1.200000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,0,255,255), Lua="default_target_100", Shape="wood_target" },
	{ Position = CVector(0.106889,-7.498086,1.243082), Scale = CVector(2.000000, 2.000000, 2.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,255,0,255), Lua="default_target_50", Shape="wood_target" },
	{ Position = CVector(0.054127,-7.444132,1.395109), Scale = CVector(0.500000, 0.500000, 0.600000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(255,0,0,255), Lua="default_target_300", Shape="wood_target" },
	{ Position = CVector(0.075974,-7.358481,1.344014), Scale = CVector(1.200000, 1.200000, 1.200000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Color = CRGBA(0,0,255,255), Lua="default_target_100", Shape="wood_target" },

}


Particles =
{
}

ExternalCameras =
{
	{ Position = CVector(0.063327, -7.449109, 1.503763), Rotation = CAngleAxis(-0.469483, -0.313699, 0.458532, 0.686241) },
}
