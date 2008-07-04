// This file is part of Mtp Target.
// Copyright (C) 2008 Vialek
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// 
// Vianney Lecroart - gpl@vialek.com


//
// Includes
//

#include "stdpch.h"

#include <nel/misc/quat.h>
#include <nel/misc/common.h>

#include <nel/3d/u_instance_material.h>
#include <nel/3d/u_visual_collision_manager.h>

#include "level.h"
#include "entity.h"
#include "3d_task.h"
#include "hud_task.h"
#include "time_task.h"
#include "mtp_target.h"
#include "font_manager.h"
#include "level_manager.h"
#include "entity_manager.h"
#include "config_file_task.h"
#include "../../common/constant.h"
#include "../../common/async_job.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Functions
//

CEntity::CEntity() : Ping(20)
{
	Type = Unknown;
	Id = 255;
	ArrivalTime = 0.0f;
	TraceParticleOpen = 0;
	TraceParticleClose = 0;
	ParticuleOpenActivated = false;
	ParticuleCloseActivated = false;
	ImpactParticle = 0;
//	Rank = 255;
	StartPointId = 255;
//	LastSent2MePos = CVector::Null;
//	LastSent2OthersPos = CVector::Null;
	Team = 255;
	Interpolator = 0;
	Ready = false;
	showCollideWhenFly = false;
	Collided = false;
	showCollideWhenFlyPos = CVector::Null;
	addChatLine.clear();
	addOpenCloseKey = false;
	addCrashEventKey = CCrashEvent(false, CVector::Null);

	OriginalColor = CRGBA::White;
	FadeOpenParticleColor = CRGBA::White;
	FadeOpenParticleStartColor = CRGBA::White;
	FadeOpenParticleDuration = 1.0f;
	FadeOpenParticleStartTime = 0.0f;

	FadeCloseParticleColor = CRGBA::White;
	FadeCloseParticleStartColor = CRGBA::White;
	FadeCloseParticleDuration = 1.0f;
	FadeCloseParticleStartTime = 0.0f;

	Color = CRGBA::White;

	CurrentScore = 0;
	LastGameScore = 0;
	TotalScore = 0;

	OpenClose = false;
	Spectator = true;

	FullVersion = false;

	CollidedWithWater = false;
	LuaUserDataRef = 0;
	LuaUserData = 0;
}

//DEBUG
double sentCTRL = 0.0;

void CEntity::swapOpenClose(bool ps)
{
	OpenClose = !OpenClose;

	if(isLocal())
	{
		double now = CTimeTask::instance().time();
		nlwarning("CTRL: PRESSED, msg received %s %f (%f)", (OpenClose?"open":"close"), now, (now-sentCTRL));
	}

	if (OpenClose)
	{
		ParticuleOpenActivated = 1;
		ParticuleCloseActivated = 0;
		if(!CloseMesh.empty())
			CloseMesh.hide();
		if(!OpenMesh.empty())
			OpenMesh.show();
		if(ps) playSound(CSoundManager::BallOpen);
	}
	else
	{
		ParticuleOpenActivated = 0;
		ParticuleCloseActivated = 1;
		if(!CloseMesh.empty())
			CloseMesh.show();
		if(!OpenMesh.empty())
			OpenMesh.hide();
		if(ps) playSound(CSoundManager::BallClose);
	}

	if(!OpenClose)
	{
		ObjMatrix.rotateX(1.0f);
		ObjMatrix.rotateY(1.0f);
	}
//	nlinfo("%s is now %s",name().toUtf8().c_str(),OpenClose?"open":"close");
}

CInterpolator &CEntity::interpolator() const
{
	nlassert(Interpolator);
	return *Interpolator;
}

void CEntity::close()
{
	OpenClose = false;

	ParticuleOpenActivated = 0;
	ParticuleCloseActivated = 0;
	if(!CloseMesh.empty())
		CloseMesh.show();
	if(!OpenMesh.empty())
		OpenMesh.hide();
}

