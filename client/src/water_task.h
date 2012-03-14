/* Copyright, 2010 Tux Target
 * Copyright, 2003 Melting Pot
 *
 * This file is part of Tux Target.
 * Tux Target is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * Tux Target is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Tux Target; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */


//
// This is the main class that manages all other classes
//

#ifndef MTPT_WATER_TASK_H
#define MTPT_WATER_TASK_H


//
// Includes
//

#include <nel/3d/transform_shape.h>


//
// Externs
//

namespace NL3D
{
	class UScene;
	class UInstance;
	class UCloudScape;
	class CWaterShape;
	class CWaterModel;
}

//
// Classes
//

class CWaterTask : public NLMISC::CSingleton<CWaterTask>, public ITask
{
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual std::string name() const { return "CWaterTask"; }

	void envMap0Name(const std::string &envMapName);
	void envMap1Name(const std::string &envMapName);
	void heightMap0Name(const std::string &heightMap);
	void heightMap1Name(const std::string &heightMap);

	std::string envMap0Name();
	std::string envMap1Name();
	std::string heightMap0Name();
	std::string heightMap1Name();
	
	friend class NLMISC::CSingleton<CWaterTask>;
protected:
	CWaterTask();
	
private:

	NL3D::UScene			*nelWaterScene;
	NL3D::CWaterShape		*WaterShape;
	NL3D::CWaterModel		*WaterModel;
	NL3D::UInstance			WaterMesh;	
	NL3D::CTransformShape   *WaterInstance;

	std::string EnvMap0Name;
	std::string EnvMap1Name;
	std::string HeightMap0Name;
	std::string HeightMap1Name;
	
	friend void cbVar (NLMISC::CConfigFile::CVar &var);
};

#endif
