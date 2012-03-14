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

/* User.C */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "user.h"
#include "types.h"
#include "channel.h"
#include "command.h"
#include "crypt.h"
#include "database.h"
#include "group.h"
#include "list.h"
#include "mtp.h"
#include "object.h"
#include "socket.h"
#include "token.h"
#include "variable.h"

/* Variables */

list UserList[1];

/* Prototypes */

static int  FindFile            (user *User, const char *FileName, const char *DirName);
static int  ConvertStringToTime (char *TimeString);
static void GetLine             (FILE *fp, char *buffer, int buffersize, int *day, int *timemin, int *timemax);

/* Functions */

/* NewUser() */

user *NewUser(const char *Id) {

   int I;
   user *User;

   if (Id == NULL) {
      Error("NewUser(): Id = NULL");
      return NULL;
   } else if (! IsId(Id)) {
      Error("NewUser(): Id = \"%s\" is not an id",Id);
      return NULL;
/*
   } else if (SearchObject(Lists,Id,OBJECT) != NULL) {
      Error("NewUser(): Id = \"%s\" is already used",Id);
      return NULL;
*/
   } else if ((User = calloc(1,sizeof(user))) == NULL) {
      Error("NewUser(): not enough memory for user \"%s\"",Id);
      return NULL;
   }

   User->Type = USER;
   User->Id   = NewString(Id);
   strcpy(User->Group,DEFAULT_GROUP);
   strcpy(User->Password,"");
   strcpy(User->Channel,"");
   User->Name      = NewString("<Unknown>");
   User->EMail     = NewString("<Unknown>");
   User->Formation = NewString("<Unknown>");
   User->Host      = NewString("<Unknown>");
   strcpy(User->Tell,"");
   strcpy(User->Invite,"");
   for (I = 0; I < PLAN_SIZE; I++) User->Plan[I] = NewString("");
   User->Language      = CRYPT_NONE;
   User->Registered    = FALSE;
   User->RegisterTime  = time(NULL);
   User->ConnectTime   = time(NULL);
   User->Birthday      = (time_t) 0;
   User->KickNb        = 0;
   User->KickedNb      = 0;
   User->LastSendTime  = time(NULL);
   User->LoginNb       = 0;
   User->TotalTime     = 0;
   User->LastLoginHost = NewString("");
   User->LastLoginTime = time(NULL);
   User->FailureNb     = 0;
   User->LastFailHost  = NewString("");
   User->LastFailTime  = time(NULL);
   User->State         = NOT_LOGGED;
   User->Away          = FALSE;
   User->Bell          = TRUE;
   User->Client        = NewString("telnet");
   User->InOut         = TRUE;
   User->Join          = TRUE;
   User->Shout         = TRUE;
   InitList(User->Aliases,"Aliases");

   User->Conn = NULL;

   return User;
}

/* NewUId() */

