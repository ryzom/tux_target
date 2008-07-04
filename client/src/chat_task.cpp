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

#include <nel/misc/command.h>

#include "time.h"
#include "graph.h"
#include "fi_ime.h"
#include "3d_task.h"
#include "hud_task.h"
#include "time_task.h"
#include "game_task.h"
#include "chat_task.h"
#include "mtp_target.h"
#include "intro_task.h"
#include "network_task.h"
#include "font_manager.h"
#include "task_manager.h"
#include "entity_manager.h"
#include "background_task.h"
#include "config_file_task.h"
#include "../../common/async_job.h"


//
// Namespaces
//

using namespace NLMISC;
using namespace NL3D;


//
// Variables
//

#define CHAT_COLUMNS 160

struct ChatLine
{
	ChatLine(const ucstring &t) : Text(t), Color(CRGBA::White) { }
	ChatLine(const ucstring &t, const CRGBA &c) : Text(t), Color(c) { }
	ucstring Text;
	CRGBA Color;
};

static list<ChatLine> ChatText;

static ucstring ChatInput;
static list<ChatLine>::reverse_iterator CurrentChatLine = ChatText.rbegin();

extern CFullIME fi;

extern CVariable<sint32> nbsmsg;
extern CVariable<sint32> sizesmsg;
extern CVariable<sint32> nbrmsg;
extern CVariable<sint32> sizermsg;


//
// Functions
//

CChatTask::CChatTask()
{
	fp = 0;
	LogChat = false;
	ChatLineCount = 0;
}

void CChatTask::init()
{
	ChatLineCount = CConfigFileTask::instance().configFile().getVar("ChatLineCount").asInt();
	LogChat = CConfigFileTask::instance().configFile().getVar("LogChat").asInt()==1;
	if(LogChat)
	{
		fp = fopen (string(CConfigFileTask::instance().documentDirectory()+"chat.txt").c_str(), "at");
		time_t t = time(NULL);
		tm *ltime = localtime(&t);
		fprintf(fp,"\n\n--------------------------------------------------------------\n");
		fprintf(fp,"%s --- Game launched ---\n", IDisplayer::dateToHumanString());
	}
}

void CChatTask::update()
{
	// Get user input
	ucstring res = C3DTask::instance().kbGetString();
	for (uint32 i = 0; i < res.size() ; i++)
	{
		if (res[i] == 27)
			continue; // Escape

		if (res[i] == 8)
		{ // Backspace
			if (ChatInput.length() > 0)
			{
				ChatInput.resize(ChatInput.size()-1);
			}
			continue;
		}

		if (res[i] == '\r')
		{
			if (ChatInput.length() > 0)
			{
				if (ChatInput[0] == '/')
				{
					if(ChatInput.substr(0,7)==ucstring("/replay"))
					{
						ucstring comment;
						if(ChatInput.size() > 8) comment = ChatInput.substr(8);
						SessionString += toString("0 CM %s\n", comment.c_str());
						addLine(ucstring(">> you saved this session is the client\\replay\\marked directory"));
						CMtpTarget::instance().moveReplay(true);
					}
					else if(ChatInput.substr(0,10)==ucstring("/unignore "))
					{
						ucstring ucname = ChatInput.substr(10);
						string name = ucname.toUtf8();
						CConfigFile::CVar &var = CConfigFileTask::instance().configFile().getVar("IgnoreList");
						for(uint i = 0; i < var.size(); i++)
						{
							if(var.asString(i) == name)
							{
								var.setAsString("", i);
								CConfigFileTask::instance().configFile().save();
								addLine(ucstring(">> You unignored ")+ucname);
							}
						}
					}
					else if(ChatInput.substr(0,8)==ucstring("/ignore "))
					{
						ucstring user = ChatInput.substr(8);
						CEntity *e = CEntityManager::instance().getByName(user);
						if(e != 0 && CMtpTarget::instance().controler().getControledEntity() != e->id())
						{
							CConfigFile::CVar &var = CConfigFileTask::instance().configFile().getVar("IgnoreList");
							var.setAsString(user.toUtf8(), var.size());
							CConfigFileTask::instance().configFile().save();
							addLine(ucstring(">> You ignored ")+user);
						} else {
							addLine(ucstring(">> You cannot ignore ")+user);
						}
					}
					else if(ChatInput.substr(0,5)==ucstring("/help"))
					{
						ucstring raw = ChatInput.substr(1);
						CNetworkTask::instance().command(raw.toString());
						addLine(ucstring("/help : this help"));
						addLine(ucstring("/replay [comment] : mark a replay with the comment"));
					} else {
						ucstring raw = ChatInput.substr(1);
						CNetworkTask::instance().command(raw.toString());
					}
				}
				else if (ChatInput[0] == '.')
				{
					ucstring raw = ChatInput.substr(1);
					ICommand::execute(raw.toString(), *InfoLog);
				}
				else
					CNetworkTask::instance().chat(ChatInput);
			}
			ChatInput.clear();
			continue;
		}

		if (ChatInput.size() >= CHAT_COLUMNS - 10)
			continue;

		ChatInput += res[i];
	}
	if (C3DTask::instance().kbPressed(KeyPRIOR))
	{
		list<ChatLine>::reverse_iterator it = ChatText.rend();
		sint32 i;
		// can't go back up too much
		for (i = 0; i < ChatLineCount && it != ChatText.rbegin(); i++, --it)
		{
			// nothing
		}
		// ok, go back up
		for (i = 0; i < ChatLineCount && CurrentChatLine != it; i++, ++CurrentChatLine)
		{
			// nothing
		}
	}
	if (C3DTask::instance().kbPressed(KeyNEXT))
	{
		// go back down
		for (sint32 i = 0; i < ChatLineCount && CurrentChatLine != ChatText.rbegin(); i++, --CurrentChatLine)
		{
			// nothing
		}
	}
}

