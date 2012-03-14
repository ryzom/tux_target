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
// Includes
//

#include "stdpch.h"

#include "time.h"

#include "3d_task.h"
#include "time_task.h"
#include "game_task.h"
#include "chat_task.h"
#include "mtp_target.h"
#include "intro_task.h"
#include "network_task.h"
#include "font_manager.h"
#include "task_manager.h"
#include "background_task.h"
#include "config_file_task.h"
#include "resource_manager2.h"

extern FILE *SessionFile;

//
// Namespaces
//

using namespace std;
using namespace NL3D;
using namespace NLMISC;


//
// Variables
//

#define CHAT_COLUMNS 160

static std::list<std::string> ChatText;
static std::string ChatInput = "";
static std::list<std::string>::reverse_iterator CurrentChatLine = ChatText.rbegin();


//
// Functions
//
	
void CChatTask::init()
{
	chatLineCount = CConfigFileTask::getInstance().configFile().getVar("ChatLineCount").asInt();
	logChat = CConfigFileTask::getInstance().configFile().getVar("LogChat").asInt()!=0;
	fp = NULL;
	if(logChat)
	{
		fp = fopen ("chat.log", "at");
		time_t t = time(NULL);
		tm *ltime = localtime(&t);
		fprintf(fp,"-----------------------------------\n\n");
		fprintf(fp,"%02d-%02d-%d\n",ltime->tm_mon+1,ltime->tm_mday,1900+ltime->tm_year);
	}
	
}

void CChatTask::update()
{
	// Get user input
	string res = C3DTask::getInstance().kbGetString();
	for (const char *src = res.c_str(); *src != '\0';src++)
	{
		if (*src == 27)
			continue; // Escape
		
		if (*src == 8)
		{ // Backspace
			if (ChatInput.length() > 0)
			{
				ChatInput.resize(ChatInput.size()-1);
			}
			continue;
		}
		
		if (*src == '\r')
		{
			if (ChatInput.length() > 0)
			{
				if (ChatInput[0] == '/')
				{
					if(ChatInput.substr(0,7)=="/replay")
					{
						if(SessionFile)
						{
							string comment = ChatInput.substr(8);
							fprintf(SessionFile,"0 CM %s\n",comment.c_str());
							addLine(">> you marked this replay(" + CMtpTarget::getInstance().sessionFileName()+") : " + comment);
							CMtpTarget::getInstance().moveReplay(true);
						}
						else
						{
							addLine(">> no replay to mark");
						}
					}
					else if(ChatInput.substr(0,5)=="/help")
					{
						CNetworkTask::getInstance().command(ChatInput.substr(1));
						addLine("/help : this help");
						addLine("/replay [comment] : mark a replay with the comment");
					}
					else
						CNetworkTask::getInstance().command(ChatInput.substr(1));
				}
				else
					CNetworkTask::getInstance().chat(ChatInput);
			}
			ChatInput = "";
			continue;
		}
		
		if (ChatInput.size() >= CHAT_COLUMNS - 10)
			continue;
		
		//		if (isprint(*src))
		if (((uint8)(*src))>=32)
		{
			ChatInput += *src;
		}
	}
	if (C3DTask::getInstance().kbPressed(KeyPRIOR))
	{
		std::list<std::string>::reverse_iterator it = ChatText.rend();
		sint32 i;
		// can't go back up too much
		for (i = 0; i < chatLineCount && it != ChatText.rbegin(); i++, --it)
		{
			// nothing
		}
		// ok, go back up
		for (i = 0; i < chatLineCount && CurrentChatLine != it; i++, ++CurrentChatLine)
		{
			// nothing
		}
	}
	if (C3DTask::getInstance().kbPressed(KeyNEXT))
	{
		// go back down
		for (sint32 i = 0; i < chatLineCount && CurrentChatLine != ChatText.rbegin(); i++, --CurrentChatLine)
		{
		}
	}
}

void CChatTask::render()
{
	uint cl = chatLineCount;
//ace todo	if(large) cl *= 4;
	
	C3DTask::getInstance().driver().setFrustum(CFrustum(0, (float)C3DTask::getInstance().screenWidth(), 0, (float)C3DTask::getInstance().screenHeight(), -1, 1, false));
	C3DTask::getInstance().driver().clearZBuffer();
	
	// black background
	C3DTask::getInstance().driver().drawQuad(0.0f, (float)C3DTask::getInstance().screenHeight() - 16*(cl+1)-8, C3DTask::getInstance().screenWidth(), C3DTask::getInstance().screenHeight(), CRGBA (0, 0, 0, 64));
	
	// text
	std::list<std::string>::reverse_iterator it = CurrentChatLine;
	for (sint32 i = cl - 1; i >= 0 && it != ChatText.rend(); i--, ++it)
	{
		CFontManager::getInstance().littlePrintf(0.0f, (float)i, "%s", (*it).c_str());
	}
	CFontManager::getInstance().littlePrintf(0.0f, (float)cl, "> %s", ChatInput.c_str());
}

void CChatTask::release()
{
}

void CChatTask::addToInput(const string &text)
{
	ChatInput += text;
}

void CChatTask::addLine(const std::string &text)
{
	if (CurrentChatLine == ChatText.rbegin())
	{
		ChatText.push_back(text);
		CurrentChatLine = ChatText.rbegin();
	}
	else
	{
		ChatText.push_back(text);
	}
	if(logChat && fp)
	{
		time_t aclock;
		time( &aclock );
		struct tm *newtime;
		newtime = localtime( &aclock );
		fprintf(fp,"%02d:%02d:%02d %s\n",newtime->tm_hour,newtime->tm_min,newtime->tm_sec,text.c_str());
		fflush(fp);
	}
}