user *NewUId(const char *Id, const char *Group, const char *Password, const char *Name, const char *EMail, const char *Formation, time_t RegisterTime, time_t ConnectTime, time_t Birthday, int KickNb, int KickedNb, int LoginNb, int TotalTime, const char *LastLoginHost, time_t LastLoginTime, int FailureNb, const char *LastFailHost, time_t LastFailTime) {

   int I;
   user *User;

   if (Id == NULL) {
      Error("NewUId(): Id = NULL");
      return NULL;
   } else if (! IsId(Id)) {
      Error("NewUId(): Id = \"%s\" is not an id",Id);
      return NULL;
   } else if (SearchObject(Lists,Id,OBJECT) != NULL) {
      Error("NewUId(): Id = \"%s\" is already used",Id);
      return NULL;
   } else if (Group == NULL) {
      Error("NewUId(): User = \"%s\", Group = NULL",Id);
      return NULL;
   } else if (! IsId(Group)) {
      Error("NewUId(): User = \"%s\", Group = \"%s\" is not an id",Id,Group);
      return NULL;
   } else if ((User = calloc(1,sizeof(user))) == NULL) {
      Error("NewUId(): not enough memory for user \"%s\"",Id);
      return NULL;
   }

   if (Password == NULL || strlen(Password) != ENCRYPTED_SIZE) {
      Error("\"%s\" hasn't got a valid password",Id);
      Password = "";
   }
   if (IsEmptyString(Name))      Name      = "<Unknown>";
   if (IsEmptyString(EMail))     EMail     = "<Unknown>";
   if (IsEmptyString(Formation)) Formation = "<Unknown>";
 
   User->Type = USER;
   User->Id   = NewString(Id);
   strcpy(User->Group,Group);
   strcpy(User->Password,Password);
   strcpy(User->Channel,"");
   User->Name      = NewString(Name);
   User->EMail     = NewString(EMail);
   User->Formation = NewString(Formation);
   User->Host      = NewString("<Unknown>");
   strcpy(User->Tell,"");
   strcpy(User->Invite,"");
   for (I = 0; I < PLAN_SIZE; I++) User->Plan[I] = NewString("");
   User->Language      = CRYPT_NONE;
   User->Registered    = TRUE;
   User->RegisterTime  = RegisterTime;
   User->ConnectTime   = ConnectTime;
   User->Birthday      = Birthday;
   User->KickNb        = KickNb;
   User->KickedNb      = KickedNb;
   User->LastSendTime  = time(NULL);
   User->LoginNb       = LoginNb;
   User->TotalTime     = TotalTime;
   User->LastLoginHost = NewString(LastLoginHost);
   User->LastLoginTime = LastLoginTime;
   User->FailureNb     = FailureNb;
   User->LastFailHost  = NewString(LastFailHost);
   User->LastFailTime  = LastFailTime;
   User->State         = NOT_LOGGED;
   User->Away          = FALSE;
   User->Bell          = TRUE;
   User->Client        = NewString("telnet");
   User->InOut         = TRUE;
   User->Join          = TRUE;
   User->Shout         = TRUE;
   InitList(User->Aliases,"Aliases");

   User->Conn = NULL;

   return User;
}

/* NewAlias() */

alias *NewAlias(const char *Word, const char *Command) {

   alias *Alias;

   if (Word == NULL) {
      Error("NewAlias(): Word = NULL");
      return NULL;
   } else if (! IsWord(Word)) {
      Error("NewAlias(): Word = \"%s\" is not a word",Word);
      return NULL;
   } else if ((Alias = calloc(1,sizeof(alias))) == NULL) {
      Error("NewAlias(): not enough memory for alias \"%s\"",Word);
      return NULL;
   }

   Alias->Type    = ALIAS;
   Alias->Id      = NewString(Word);
   Alias->Command = NewString(Command);

   return Alias;
}

/* InitUser() */

void InitUser(user *User, const char *Id) {

   int I;

   if (Id == NULL) {
      Error("InitUser(): Id = NULL");
      return;
   } else if (! IsId(Id)) {
      Error("InitUser(): Id = \"%s\" is not an id",Id);
      return;
   }

   User->Type = USER;
   User->Id   = NewString(Id);
   strcpy(User->Group,DEFAULT_GROUP);
   strcpy(User->Password,"");
   strcpy(User->Channel,"");
   SetString(&User->Name,"<Unknown>");
   SetString(&User->EMail,"<Unknown>");
   SetString(&User->Formation,"<Unknown>");
   strcpy(User->Tell,"");
   strcpy(User->Invite,"");
   for (I = 0; I < PLAN_SIZE; I++) SetString(&User->Plan[I],"");
   User->Language      = CRYPT_NONE;
   User->Registered    = FALSE;
   User->RegisterTime  = time(NULL);
   User->ConnectTime   = time(NULL);
   User->Birthday      = (time_t) 0;
   User->KickNb        = 0;
   User->KickedNb      = 0;
   User->LoginNb       = 0;
   User->TotalTime     = 0;
   SetString(&User->LastLoginHost,"");
   User->LastLoginTime = time(NULL);
   User->FailureNb     = 0;
   SetString(&User->LastFailHost,"");
   User->LastFailTime  = time(NULL);
   User->Away          = FALSE;
   User->Bell          = TRUE;
   SetString(&User->Client,"telnet");
   User->InOut         = TRUE;
   User->Shout         = TRUE;
}