void CChatTask::render()
{
	if(CConfigFileTask::instance().configFile().getVar("NoHUD").asInt()==1) return;

	uint cl = ChatLineCount;

	C3DTask::instance().driver().setFrustum(CFrustum(0, (float)C3DTask::instance().screenWidth(), 0, (float)C3DTask::instance().screenHeight(), -1, 1, false));
	C3DTask::instance().driver().clearZBuffer();

	// black background
	uint32 size = CFontManager::instance().textContext(CFontManager::TCChat).getFontSize();
	C3DTask::instance().driver().drawQuad(0.0f, (float)C3DTask::instance().screenHeight() - size*(cl+1)-2, C3DTask::instance().screenWidth(), C3DTask::instance().screenHeight(), CRGBA (0, 0, 0, 100));

	// text
	list<ChatLine>::reverse_iterator it = CurrentChatLine;
	for (sint32 i = cl - 1; i >= 0 && it != ChatText.rend(); i--, ++it)
	{
		CFontManager::instance().print(CFontManager::TCChat, (*it).Color, 0.0f, (float)i, (*it).Text, true);
	}
	CFontManager::instance().print(CFontManager::TCChat, 0.0f, (float)cl, fi.indicator() + ucstring("> ") + ChatInput + fi.composition(), true);

	// debug stuff
	if (DisplayDebug==3)
	{
		float h = 8.0f;
//		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "RECORDING LOW LEVEL POS in trace.csv");
	}
	else if (DisplayDebug==2)
	{
		updateGraph ();
	}
	else if (DisplayDebug==1)
	{
		float h = 8.0f;
		CVector v = C3DTask::instance().scene().getCam().getMatrix().getPos();

		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "pos %.2f %.2f %.2f", C3DTask::instance().scene().getCam().getMatrix().getPos().x, C3DTask::instance().scene().getCam().getMatrix().getPos().y, C3DTask::instance().scene().getCam().getMatrix().getPos().z);

		CQuat q = C3DTask::instance().scene().getCam().getMatrix().getRot();
		CAngleAxis aa = q.getAngleAxis();
		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "rot %.2f %.2f %.2f %.2f", aa.Axis.x, aa.Axis.y, aa.Axis.z, aa.Angle);

		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "%.2ffps %.2fms", CTimeTask::instance().fps(), CTimeTask::instance().spf()*1000.0f);

		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "LCT %d", LCT.get());

		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "FxNbFaceAsked %.2f", C3DTask::instance().scene().getGroupNbFaceAsked("Fx"));

		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "FxNbFaceMax %d", C3DTask::instance().scene().getGroupLoadMaxPolygon("Fx"));

		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "TotalNbFacesAsked %.2f", C3DTask::instance().scene().getNbFaceAsked());

		CPrimitiveProfile in, out;
		C3DTask::instance().driver().profileRenderedPrimitives(in, out);
		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "in:  %d %d %d %d %d", in.NPoints, in.NLines, in.NTriangles, in.NQuads, in.NTriangleStrips);
		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "out: %d %d %d %d %d", out.NPoints, out.NLines, out.NTriangles, out.NQuads, out.NTriangleStrips);

		uint8 id = CMtpTarget::instance().controler().getControledEntity();
		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "Cam (%.2f %.2f %.2f)", CMtpTarget::instance().controler().Camera.getMatrixFollow()->getPos().x, CMtpTarget::instance().controler().Camera.getMatrixFollow()->getPos().y, CMtpTarget::instance().controler().Camera.getMatrixFollow()->getPos().z);
		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "Pos (%.2f %.2f %.2f)", CMtpTarget::instance().controler().Camera.Position.x, CMtpTarget::instance().controler().Camera.Position.y, CMtpTarget::instance().controler().Camera.Position.z);
		if (CMtpTarget::instance().controler().Camera.EId == 255)
			CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "curpos no follow");
		else
		{
			CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "curpos (%.2f %.2f %.2f)", CEntityManager::instance()[CMtpTarget::instance().controler().Camera.EId].interpolator().position().x, CEntityManager::instance()[CMtpTarget::instance().controler().Camera.EId].interpolator().position().y, CEntityManager::instance()[CMtpTarget::instance().controler().Camera.EId].interpolator().position().z);
			CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "facing %.2f", CMtpTarget::instance().controler().Camera.Facing);

			CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "ping %u", CEntityManager::instance()[CMtpTarget::instance().controler().Camera.EId].ping());
		}

		fi.displayIMEInfo();

		h++;
		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "AsyncJobDebug: %s", CAsyncJob::instance().toString().c_str());

		if(CEntityManager::instance().exist(CMtpTarget::instance().controler().Camera.EId))
		{
			const CVector &pos = CEntityManager::instance()[CMtpTarget::instance().controler().Camera.EId].interpolator().position();
			CVector camPos;
			if(CMtpTarget::instance().spectator())
				camPos = ControlerFreeLookPos;
			else
				camPos = CMtpTarget::instance().controler().Camera.getMatrix()->getPos();
			CVector dpos = pos - camPos;
			CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "dpos: %f", dpos.norm());
		}

		h++;
		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "%s", nbsmsg.getStat(true).c_str());
		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "%s", sizesmsg.getStat(true).c_str());
		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "%s", nbrmsg.getStat(true).c_str());
		CFontManager::instance().printf(CFontManager::TCDebug, 0.0f, h++, "%s", sizermsg.getStat(true).c_str());
	}
}

