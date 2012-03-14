/* Copyright, 2001 MELTING POT.
 *
 * This file is part of MTP CHAT.
 * MTP CHAT is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * MTP CHAT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with MTP CHAT; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

/* DataBase.C */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/*
#define _XOPEN_SOURCE
#include <unistd.h>
*/
extern char *crypt (const char *, const char *); /* Solashit */

#include "database.h"
#include "types.h"
#include "channel.h"
#include "group.h"
#include "list.h"
#include "object.h"
#include "mtp.h"
#include "server.h"
#include "token.h"
#include "user.h"
#include "variable.h"

/* Constants */

#define DATABASE_NAME "database"

/* Variables */

int DataBaseChanged;

static char *Line = NULL, *LinePtr;

/* Prototypes */

static char *FirstToken (const char *String);
static char *NextToken  (void);
static char *LastToken  (void);

static int   Rand64     (void);

/* Functions */

/* ReadDataBase() */

int ReadDataBase(void) {

   FILE   *DataBase;
   char    Line[256], *UserName, *GroupName, *Password, *Name, *EMail, *Formation, *LeaderName, *AliasName, *Command, *Plan, *LastLoginHost, *LastFailHost;
   time_t  RegisterTime, ConnectTime, Birthday, LastLoginTime, LastFailTime;
   int     Level, KickNb, KickedNb, LoginNb, TotalTime, LineNumber, FailureNb;
   user   *User, *UId;
   group  *Group;
   alias  *Alias;

   DataBase = fopen(DATABASE_NAME,"r");
   if (DataBase == NULL) {
      Error("Couldn't open database");
      return FALSE;
   }

   do {
      if (fgets(Line,256,DataBase) == NULL) {
         Error("\"# <Mtp> Variables\" line not found in \"%s\"",DATABASE_NAME);
         fclose(DataBase);
         return FALSE;
      }
   } while (! SameString(Line,"# <Mtp> Variables\n"));

   /* # <Mtp> Variables */
   /* UserNbMax ... */

   do {
      if (fgets(Line,256,DataBase) == NULL) {
         Error("\"UserNbMax ...\" line not found in \"%s\"",DATABASE_NAME);
         fclose(DataBase);
         return FALSE;
      }
   } while (! StartWith(Line,"UserNbMax "));

   UserNbMax = atoi(Line+10);

   do {
      if (fgets(Line,256,DataBase) == NULL) {
         Error("\"# <Mtp> Groups\" line not found in \"%s\"",DATABASE_NAME);
         fclose(DataBase);
         return FALSE;
      }
   } while (! SameString(Line,"# <Mtp> Groups\n"));

   /* # <Mtp> Groups */
   /* # Group|Leader|Level|Name */
   /* group *NewGroup(const char *Id, const char *Leader, int Level, const char *Name); */

   do {

      if (fgets(Line,256,DataBase) == NULL) {
         Error("\"# <Mtp> Users\" line not found in \"%s\"",DATABASE_NAME);
         fclose(DataBase);
         return FALSE;
      }

      if (Line[0] != '\n' && Line[0] != '#') {

         GroupName  = FirstToken(Line);
         LeaderName = NextToken();
         Level      = atoi(NextToken());
         Name       = LastToken();

         Group = NewGroup(GroupName,LeaderName,Level,Name);
         if (Group != NULL) AddTail(GroupList,Group);
      }

   } while (! SameString(Line,"# <Mtp> Users\n"));

   /* # <Mtp> Users */
   /* # User|Group|Password|Name|EMail|Formation|RegisterTime|ConnectTime|Birthday|KickNb|KickedNb|LoginNb|TotalTime|LastLoginHost|LastLoginTime|FailureNb|LastFailHost|LastFailTime */
   /* user *NewUId(const char *Id, const char *Group, const char *Password, const char *Name, const char *EMail, const char *Formation, time_t RegisterTime, time_t ConnectTime, time_t Birthday, int KickNb, int KickedNb, int LoginNb, int TotalTime, const char *LastLoginHost, time_t LastLoginTime, int FailureNb, const char *LastFailHost, time_t LastFailTime); */

   do {

      if (fgets(Line,256,DataBase) == NULL) {
         Error("\"# <Mtp> Aliases\" line not found in \"%s\"",DATABASE_NAME);
         fclose(DataBase);
         return FALSE;
      }

      if (Line[0] != '\n' && Line[0] != '#') {

         UserName      = FirstToken(Line);
         GroupName     = NextToken();
         Password      = NextToken();
         Name          = NextToken();
         EMail         = NextToken();
         Formation     = NextToken();
         RegisterTime  = (time_t) atoi(NextToken());
         ConnectTime   = (time_t) atoi(NextToken());
         Birthday      = (time_t) atoi(NextToken());
         KickNb        = atoi(NextToken());
         KickedNb      = atoi(NextToken());
         LoginNb       = atoi(NextToken());
         TotalTime     = atoi(NextToken());
         LastLoginHost = NextToken();
         LastLoginTime = (time_t) atoi(NextToken());
         FailureNb     = atoi(NextToken());
         LastFailHost  = NextToken();
         LastFailTime  = (time_t) atoi(NextToken());

         UId = NewUId(UserName,GroupName,Password,Name,EMail,Formation,RegisterTime,ConnectTime,Birthday,KickNb,KickedNb,LoginNb,TotalTime,LastLoginHost,LastLoginTime,FailureNb,LastFailHost,LastFailTime);
         if (UId != NULL) AddTail(UserList,UId);
      }

   } while (! SameString(Line,"# <Mtp> Aliases\n"));

   /* # <Mtp> Aliases */
   /* # User|Alias|Command */
   /* alias *NewAlias(const char *Id, const char *Command); */

   do {

      if (fgets(Line,256,DataBase) == NULL) {
         Error("\"# <Mtp> Plans\" line not found in \"%s\"",DATABASE_NAME);
         fclose(DataBase);
         return FALSE;
      }

      if (Line[0] != '\n' && Line[0] != '#') {

         UserName  = FirstToken(Line);
         AliasName = NextToken();
         Command   = LastToken();

         User = SearchUId(UserName);
         if (User != NULL) {
            Alias = NewAlias(AliasName,Command);
            if (Alias != NULL) AddTail(User->Aliases,Alias);
         }
      }
   } while (! SameString(Line,"# <Mtp> Plans\n"));

   /* # <Mtp> Plans */
   /* # User|LineNumber|Plan */

   while (fgets(Line,256,DataBase) != NULL) {

      if (Line[0] != '\n' && Line[0] != '#') {

         UserName   = FirstToken(Line);
         LineNumber = atoi(NextToken());
         Plan       = LastToken();

         if (LineNumber >= 0 && LineNumber < PLAN_SIZE) {
            User = SearchUId(UserName);
            if (User != NULL) SetString(&User->Plan[LineNumber],Plan);
         }
      }
   }

   fclose(DataBase);

   DataBaseChanged = FALSE;

   return TRUE;
}