/* InitUId() */

void InitUId(user *User) {

   strcpy(User->Tell,"");
   strcpy(User->Invite,"");
   User->Away = FALSE;
   User->Bell = TRUE;
   SetString(&User->Client,"telnet");
   User->InOut = TRUE;
   User->Shout = TRUE;
}

/* DeleteUser() */

void DeleteUser(user *User) {
   int I;
   alias *Alias;

   if (User == NULL) {
      Error("DeleteUser(): User = NULL");
      return;
   } else if (User->Type != USER) {
      Error("DeleteUser(): \"%s\" is not a user",User->Id);
      return;
   }

   DeleteString(&User->Id);
   DeleteString(&User->Name);
   DeleteString(&User->EMail);
   DeleteString(&User->Formation);
   DeleteString(&User->Host);
   for (I = 0; I < PLAN_SIZE; I++) DeleteString(&User->Plan[I]);
   DeleteString(&User->LastLoginHost);
   DeleteString(&User->LastFailHost);
   DeleteString(&User->Client);

   while (User->Aliases->Head != NULL) {
      Alias = (alias*) User->Aliases->Head->Object;
      RemObject(User->Aliases,Alias);
      DeleteAlias(Alias);
   }

   free(User);
}

/* DeleteAlias() */

void DeleteAlias(alias *Alias) {

   if (Alias == NULL) {
      Error("DeleteAlias(): Alias = NULL");
      return;
   } else if (Alias->Type != ALIAS) {
      Error("DeleteAlias(): \"%s\" is not an alias",Alias->Id);
      return;
   }

   DeleteString(&Alias->Id);
   DeleteString(&Alias->Command);

   free(Alias);
}

/* SearchUser() */

user *SearchUser(const char *Id) {

   user *User;

   if (Id == NULL) {
      Error("SearchUser(): Id = NULL");
      return NULL;
   } else if (! IsId(Id)) {
      Error("SearchUser(): Id = \"%s\" is not an id",Id);
      return NULL;
   }

   User = SearchObject(UserList,Id,USER);
   if (User != NULL && IsConnected(User)) {
      return User;
   }

   return NULL;
}

/* SearchUId() */

user *SearchUId(const char *Id) {

   user *User;

   if (Id == NULL) {
      Error("SearchUId(): Id = NULL");
      return NULL;
   } else if (! IsId(Id)) {
      Error("SearchUId(): Id = \"%s\" is not an id",Id);
      return NULL;
   }

   User = SearchObject(UserList,Id,USER);
   if (User != NULL && User->Registered) return User;

   return NULL;
}

/* SearchAlias() */

alias *SearchAlias(list *List, const char *Word) {

   if (Word == NULL) {
      Error("SearchAlias(): Word = NULL");
      return NULL;
   } else if (! IsWord(Word)) {
      Error("SearchAlias(): Word = \"%s\" is not a WORD",Word);
      return NULL;
   }

   return (alias *) SearchObject(List,Word,ALIAS);
}

/* UserLevel() */

int UserLevel(const user *User) {

   if (User == NULL) {
      Error("UserLevel(): User = NULL");
      return 0;
   } else if (User->Type != USER) {
      Error("UserLevel(): \"%s\" is not a user",User->Id);
      return 0;
   }

   return GroupLevel(SearchGroup(User->Group));
}

/* IsAdmin() */

int IsAdmin(const user *User) {

   return UserLevel(User) >= GroupLevel(SearchGroup("Guest"));
}

/* IsSuperUser() */

int IsSuperUser(const user *User) {

   return SameString(User->Id,SUPER_USER);
}

/* IsUpperUser() */

