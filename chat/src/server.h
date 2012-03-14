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

/* Server.H */

#ifndef SERVER_H
#define SERVER_H

#include <time.h>

#include "user.h"

/* Constants */

#define PORT_NUM  4000

#define NAME_SIZE 64
#define LINE_SIZE 256
#define NO_SOCKET (-1)

/* Types */

typedef struct {
   int    Size;
   char **String;
} history;

/* Variables */

extern time_t  RebootTime;
extern int     UserNb;
extern int     UserNbMax;
extern int     RegOnly;
extern history LogHistory[1];
extern history WallHistory[1];

/* Prototypes */

extern void Logout      (user *User, const user *Except, const char *Message, ...);

extern void InitHistory (history *History, int Size);
extern void LoadHistory (const char *FileName, history *History);
extern void AddHistory  (history *History, const char *Message, ...);
extern void SendHistory (user *User, const char *HistoryName, const history *History);
extern void ClearHistory(history *History, int Size);
#endif /* ! defined SERVER_H */

/* End of Server.H */