/* WriteDataBase() */

int WriteDataBase(void) {

   int I;
   FILE  *DataBase;
   node  *Node, *Node2;
   user  *User;
   group *Group;
   alias *Alias;

   DataBase = fopen(DATABASE_NAME,"w");
   if (DataBase == NULL) {
      perror("database write fopen");
      return FALSE;
   }

   fprintf(DataBase,"\n# <Mtp> Variables\n\n");
   fprintf(DataBase,"UserNbMax %d\n",UserNbMax);

   fprintf(DataBase,"\n# <Mtp> Groups\n\n# Group|Leader|Level|Name\n\n");

   for (Node = GroupList->Head; Node != NULL; Node = Node->Next) {
      Group = (group *) Node->Object;
      if (Group->Type == GROUP) {
         fprintf(DataBase,"%s|%s|%d|%s\n",Group->Id,Group->Leader,Group->Level,Group->Name);
      }
   }

   fprintf(DataBase,"\n# <Mtp> Users\n\n# User|Group|Password|Name|EMail|Formation|RegisterTime|ConnectTime|Birthday|KickNb|KickedNb|LoginNb|TotalTime|LastLoginHost|LastLoginTime|FailureNb|LastFailHost|LastFailTime\n\n");
   for (Node = UserList->Head; Node != NULL; Node = Node->Next) {
      User = (user *) Node->Object;
      if (User->Type == USER && User->Registered) {
         fprintf(DataBase,"%s|%s|%s|%s|%s|%s|%d|%d|%d|%d|%d|%d|%d|%s|%d|%d|%s|%d\n",User->Id,User->Group,User->Password,User->Name,User->EMail,User->Formation,(int)User->RegisterTime,(int)User->ConnectTime,(int)User->Birthday,User->KickNb,User->KickedNb,User->LoginNb,User->TotalTime,User->LastLoginHost,(int)User->LastLoginTime,User->FailureNb,User->LastFailHost,(int)User->LastFailTime);
      }
   }

   fprintf(DataBase,"\n# <Mtp> Aliases\n\n# User|Alias|Command\n\n");

   for (Node = UserList->Head; Node != NULL; Node = Node->Next) {
      User = (user *) Node->Object;
      if (User->Type == USER && User->Registered) {
         for (Node2 = User->Aliases->Head; Node2 != NULL; Node2 = Node2->Next) {
            Alias = (alias *) Node2->Object;
            fprintf(DataBase,"%s|%s|%s\n",User->Id,Alias->Id,Alias->Command);
         }
      }
   }

   fprintf(DataBase,"\n# <Mtp> Plans\n\n# User|LineNumber|Plan\n\n");

   for (Node = UserList->Head; Node != NULL; Node = Node->Next) {
      User = (user *) Node->Object;
      if (User->Type == USER && User->Registered) {
         for (I = 0; I <PLAN_SIZE; I++) {
	   if(User->Plan[I] != NULL && User->Plan[I][0] != '\0') {
	     fprintf(DataBase,"%s|%d|%s\n",User->Id,I,User->Plan[I]);
	   }
         }
      }
   }

   fprintf(DataBase,"\n");

   fclose(DataBase);

   DataBaseChanged = FALSE;

   return TRUE;
}