int IsUpperUser(const user *User1, const user *User2) {

   if (User1 == NULL) {
      Error("IsUpperUser(): User1 = NULL");
      return FALSE;
   } else if (User1->Type != USER) {
      Error("IsUpperUser(): \"%s\" is not a user",User1->Id);
      return FALSE;
   } else if (User2 == NULL) {
      Error("IsUpperUser(): User2 = NULL");
      return FALSE;
   } else if (User2->Type != USER) {
      Error("IsUpperUser(): \"%s\" is not a user",User2->Id);
      return FALSE;
   }

   return IsSuperUser(User1) || (IsAdmin(User1) && UserLevel(User1) > UserLevel(User2));
}

/* IsConnected() */

int IsConnected(const user *User) {

   if (User == NULL) {
      Error("IsConnected(): User = NULL");
      return FALSE;
   } else if (User->Type != USER) {
      Error("IsConnected(): \"%s\" is not a user",User->Id);
      return FALSE;
   }

   return User->State == LOGGED && IsAliveConn(User->Conn);
}

/* CanSee() */

int CanSee(const user *User1, const user *User2) {

   channel *Channel;

   if (User1 == NULL) {
      Error("CanSee(): User1 = NULL");
      return FALSE;
   } else if (User1->Type != USER) {
      Error("CanSee(): \"%s\" is not a user",User1->Id);
      return FALSE;
   } else if (! IsConnected(User1)) {
      Error("CanSee(): \"%s\" is not connected",User1->Id);
      return FALSE;
   } else if (User2 == NULL) {
      Error("CanSee(): User2 = NULL");
      return FALSE;
   } else if (User2->Type != USER) {
      Error("CanSee(): \"%s\" is not a user",User2->Id);
      return FALSE;
   }

   if (! IsConnected(User2)) return FALSE;
   if (SameString(User1->Channel,User2->Channel)) return TRUE;
   Channel = SearchChannel(User2->Channel);
   if (Channel == NULL) {
      Error("Couldn't find channel \"%s\" !",User2->Channel);
      return TRUE;
   }
   if (! Channel->Hidden) return TRUE;

   return UserLevel(User1) >= ChannelLevel(Channel);
}

/* SendUser() */

void SendUser(user *User, const char *Format, ...) {

   time_t   Time;
   va_list  Args;
   char    *Char, *Char2, String[STRING_SIZE], String2[STRING_SIZE], String3[STRING_SIZE];

   if (User->State == NOT_LOGGED || ! IsAliveConn(User->Conn)) return;

   Time = time(NULL);
   strftime(String3,(size_t)15,"%X ",localtime(&Time));

   va_start(Args,Format);
   vsprintf(String,Format,Args);
   va_end(Args);

   AsciiToTelnet(String,String2);
   strcat(String3,String2);

   SendString(User->Conn,(User->Away)?String3:String2);
}

/* SendUsers() */

void SendUsers(const user *Except, const char *Format, ...) {

   time_t   Time;
   va_list  Args;
   char    *Char, *Char2, String[STRING_SIZE], String2[STRING_SIZE], String3[STRING_SIZE];
   node    *Node;
   user    *User;

   Time = time(NULL);
   strftime(String3,(size_t)15,"%X ",localtime(&Time));

   va_start(Args,Format);
   vsprintf(String,Format,Args);
   va_end(Args);

   AsciiToTelnet(String,String2);
   strcat(String3,String2);

   for (Node = UserList->Head; Node != NULL; Node = Node->Next) {
      User = (user *) Node->Object;
      if (IsConnected(User) && User != Except) {
         SendString(User->Conn,(User->Away)?String3:String2);
      }
   }
}

/* SendInOut() */