void CEntity::update()
{
	if(!spectator()) interpolator().update();

	if(interpolator().openCloseEvent())
		swapOpenClose();

	CCrashEvent ce = interpolator().crashEvent();
	if(ce.Crash)
	{
		collideWhenFly(ce.Position);
	}

//	nlinfo("set matrix for %hu", (uint16)id());

//	if(CMtpTarget::instance().controler().getControledEntity() != id()) nlinfo("%s %f %f %f", name().toUtf8().c_str(), interpolator().matrix().getPos().x, interpolator().matrix().getPos().y, interpolator().matrix().getPos().z);

	if(!OpenMesh.empty())
	{
		CMatrix m2;
		m2.identity();
		m2.setScale(CVector(2*GScale, 2*GScale, 2*GScale));
		CMatrix m3;
		m3.setMulMatrix(interpolator().matrix(), m2);
		OpenMesh.setMatrix(m3);
	}

	if(!CloseMesh.empty())
	{
		CMatrix m2;
		m2.identity();
		m2.setScale(CVector(2*GScale, 2*GScale, 2*GScale));
		CMatrix m3;
		m3.setMulMatrix(interpolator().matrix(), m2);
		CloseMesh.setMatrix(m3);
	}

//	SoundsDescriptor.update3d(interpolator().getMatrix().getPos(), CVector(0,0,0)); // todo : velocity
#ifdef USE_FMOD
	for(list<FMOD::Channel*>::iterator it = Channels.begin(); it != Channels.end();)
	{
		FMOD_VECTOR p;
		// fmod is left handed
		p.x = interpolator().matrix().getPos().x/GScale;
		p.y = interpolator().matrix().getPos().y/GScale;
		p.z = -interpolator().matrix().getPos().z/GScale;
		(*it)->set3DAttributes(&p, 0);

		bool playing;
		(*it)->isPlaying(&playing);
		if(!playing)
		{
			list<FMOD::Channel*>::iterator cur = it++;
			Channels.erase(cur);
		}
		else
		{
			it++;
		}
	}
#endif

	if(!ImpactParticle.empty())
	{
		ImpactParticle.setPos(showCollideWhenFlyPos);
	}

	if(showCollideWhenFly)
	{
		if(isLocal())
		{
			playSound(CSoundManager::Impact);
			CHudTask::instance().addMessage(CHudMessage(-2.0f,5,1,CRGBA(178,17,0),5, CI18N::get("DontTouch"), UTextContext::MiddleBottom));
		}
		if(!ImpactParticle.empty())
		{
			//ImpactParticle.setPos(showCollideWhenFlyPos);
			ImpactParticle.activateEmitters(true);
			ImpactParticle.show();
		}
		//ImpactParticle.setPos(interpolator().position());
		//ImpactParticle.setScale(100,100,100);
		showCollideWhenFly = false;
	}

	if(!TraceParticleOpen.empty())
	{
		TraceParticleOpen.setPos(interpolator().position());
		if (ParticuleOpenActivated != -1 && TraceParticleOpen.isSystemPresent())
		{
			if(ParticuleOpenActivated==1)
				fadeOpenParticleColorTo(CRGBA(255,255,255,255),1);
			else
				fadeOpenParticleColorTo(CRGBA(0,0,0,0),1);

			ParticuleOpenActivated = -1;
		}
	}

	if(!TraceParticleClose.empty())
	{
		TraceParticleClose.setPos(interpolator().position());
		if (ParticuleCloseActivated != -1 && TraceParticleClose.isSystemPresent())
		{
			if(ParticuleCloseActivated==1)
				fadeCloseParticleColorTo(CRGBA(255,255,255,255),1);
			else
				fadeCloseParticleColorTo(CRGBA(0,0,0,0),1);

			ParticuleCloseActivated = -1;
		}
	}

	// we activate

	fadeOpenParticleColorUpdate();
	fadeCloseParticleColorUpdate();
}

