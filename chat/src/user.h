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

/* User.H */

#ifndef USER_H
#define USER_H

#include <time.h>

#include "object.h"
#include "database.h"
#include "list.h"
#include "socket.h"

/* Constants */

#define DEFAULT_USER "Nobody"
#define SUPER_USER   "Root"

#define PLAN_SIZE    5 /* 5 lines for the .plan */

enum { NOT_LOGGED, HOST, LOGIN, PASSWORD, LOGGED }; /* User State field */

/* Types */

typedef struct {
   int     Type;
   char   *Id;
   char    Group[ID_SIZE+1];
   char    Password[ENCRYPTED_SIZE+1];
   char    Channel[ID_SIZE+1];
   char   *Name;
   char   *EMail;
   char   *Formation;
   char   *Host;
   char   *Plan[PLAN_SIZE];
   char    Tell[ID_SIZE+1];
   char    Invite[ID_SIZE+1];
   int     Language;
   int     Registered;
   time_t  RegisterTime;
   time_t  ConnectTime;
   time_t  Birthday;
   int     dbbn; /* Day before birthday number. You must not use this number */
   int     KickNb;
   int     KickedNb;
   time_t  LastSendTime;
   int     LoginNb;
   int     TotalTime;
   char   *LastLoginHost;
   time_t  LastLoginTime;
   int     FailureNb;
   char   *LastFailHost;
   time_t  LastFailTime;
   int     State;
   int     Away;
   int     Bell;
   char   *Client;
   int     InOut;
   int     Join;
   int     Shout;
   list    Aliases[1];
   conn   *Conn;
} user;

typedef struct {
   int   Type;
   char *Id;
   char *Command;
} alias;

/* Variables */

extern list UserList[1];

/* Prototypes */

extern user  *NewUser            (const char *Id);
extern user  *NewUId             (const char *Id, const char *Group, const char *Password, const char *Name, const char *EMail, const char *Formation, time_t RegisterTime, time_t ConnectTime, time_t Birthday, int KickNb, int KickedNb, int LoginNb, int TotalTime, const char *LastLoginHost, time_t LastLoginTime, int FailureNb, const char *LastFailHost, time_t LastFailTime);
extern alias *NewAlias           (const char *Word, const char *String);

extern void   InitUser           (user *User, const char *Id);
extern void   InitUId            (user *User);

extern void   DeleteUser         (user *User);
extern void   DeleteAlias        (alias *Alias);

extern user  *SearchUser         (const char *Id);
extern user  *SearchUId          (const char *Id);
extern alias *SearchAlias        (list *List, const char *Id);

extern int    UserLevel          (const user *User);
extern int    IsAdmin            (const user *User);
extern int    IsSuperUser        (const user *User);
extern int    IsUpperUser        (const user *User1, const user *User2);

extern int    IsConnected        (const user *User);
extern int    CanSee             (const user *User1, const user *User2);

extern void   SendUser           (user *User, const char *Format, ...);
extern void   SendUsers          (const user *Except, const char *Format, ...);
extern void   SendInOut          (const user *Except, const char *Format, ...);
extern void   SendShout          (const user *Except, const char *Format, ...);

extern int    FileExists         (const char *FileName);
extern void   DeleteFile         (const char *FileName);
extern void   AppendFile         (const char *FileName, const char *Message, ...);
extern void   DeleteLines        (const char *FileName, int FirstLine, int LastLine);

extern int    SendFile           (user *User, const char *FileName);
extern int    SendFileWithLineNb (user *User, const char *FileName);
extern int    SendHelpFile       (user *User, const char *FileName);

extern int    GetBirthday        (const user *User);

extern void   GetCalendar        (const char *FileName, int day, int time, char *Result, int ResultSize);
extern void   GetCalendarNow     (const char *FileName, char *Result, int ResultSize);

#endif /* ! defined USER_H */

/* End of User.H */