void SendInOut(const user *Except, const char *Format, ...) {

   time_t   Time;
   va_list  Args;
   char    *Char, *Char2, String[STRING_SIZE], String2[STRING_SIZE], String3[STRING_SIZE];
   node    *Node;
   user    *User;

   Time = time(NULL);
   strftime(String3,(size_t)15,"%X ",localtime(&Time));

   va_start(Args,Format);
   vsprintf(String,Format,Args);
   va_end(Args);

   AsciiToTelnet(String,String2);
   strcat(String3,String2);

   for (Node = UserList->Head; Node != NULL; Node = Node->Next) {
      User = (user *) Node->Object;
      if (IsConnected(User) && User != Except && User->InOut) {
         SendString(User->Conn,(User->Away)?String3:String2);
      }
   }
}

/* SendShout() */

void SendShout(const user *Except, const char *Format, ...) {

   time_t   Time;
   va_list  Args;
   char    *Char, *Char2, String[STRING_SIZE], String2[STRING_SIZE], String3[STRING_SIZE];
   node    *Node;
   user    *User;

   Time = time(NULL);
   strftime(String3,(size_t)15,"%X ",localtime(&Time));

   va_start(Args,Format);
   vsprintf(String,Format,Args);
   va_end(Args);

   AsciiToTelnet(String,String2);
   strcat(String3,String2);

   for (Node = UserList->Head; Node != NULL; Node = Node->Next) {
      User = (user *) Node->Object;
      if (IsConnected(User) && User != Except && User->Shout) {
         SendString(User->Conn,(User->Away)?String3:String2);
      }
   }
}

/* FileExists() */

int FileExists(const char *FileName) {

   FILE *File;

   File = fopen(ToLower(FileName),"r");
   if (File != NULL) {
      fclose(File);
      return TRUE;
   }

   return FALSE;
}

/* DeleteFile() */

void DeleteFile(const char *FileName) {

   remove(ToLower(FileName));
}

/* AppendFile() */

void AppendFile(const char *FileName, const char *Message, ...) {

   char     TimeString[32];
   time_t   Time;
   FILE    *File;
   va_list  Args;

   File = fopen(ToLower(FileName),"a");
   if (File == NULL) return;

   Time = time(NULL);
   strftime(TimeString,(size_t)31,"%x %X ",localtime(&Time));

   fprintf(File,"%s",TimeString);

   va_start(Args,Message);
   vfprintf(File,Message,Args);
   va_end(Args);

   fprintf(File,"\n");

   fclose(File);
}

/* DeleteLines() */

void DeleteLines(const char *FileName, int FirstLine, int LastLine) {

   int LineNb;
   int Copied;
   FILE *TmpFile;
   FILE *File;
   char Line[STRING_SIZE];
   
   /* Open the Tmp file */

   TmpFile = tmpfile();
   if (TmpFile == NULL) {
      Error("DeleteLines(): TmpFile = NULL");
      return;
   }

   /* Copy File -> Tmp */

   File = fopen(ToLower(FileName),"r");
   if (File == NULL) {
      Error("DeleteLines(): Couldn't open file \"%s\" for reading",FileName);
      fclose(TmpFile);
      return;
   }
   
   Copied = FALSE;
   for (LineNb = 1; fgets(Line,STRING_SIZE,File) != NULL; LineNb++) {
      if (LineNb < FirstLine || LineNb > LastLine) {
         fputs(Line,TmpFile);
         Copied = TRUE;
      }
   }

   fclose(File);
   
   if (! Copied) {
      fclose(TmpFile);
      remove(ToLower(FileName));
      return;
   }

   /* Return to the begining of TmpFile */

   rewind(TmpFile);
   
   /* Copy Tmp -> File */

   File = fopen(ToLower(FileName),"w");
   if (File == NULL) {
      Error("DeleteLines(): Couldn't open file \"%s\" for writing",FileName);
      fclose(TmpFile);
      return;
   }
   
   while (fgets(Line,STRING_SIZE,TmpFile) != NULL) fputs(Line,File);

   fclose(File);

   /* Close the Tmp file */

   fclose(TmpFile);
}

/* SendFile() */