void CEntity::collideWhenWater()
{
	if(CollidedWithWater)
		return;

	CollidedWithWater = true;
	Collided = true;

	// launch the splash sound
	playSound(CSoundManager::Splash);

/*
	CWaterHeightMap &whm = GetWaterPoolManager().getPoolByID(0);
	const float waterRatio = whm.getUnitSize();
	const float invWaterRatio = 1.0f / waterRatio;
	sint px = (sint) (_position.x * invWaterRatio);
	sint py = (sint) (_position.y * invWaterRatio);
	whm.perturbate(px, py, 2, 2.0f);
*/

	close ();
}

bool CEntity::namePosOnScreen(CVector &res)
{
	if(CMtpTarget::instance().controler().getControledEntity() != id() || !ReplayFile.empty())
	{
		CVector p = interpolator().position();
		CVector dpos = p - CMtpTarget::instance().controler().Camera.getMatrix()->getPos();
		CMatrix cameraMatrix = C3DTask::instance().scene().getCam().getMatrix();
		if(dpos.norm()>1) return false;

		CVector vv = cameraMatrix.getPos() - p;

		cameraMatrix.invert();

		p = cameraMatrix * p;

		// the text is behind us, don't display it
		if (p.y < 0.0f)
			return false;

		p = C3DTask::instance().scene().getCam().getFrustum().project(p);
		res = p;
		return 0<p.x && p.x<1 && 0<p.y && p.y<1;
	}
	return false;
}

void CEntity::renderName(bool all) const
{
	if(CConfigFileTask::instance().configFile().getVar("NoHUD").asInt()==1) return;

	// display name of other people than me
	if(CMtpTarget::instance().controler().getControledEntity() != id() || !ReplayFile.empty())
	{
		const CVector &pos = interpolator().position();
		CVector camPos;
		if(CMtpTarget::instance().spectator())
			camPos = ControlerFreeLookPos;
		else
			camPos = CMtpTarget::instance().controler().Camera.getMatrix()->getPos();
		CVector dpos = pos - camPos;
		float norm = dpos.norm();
		if(norm < 1.0f || OpenClose || all)
		{
			float alpha = max((norm-0.5f)*2.0f, 0.0f);
			alpha = 255.0f - alpha * 255.0f;
			if(OpenClose || all) alpha = 255.0f;
			CRGBA col = color();
			col.A = (uint8)alpha;
			CFontManager::instance().print3D(CFontManager::TCPLayerName, col, pos, name());
		}
	}
}

void CEntity::id(uint8 nid)
{
	nlassert(!Interpolator);
	Interpolator = new CInterpolator();
	Id = nid;
}

void CEntity::reset()
{
	//nlinfo(">> 0x%p::CEntity::reset() (eid %u)",this, Id);

	if(Interpolator) delete Interpolator;
	Interpolator = new CInterpolator();
	interpolator().setEntity(this);

	if(!TraceParticleOpen.empty())
	{
		C3DTask::instance().scene().deleteInstance(TraceParticleOpen);
	}
	if(!TraceParticleClose.empty())
	{
		C3DTask::instance().scene().deleteInstance(TraceParticleClose);
	}

	if(!ImpactParticle.empty())
	{
		C3DTask::instance().scene().deleteInstance(ImpactParticle);
	}

	if(!CloseMesh.empty())
	{
		//nlinfo(">>   C3DTask::instance().scene().deleteInstance(CloseMesh);");
		CloseMesh.hide();
		//C3DTask::instance().scene().deleteInstance(CloseMesh);
	}

	if(!OpenMesh.empty())
	{
		OpenMesh.hide();
		//C3DTask::instance().scene().deleteInstance(OpenMesh);
	}

//	CSoundManager::instance().unregisterEntity(SoundsDescriptor);

	Type = Unknown;
	Name.clear();
	Color.set(255,255,255);
	Texture.clear();
	Trace.clear();
	CurrentScore = 0;
	LastGameScore = 0;
	TotalScore = 0;
	OpenClose = false;
	ParticuleOpenActivated = 0;
	ParticuleCloseActivated = 1;
	ObjMatrix.identity();
	ObjMatrix.rotateX(1.0f);
	ObjMatrix.rotateY(1.0f);
	Ready = false;
	StartPointId = 255;
	showCollideWhenFly = false;
	Collided = false;
	showCollideWhenFlyPos = CVector(0,0,0);
	if(!ImpactParticle.empty())
	{
		ImpactParticle.activateEmitters(false);
		ImpactParticle.hide();
	}
	CollidedWithWater = false;
}

