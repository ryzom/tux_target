Name = "Snow funnel"
ReleaseLevel = 0

Cameras =
{
	CVector(0, 1, 10),
}

StartPoints =
{
	CVector(0.50000,0,3.1),
	CVector(-0.50000,0,3.1),
	CVector(0.50000,0,3.1),
	CVector(-0.50000,0,3.1),
	CVector(0.25000,0.25000,3.1),
	CVector(-0.25000,0.25000,3.1),
	CVector(0.25000,-0.25000,3.1),
	CVector(-0.25000,-0.25000,3.1),
	CVector(0.50000,0,3.200001),
	CVector(-0.50000,0,3.200001),
	CVector(0.50000,0,3.200001),
	CVector(-0.50000,0,3.200001),
	CVector(0.25000,0.25000,3.200001),
	CVector(-0.25000,0.25000,3.200001),
	CVector(0.25000,-0.25000,3.200001),
	CVector(-0.25000,-0.25000,3.200001),
}

Modules =
{
	{ Position = CVector(0,0,3), Scale = CVector(10, 10, 1), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(0,0,2.950859), Scale = CVector(3, 3, 0.3), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(0,0,2.938401), Scale = CVector(1, 1, 0.1), Lua="snow_neutral", Shape="snow_funnel" },
	{ Position = CVector(0,0,2.926), Scale = CVector(1, 1, 1), Lua="snow_target_300", Shape="snow_torus" },

}

ExternalCameras =
{
}