int SendFile(user *User, const char *FileName) {

   FILE *File;
   int   LineNb;
   char  Line[STRING_SIZE];

   File = fopen(ToLower(FileName),"r");
   if (File == NULL) return 0;
   for (LineNb = 0; fgets(Line,STRING_SIZE,File) != NULL; LineNb++) SendUser(User,"%s",Line);
   fclose(File);

   return LineNb;
}

/* SendFileWithLineNb() */

int SendFileWithLineNb(user *User, const char *FileName) {

   FILE *File;
   int   LineNb;
   char  Line[STRING_SIZE];

   File = fopen(ToLower(FileName),"r");
   if (File == NULL) return 0;
   for (LineNb = 0; fgets(Line,STRING_SIZE,File) != NULL; LineNb++) {
      SendUser(User,"%2d %s",LineNb+1,Line);
   }
   fclose(File);

   return LineNb;
}

/* SendHelpFile() */

int SendHelpFile(user *User, const char *FileName) {

   return FindFile(User,FileName,"help");
}

/* FindFile() */

static int FindFile(user *User, const char *FileName, const char *DirName) {

   int N, I, Found;
   struct stat Stat[1];
   DIR *Dir;
   struct dirent *DirEntry;
   char Line[STRING_SIZE], Cwd[STRING_SIZE], Entry[100][NAME_SIZE+1];

   Found = FALSE;

   if (getcwd(Cwd,STRING_SIZE) == NULL) {
      Error("Counldn't get current work dir");
      return Found;
   }

   Dir = opendir(DirName);
   if (Dir == NULL) {
      Error("Couldn't open directory \"%s/%s\"",Cwd,DirName);
      return Found;
   }
   N = 0;
   while (N < 100 && (DirEntry = readdir(Dir)) != NULL) {
      if (DirEntry->d_name[0] != '.') {
	 strncpy(Entry[N],DirEntry->d_name,NAME_SIZE+1);
	 N++;
      }
   }
   closedir(Dir);
   qsort(Entry,(size_t)N,(size_t)(NAME_SIZE+1),(CMP_FCT)strcmp);

   if (SameString(DirName,FileName)) {
      for (I = 0; I < N; I++) SendUser(User,"%s\n",Entry[I]);
      Found = TRUE;
   }

   if (chdir(DirName) == -1) {
      if (getcwd(Line,STRING_SIZE) != NULL) {
         Error("Couldn't cd from \"%s\" to \"%s\"",Line,DirName);
      } else {
         Error("Couldn't cd from to \"%s\"",DirName);
      }
      return Found;
   }

   for (I = 0; I < N; I++) {

      if (stat(Entry[I],Stat) == -1) {
         Error("Couldn't open file \"%s\"",Entry[I]);
	 continue;
      }

      if (S_ISREG(Stat->st_mode)) {
	 if (SameString(Entry[I],FileName) && (Stat->st_mode & (S_IRGRP | S_IROTH)) == (S_IRGRP | S_IROTH)) {
	    Found = SendFile(User,Entry[I]) != 0;
         }
      } else if (S_ISDIR(Stat->st_mode)) {
	 if (FindFile(User,FileName,Entry[I])) Found = TRUE;
      }
   }

   if (chdir(Cwd) == -1) {
      if (getcwd(Line,STRING_SIZE) != NULL) {
         Error("Couldn't cd from \"%s\" to \"%s\"",Line,"..");
      } else {
         Error("Couldn't cd from to \"%s\"","..");
      }
      return Found;
   }

   return Found;
}

/* GetBirthday() */

