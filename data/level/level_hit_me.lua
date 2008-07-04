Name = "Hit me"
ReleaseLevel = 0

CameraMinDistFromStartPointToMove = 0.01;

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
	CVector(3.049544,-14.924075,3.088972),
	CVector(-0.251090,1.565340,9.689780),
	CVector(3.055463,-14.896098,3.088972),
	CVector(-0.570633,1.552030,9.689780),
	CVector(2.964535,-14.952650,3.088972),
	CVector(0.225997,1.562820,9.689780),
	CVector(3.006582,-14.952650,3.088972),
	CVector(0.554403,1.549530,9.689780),
	CVector(2.962006,-14.901152,3.088972),
	CVector(-0.331207,1.381970,9.689780),
	CVector(3.053153,-14.952650,3.088972),
	CVector(-0.650750,1.368660,9.689780),
	CVector(3.006771,-14.898813,3.088972),
	CVector(0.145880,1.379460,9.689780),
	CVector(3.006771,-14.923434,3.088972),
	CVector(0.474286,1.366160,9.689780),

}

Modules =
{
	{ Position = CVector(10,-25,3.5), Shape="snow_island" },
	{ Position = CVector(6,-14,2.8), Shape="snow_island2" },
	{ Position = CVector(-3.8,-17,2), Shape="snow_island3" },
	{ Position = CVector(1.216417,-14.929698,2.956648), Scale = CVector(300, 4, 10), Lua="hit_me_lane", Shape="snow_box" },
	{ Position = CVector(1.397697,-14.930597,2.805407), Scale = CVector(300, 20, 10), Lua="dont_go_too_far_target_100", Shape="snow_box" },
	{ Position = CVector(0,0,5), Shape="snow_ramp" },

}

ExternalCameras =
{
}
