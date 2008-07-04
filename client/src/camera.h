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

#ifndef MT_CAMERA_H
#define MT_CAMERA_H


//
// Includes
//

#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>


//
// Namespaces
//

using NLMISC::CVector;
using NLMISC::CMatrix;


//
// Functions
//

void lookAt(CMatrix &m, const CVector &From, const CVector &At, const CVector &Up);


//
// Classes
//

class CCamera
{
public:
	CCamera();
	~CCamera();

	void		 setInitialPosition(const CVector &initialPosition);
	void		 setFollowedEntity(uint8 eid);
	//void		 setQuakeControl(bool on);

	void		 reset();
	uint8		 getFollowedEntity();
	
	void		 update();
	bool		 updateRampe(float backDist,float height,float targetBackDist,float targetHeight);
	
	CMatrix		*getMatrix();
	CMatrix		*getMatrixNoZoom();
	CMatrix		*getMatrixQuake();
	CMatrix		*getMatrixFollow();

	CVector		 project(const CVector &point);

	float		 minDistFromStartPointToMove();
	void		 minDistFromStartPointToMove(float dist);
	float		 minDistFromStartPointToMoveVerticaly();
	void		 minDistFromStartPointToMoveVerticaly(float dist);
	
	float		 ForcedSpeed;

private:

	CMatrix		 MatrixFollowNoZoom;
	CMatrix		 MatrixFollow;
	CMatrix		 MatrixQuake;
	CMatrix		*ActiveMatrix;

	uint8		 EId;

	CVector		 InitialPosition;
	CVector		 Position;
	CVector		 LastEntityPosition;
	CVector		 CurrentLookAt;
	CVector		 Velocity;

	float		 Facing;

	float		 CurrentHeight;
	float		 CurrentBackDist;
	float		 CurrentTargetHeight;
	float		 CurrentTargetBackDist;

	float		 CurrentHeightSpeed;
	float		 MinDistFromStartPointToMove;
	float		 MinDistFromStartPointToMoveVerticaly;
	
	friend class CChatTask;
};

#endif