void CEntity::sessionStart()
{
	//setColor(OriginalColor);
}

void CEntity::sessionReset()
{
	if(ReplayFile.empty())
		interpolator().reset();
	OpenClose = false;
 	if(!TraceParticleOpen.empty())
 		TraceParticleOpen.setUserColor(CRGBA(0,0,0,0));
	if(!TraceParticleClose.empty())
		TraceParticleClose.setUserColor(CRGBA(0,0,0,0));
	ParticuleOpenActivated = 0;
	ParticuleCloseActivated = 1;
	showCollideWhenFly = false;
	Collided = false;
	showCollideWhenFlyPos = CVector(0,0,0);
	if(!ImpactParticle.empty())
	{
		ImpactParticle.activateEmitters(false);
		ImpactParticle.hide();
	}
	ArrivalTime = 0.0f;
	CollidedWithWater = false;
	setCurrentScore(0);
}

void CEntity::init(TEntity type, const ucstring &name, sint32 totalScore, CRGBA &color, const string &texture, const string &meshname, bool spectator, bool isLocal, const string &trace, bool fullversion)
{
	nlassert(type != Unknown);

	Type = type;
	Name = name;
	OriginalColor = color;
	Color = color;
	Trace = trace;
	MeshName = meshname;
	Spectator = spectator;
	Ready = false;
	interpolator().reset();
	setIsLocal(isLocal);
	this->setTotalScore(totalScore);
	FullVersion = fullversion;

	//nlinfo("CEntity::init() , texture=%s",Texture.c_str());
//	CSoundManager::instance().registerEntity(SoundsDescriptor);

	load3d();
	this->setTexture(texture);
}