/* FirstToken() */

static char *FirstToken(const char *String) {

   SetString(&Line,String);
   LinePtr = Line;

   return NextToken();
}

/* NextToken() */

static char *NextToken(void) {

   char *Start;

   Start = LinePtr;

   while (*LinePtr != '\0' && *LinePtr != '|' && *LinePtr != '\n') LinePtr++;
   if (*LinePtr != '\0') *LinePtr++ = '\0';
   
   return Start;
}

/* LastToken() */

static char *LastToken(void) {

   char *Start;

   Start = LinePtr;

   while (*LinePtr != '\0' && *LinePtr != '\n') LinePtr++;
   if (*LinePtr != '\0') *LinePtr++ = '\0';
   
   return Start;
}

/* CryptPassword() */

char *CryptPassword(const char *Password) {

   char Salt[3];
   static char SaltString[64] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

   Salt[0] = SaltString[Rand64()];
   Salt[1] = SaltString[Rand64()];
   Salt[2] = '\0';

   return crypt(Password,Salt);
}

/* Rand64() */

static int Rand64(void) {

   return (int) floor(64.0*(double)rand()/((double)RAND_MAX+1.0));
}

/* CheckPassword() */

int CheckPassword(const char *String, const char *Encrypted) {

   char Salt[3];

   if (strlen(Encrypted) != ENCRYPTED_SIZE) {
      Error("CheckPassword(): \"%s\" is not a valid encrypted password",Encrypted);
      return FALSE;
   }

   Salt[0] = Encrypted[0];
   Salt[1] = Encrypted[1];
   Salt[2] = '\0';

   return strcmp(Encrypted,crypt(String,Salt)) == 0;
}

/* End of DataBase.C */
