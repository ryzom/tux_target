Name = "Arena"
Author = "Skeet"
ServerLua = "level_arena_server.lua"
ReleaseLevel = 0

skyShapeFileName = "sky.shape"

sunAmbientColor = CRGBA(82, 100, 133, 255);
sunDiffuseColor = CRGBA(255, 255, 255, 255);
sunSpecularColor = CRGBA(255, 255, 255, 255);
sunDirection = CVector(-1,0,-1);

clearColor = CRGBA(30, 45, 90, 0);

fogDistMin = 0;
fogDistMax = 150;
fogColor = clearColor;

cameraMinDistFromStartPointToMove = 10.0;

Cameras =
{
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10),
	CVector(0, 3, 10),
	CVector(0, -3, 10)
}

StartPoints =
{
	CVector(0.092517,-0.235331,0.616442),
	CVector(0.235167,0.187477,0.616442),
	CVector(-0.130000,0.206845,0.616442),
	CVector(0.209573,-0.224778,0.616442),
	CVector(-0.005987,-0.232316,0.616442),
	CVector(0.215419,-0.141065,0.616442),
	CVector(0.226817,-0.031980,0.616442),
	CVector(0.230909,0.061545,0.616442),
	CVector(0.126736,0.199527,0.616442),
	CVector(-0.008141,0.204887,0.616442),
	CVector(-0.210000,0.008517,0.616442),
	CVector(-0.209013,-0.145247,0.616442),
	CVector(-0.092250,-0.225863,0.616442),
	CVector(-0.168845,-0.211155,0.616442),
	CVector(-0.215316,0.101510,0.616442),
	CVector(-0.219723,0.205184,0.616442),

}

Modules =
{
	{ Position = CVector(0.000000,0.000000,0.500000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="arena", Shape="arena" },
	{ Position = CVector(-0.023449,0.000023,1.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(1.000000,0.000000,0.000000,0.000000), Lua="boxae", Shape="boxae" },
	{ Position = CVector(0.476551,0.000000,1.000000), Scale = CVector(1, 1, 1), Rotation = CAngleAxis(0.000000,0.000000,1.000000,3.00000), Lua="boxae", Shape="boxae" },

}


Particles =
{
}

ExternalCameras =
{
}