void CEntity::load3d()
{
//	nlinfo(">> 0x%p::CEntity::load3d()",this);

	if(MeshName.empty())
		MeshName = "pingoo";

	if(CloseMesh.empty())
	{
		string res = CPath::lookup("entity_"+MeshName+"_close.shape");
		CloseMesh = C3DTask::instance().scene().createInstance(res);
	}
	for(uint i = 0; i < CloseMesh.getNumMaterials(); i++)
	{
		CloseMesh.getMaterial(i).setDiffuse(Color);
		CloseMesh.getMaterial(i).setAmbient(Color);
	}
	if(OpenClose)
		CloseMesh.hide();
	else
		CloseMesh.show();
	CloseMesh.setTransformMode (UTransformable::DirectMatrix);

	if(OpenMesh.empty())
	{
		string res = CPath::lookup("entity_"+MeshName+"_open.shape");
		OpenMesh = C3DTask::instance().scene().createInstance(res);
	}
	for(uint i = 0; i < OpenMesh.getNumMaterials(); i++)
	{
		OpenMesh.getMaterial(i).setDiffuse(Color);
		OpenMesh.getMaterial(i).setAmbient(Color);
	}
	if(OpenClose)
		OpenMesh.show();
	else
		OpenMesh.hide();
	OpenMesh.setTransformMode (UTransformable::DirectMatrix);


	if(CConfigFileTask::instance().configFile().getVar("DisplayParticle").asInt() == 1)
	{
		string TraceFilename = "trace";
		if(!Trace.empty())
			TraceFilename = Trace;

		if(TraceParticleOpen.empty())
		{
			string res = CPath::lookup(TraceFilename+"_open.ps");
			TraceParticleOpen.cast(C3DTask::instance().scene().createInstance(res));
			TraceParticleOpen.setTransformMode (UTransformable::RotQuat);
			TraceParticleOpen.setOrderingLayer(2);
			TraceParticleOpen.activateEmitters(true);
			TraceParticleOpen.show();
			TraceParticleOpen.setUserColor(CRGBA(0,0,0));
		}
		if(TraceParticleClose.empty())
		{
			string res = CPath::lookup(TraceFilename+"_close.ps");
			TraceParticleClose.cast(C3DTask::instance().scene().createInstance(res));
			TraceParticleClose.setTransformMode (UTransformable::RotQuat);
			TraceParticleClose.setOrderingLayer(2);
			TraceParticleClose.activateEmitters(true);
			TraceParticleClose.show();
			TraceParticleClose.setUserColor(CRGBA(0,0,0,0));
		}
		ParticuleOpenActivated = OpenClose ? 1:0;
		ParticuleCloseActivated = OpenClose ? 0:1;
	}

	if(ImpactParticle.empty() && CConfigFileTask::instance().configFile().getVar("DisplayParticle").asInt() == 1)
	{
		string res = CPath::lookup("impact.ps");
		ImpactParticle.cast(C3DTask::instance().scene().createInstance(res));
		ImpactParticle.setTransformMode (UTransformable::RotQuat);
		ImpactParticle.setOrderingLayer(2);
		ImpactParticle.activateEmitters(false);
		ImpactParticle.hide();
		//ImpactParticle.setScale(1,1,1);
	}

	if(Shadow)
	{
		CloseMesh.enableCastShadowMap(true);
		CloseMesh.setShadowMapDirectionZThreshold(-0.86f);
		CloseMesh.setShadowMapMaxDepth(0.5f);
		NL3D::UVisualCollisionMesh colMesh;
		CloseMesh.getShape().getVisualCollisionMesh(colMesh);
		// if this mesh has a collision
		if(!colMesh.empty())
		{
			// get the instance matrix
			const CMatrix &mat = CloseMesh.getMatrix();

			// then send the result to the collision manager, and keep the mesh col id if succeed
			uint32     meshId = C3DTask::instance().collisionManager().addMeshInstanceCollision(colMesh, mat, false, false);
			//			if(meshId)
			//				mg.Meshs.push_back(meshId);
		}

		OpenMesh.enableCastShadowMap(true);
		OpenMesh.setShadowMapDirectionZThreshold(-0.86f);
		OpenMesh.setShadowMapMaxDepth(0.5f);
		OpenMesh.getShape().getVisualCollisionMesh(colMesh);
		// if this mesh has a collision
		if(!colMesh.empty())
		{
			// get the instance matrix
			const CMatrix &mat = OpenMesh.getMatrix();

			// then send the result to the collision manager, and keep the mesh col id if succeed
			uint32     meshId = C3DTask::instance().collisionManager().addMeshInstanceCollision(colMesh, mat, false, false);
			//			if(meshId)
			//				mg.Meshs.push_back(meshId);
		}

	}
}


bool CEntity::isLocal()
{
	return CMtpTarget::instance().controler().getControledEntity() == Id;
}

void CEntity::setIsLocal(bool local)
{
	if(local)
		CMtpTarget::instance().controler().setControledEntity(id());
}


void CEntity::fadeOpenParticleColorTo(const NLMISC::CRGBA &color,float duration)
{
	if(FadeOpenParticleColor==color || duration<=0) return;

	FadeOpenParticleColor = color;
	FadeOpenParticleStartColor = TraceParticleOpen.getUserColor();
	FadeOpenParticleDuration = duration;
	FadeOpenParticleStartTime = (float)CTimeTask::instance().time();
}

void CEntity::fadeOpenParticleColorUpdate()
{
	float time = (float)CTimeTask::instance().time();
	float lpos = (time  - FadeOpenParticleStartTime) / FadeOpenParticleDuration;
	if(lpos<0 || 1<lpos) return;

	CRGBA newCol;
	newCol.blendFromui(FadeOpenParticleStartColor,FadeOpenParticleColor,(uint)(256 * lpos));
 	if(!TraceParticleOpen.empty())
 		TraceParticleOpen.setUserColor(newCol);
}

