Name = "Run Away"
Author = "Skeet"
ServerLua = "level_run_away_server.lua"
ReleaseLevel = 0

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
	CVector(3, 0, 10),
	CVector(0, 3, 10),
	CVector(3, 0, 10),
	CVector(0, 3, 10),
	CVector(3, 0, 10),
	CVector(0, 3, 10),
	CVector(3, 0, 10),
	CVector(0, 3, 10),
	CVector(3, 0, 10),
	CVector(0, 3, 10),
	CVector(3, 0, 10),
	CVector(0, 3, 10),
	CVector(3, 0, 10),
	CVector(0, 3, 10),
	CVector(3, 0, 10),
	CVector(0, 3, 10)
}

StartPoints =
{
	CVector(0.960272,-14.939082,2.916361),
	CVector(1.460010,-14.951203,2.916361),
	CVector(1.305305,-15.185359,2.916361),
	CVector(1.047855,-15.087206,2.916361),
	CVector(1.197958,-15.190729,2.916361),
	CVector(1.451706,-15.032510,2.916361),
	CVector(1.457329,-14.846673,2.916361),
	CVector(0.989449,-15.027626,2.916361),
	CVector(1.330427,-14.706165,2.916361),
	CVector(1.138438,-14.694311,2.916361),
	CVector(1.002680,-14.826438,2.916361),
	CVector(1.246121,-14.697505,2.916361),
	CVector(1.406534,-15.134086,2.916361),
	CVector(1.399452,-14.761731,2.916361),
	CVector(1.055915,-14.760860,2.916361),
	CVector(1.103288,-15.170325,2.929304),

}

Modules =
{
	{ Position = CVector(1.230400,-14.914321,2.886570), Scale = CVector(1.000000, 1.000000, 0.200000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="run_away_bowl", Shape="snow_bowl" },
	{ Position = CVector(0.000000,0.000000,5.000000), Scale = CVector(1.000000, 1.000000, 1.000000), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="snow_ramp", Shape="snow_ramp" },

}


Particles =
{
}

ExternalCameras =
{
}