void CChatTask::release()
{
}

void CChatTask::addToInput(const ucstring &text)
{
	for(uint i = 0; i < text.size(); i++)
	{
		ChatInput += text[i];
	}
}

void CChatTask::addLine(const ucstring &text)
{
	CRGBA col(CRGBA::White);

	if(text.size() > 2 && (text[0] == '<' || text[0] == '{'))
	{
		uint32 pos = text.find('>');
		if(pos == string::npos)
			pos = text.find('}');
		if(pos != string::npos)
		{
			ucstring ucname = text.substr(1, pos-1);
			string name = ucname.toUtf8();

			// check if the user is not ignored
			CConfigFile::CVar &var = CConfigFileTask::instance().configFile().getVar("IgnoreList");
			for(uint i = 0; i < var.size(); i++)
			{
				if(var.asString(i) == name)
				{
					return;
				}
			}

			CEntity *e = CEntityManager::instance().getByName(ucname);
			if(e)
			{
				if(e->team() == 0) col.set(255,200,200);
				else if(e->team() == 1) col.set(200,200,255);
			}
		}
	}

	bool end = CurrentChatLine == ChatText.rbegin();
	ChatText.push_back(ChatLine(text, col));
	if (end) CurrentChatLine = ChatText.rbegin();

	if(LogChat && fp)
	{
		fprintf(fp, "%s %s\n", IDisplayer::dateToHumanString(), text.toUtf8().c_str());
		fflush(fp);
	}
}