void CEntity::fadeCloseParticleColorTo(const NLMISC::CRGBA &color,float duration)
{
	if(FadeCloseParticleColor==color || duration<=0) return;

	FadeCloseParticleColor = color;
	FadeCloseParticleStartColor = TraceParticleClose.getUserColor();
	FadeCloseParticleDuration = duration;
	FadeCloseParticleStartTime = (float)CTimeTask::instance().time();
}

void CEntity::fadeCloseParticleColorUpdate()
{
	float time = (float)CTimeTask::instance().time();
	float lpos = (time  - FadeCloseParticleStartTime) / FadeCloseParticleDuration;
	if(lpos<0 || 1<lpos) return;

	CRGBA newCol;
	newCol.blendFromui(FadeCloseParticleStartColor,FadeCloseParticleColor,(uint)(256 * lpos));
	if(!TraceParticleClose.empty())
		TraceParticleClose.setUserColor(newCol);
}

void  CEntity::setColor(const NLMISC::CRGBA &col)
{
	nlinfo("LEVEL: setColor changes from %d %d %d %d to %d %d %d %d", Color.R, Color.G, Color.B, Color.A, col.R, col.G, col.B, col.A);

	Color = col;

	if(!CloseMesh.empty())
	{
		for(uint i = 0; i < CloseMesh.getNumMaterials(); i++)
		{
			CloseMesh.getMaterial(i).setDiffuse(Color);
			CloseMesh.getMaterial(i).setAmbient(Color);
		}
	}
	if(!OpenMesh.empty())
	{
		for(uint i = 0; i < OpenMesh.getNumMaterials(); i++)
		{
			OpenMesh.getMaterial(i).setDiffuse(Color);
			OpenMesh.getMaterial(i).setAmbient(Color);
		}
	}
}

void CEntity::setTotalScore(sint32 score)
{
	if(isLocal())
	{
		CMtpTarget::instance().displayTutorialInfo(score<=CConfigFileTask::instance().configFile().getVar("MinTotalScoreToHideTutorial").asInt());
	}
	TotalScore = score;
}

void CEntity::setStartPointId(uint8 spid)
{
	StartPointId = spid;
	if(isLocal() && CLevelManager::instance().levelPresent())
	{
		CMtpTarget::instance().controler().Camera.setInitialPosition(CLevelManager::instance().currentLevel().cameraPosition(StartPointId));
	}
}

void CEntity::collideWhenFly(CVector &pos)
{
	showCollideWhenFlyPos = pos;
	showCollideWhenFly = true;
	Collided = true;
}

struct CApplyTextureJob : public CJob
{
	CApplyTextureJob(const string &t, uint8 id) : Texture(t), Id(id)
	{
		Description = "Applying texture '"+Texture+"' on id "+toString(Id);
	}
	virtual void run()
	{
		string ffn = CConfigFileTask::instance().cacheDirectory()+Texture;
		if(CFile::fileExists(ffn))
		{
			CSynchronized<list<pair<string, uint8> > >::CAccessor acces(&CEntityManager::instance().ReloadTextures);
			acces.value().push_back(make_pair(Texture, Id));
		}
		else
		{
			nlwarning("Cannot apply texture, file not found %s", ffn.c_str());
		}
	}
	string Texture;
	uint8 Id;
};