int GetBirthday(const user *User) {

  int cyear, cmon, cday, callday, bmon, bday, ballday, diff;
  struct tm *mytm;
  time_t ct, bt;

  /* unknown birthday */
  if(User->Birthday == (time_t) 0) return -1;

  ct=time(NULL);
  mytm = localtime(&ct);
  cday = mytm->tm_mday;
  cmon = mytm->tm_mon+1;
  cyear = mytm->tm_year;
  callday = mytm->tm_yday;

  mytm = localtime(&User->Birthday);
  mytm->tm_year = cyear;
  
  bt = mktime(mytm);
  mytm = localtime(&bt);
  
  bday = mytm->tm_mday;
  bmon = mytm->tm_mon+1;
  ballday = mytm->tm_yday;
  
  diff = ballday - callday;
  
  if (diff<0) {

    mytm = localtime(&User->Birthday);
    mytm->tm_year = cyear+1;
    
    bt = mktime(mytm);
    mytm = localtime(&bt);
    
    bday = mytm->tm_mday;
    bmon = mytm->tm_mon+1;
    ballday = mytm->tm_yday;
    
    diff = difftime(bt,ct)/86400;
  }
  return diff;
}

/* GetCalendar() */

void GetCalendar(const char *FileName, int day, int time, char *Result, int ResultSize) {

  FILE *fp = fopen(ToLower(FileName), "r");
  char fline[512];
  int resday, restimemin, restimemax;

  if (ResultSize<1)
  	return;

  Result[0] = '\0';

  if (fp==NULL) {
    strncpy(Result, "There's no calendar for him/her", ResultSize);
    return;
  }

  while (!feof(fp)) {
    GetLine(fp, fline, 512, &resday, &restimemin, &restimemax);
    if (resday == day && time>=restimemin && time<=restimemax) {
      strncpy(Result, fline, ResultSize);
      fclose(fp);
      return;
    }
  }
  strncpy(Result, "He could be anywhere", ResultSize);
 
  fclose(fp);
}

/* GetCalendarNow() */

void GetCalendarNow(const char *FileName, char *Result, int ResultSize) {

  time_t ltime;
  struct tm *today;

  time(&ltime);
  today = localtime(&ltime);

  GetCalendar(FileName, today->tm_wday, today->tm_hour*10000+today->tm_min*100, Result, ResultSize);
}

/* ConvertStringToTime() */

static int ConvertStringToTime(char *TimeString) {

  int i=0, time[3], pos=0;

  /* TODO: gerer les heures non valides (retourner 0 ou alors saturer...) */
  
  time[0]=time[1]=time[2]=0;

  if (TimeString == NULL) return 0;

  while (TimeString[i]==' ' || TimeString[i]=='\t')
    i++;

  if (TimeString[i]=='\0') return 0;

  while (isdigit(TimeString[i]) || TimeString[i]==':') {
    if (TimeString[i]==':') {
      pos++;
    } else {
      time[pos] = time[pos]*10+(TimeString[i]-'0');
    }
    i++;
  }

  return time[0]*10000 + time[1]*100 + time[2];
}

/* GetLine() */

#define SKIP_SPACE while (buffer[i] == ' ' || buffer[i] == '\t') i++

static void GetLine(FILE *fp, char *buffer, int buffersize, int *day, int *timemin, int *timemax) {

  char *daytab[] = { "SU", "MO", "TU", "WE", "TH" , "FR", "SA" };
  int c, i=0, d;

  if (buffersize<1)
    return;

  c = fgetc(fp);
  for (i=0; c!='\n' && (i<buffersize) && (feof(fp)==0); i++) {
    buffer[i] = (char)c;
    c = fgetc(fp);
  }
  buffer[i] = '\0';

  /* parse the string */
  i=0;

  /* eject space */
  SKIP_SPACE;

  if (buffer[i]=='\0' || buffer[i+1]=='\0') return;
  
  /* get the day */
  for (d=0; d<sizeof(daytab)/sizeof(daytab[0]); d++) {
    if (daytab[d][0] == toupper(buffer[i]) && daytab[d][1] == toupper(buffer[i+1])) {
      *day = d;
      break;
    }
  }
  i+=2;

  SKIP_SPACE;
  *timemin = ConvertStringToTime(&buffer[i]);
  while (isdigit(buffer[i]) || buffer[i]==':') i++;

  SKIP_SPACE;
  *timemax = ConvertStringToTime(&buffer[i]);
  while (isdigit(buffer[i]) || buffer[i]==':') i++;
}

/* End of User.C */