void CEntity::setTexture(const string &path)
{
	string filename = CFile::getFilename(path);

	string ffn = CConfigFileTask::instance().cacheDirectory()+filename;
	bool inQueue = CAsyncJob::instance().downloadInQueue(filename);

	if(filename.empty() || !CFile::isExists(ffn) || inQueue)
	{
		if(!filename.empty() && !inQueue)
		{
			string server("http://");
			server += CConfigFileTask::instance().configFile().getVar("LSHost").asString();
			if(CConfigFileTask::instance().configFile().getVar("LSHost").asString() != "www.mtp-target.org")
				server += "/www";
			server += "/";
			CAsyncJob::instance().addDownloadJob(server+path, ffn);
		}
		if(!filename.empty()) CAsyncJob::instance().addJob(new CApplyTextureJob(filename, Id));

		ffn = CPath::lookup("ping_ball_blue.dds");
	}
	if(filename.empty())
		Texture = "ping_ball_blue.dds";
	else
		Texture = filename;

	// set the texture now if object is available
	if(OpenMesh.empty() || CloseMesh.empty())
		return;

	for(uint i = 0; i < OpenMesh.getNumMaterials(); i++)
	{
		OpenMesh.getMaterial(i).setTextureFileName(ffn);
	}
	for(uint i = 0; i < CloseMesh.getNumMaterials(); i++)
	{
		CloseMesh.getMaterial(i).setTextureFileName(ffn);
	}
}

bool CEntity::collided ()
{
	return Collided;
}

void CEntity::playSound (CSoundManager::TSound sound)
{
#ifdef USE_FMOD
	FMOD::Channel *channel = CSoundManager::instance().playSound(sound);
	if(channel)
		Channels.push_back(channel);
#endif
}

void CEntity::setTeam(uint8 team)
{
	Team = team;

	switch(team)
	{
	case 0:
		setColor(CRGBA(255,100,100));
		if(isLocal())
		{
			float fs = float(CFontManager::instance().textContext(CFontManager::TCBig).getFontSize());
			float y = float(C3DTask::instance().screenHeight() - 3.5*fs)/fs;
			CHudTask::instance().addMessage(CHudMessage(-2.0f, y, 1, CRGBA(255,0,0), 15, CI18N::get("RedTeam"), UTextContext::MiddleBottom));
		}
		break;
	case 1:
		setColor(CRGBA(100,100,255));
		if(isLocal())
		{
			float fs = float(CFontManager::instance().textContext(CFontManager::TCBig).getFontSize());
			float y = float(C3DTask::instance().screenHeight() - 3.5*fs)/fs;
			CHudTask::instance().addMessage(CHudMessage(-2.0f, y, 1, CRGBA(128,128,255), 15, CI18N::get("BlueTeam"), UTextContext::MiddleBottom));
		}
		break;
	case 255:
		setColor(CRGBA(255,255,255));
		break;
	default:
		nlstop;
	}
}

//////////////////////////////////////////////////////////////////////////

int CEntity::setMetatable(lua_State *L)
{
	int metatable = lua_gettop(L);
	int res = lua_setmetatable(L, -2);
	if(res==0)
		nlwarning("cannot set metatable");
	return 0;
}

int CEntity::getUserData(lua_State *luaSession)
{
	lua_getref(luaSession, LuaUserDataRef); //push obj which have this ref id
	/*
	if(!LuaUserDataRef)
	nlwarning("client lua call getuserdata but userdata is 0");
	*/
	return 1; // one return value
}

int CEntity::setUserData(lua_State *luaSession)
{
	LuaUserData = lua_touserdata(luaSession, 1); // get arg
	LuaUserDataRef = lua_ref(luaSession,1); //get ref id and lock it
	return 0; // no return value
}

int CEntity::name(lua_State *luaSession)
{
	lua_pushstring(luaSession, name().toUtf8().c_str());
	return 1;
}

int CEntity::setCurrentScore(lua_State *luaSession)
{
	lua_Number score = luaL_checknumber(luaSession,1);
	setCurrentScore(sint32(score));
	return 0;
}

/*int CEntity::setColor(lua_State *luaSession)
{
	uint8 r = (uint8 )luaL_checknumber(luaSession,1);
	uint8 g = (uint8 )luaL_checknumber(luaSession,2);
	uint8 b = (uint8 )luaL_checknumber(luaSession,3);
	uint8 a = (uint8 )luaL_checknumber(luaSession,4);
	color(CRGBA(r,g,b,a));
	return 0;
}*/

int CEntity::setStartPointId(lua_State *luaSession)
{
	uint8 startPointId = (uint8)luaL_checknumber(luaSession,1);
	setStartPointId(startPointId);
	return 0;
}
