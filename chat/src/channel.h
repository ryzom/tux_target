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

/* Channel.H */

#ifndef CHANNEL_H
#define CHANNEL_H

#include "object.h"
#include "list.h"
#include "user.h"

/* Constants */

#define DEFAULT_CHANNEL "Hall"

/* Types */

typedef struct {
   int   Type;
   char *Id;
   char  Owner[ID_SIZE+1];
   char  Group[ID_SIZE+1];
   char *Topic;
   int   Closed;
   int   Hidden;
   int   Invite;
   int   Protected;
   int   Resident;
   int   UserNb;
} channel;

/* Variables */

extern list ChannelList[1];

/* Prototypes */

extern channel *NewChannel    (const char *ChannelName);
extern void     DeleteChannel (channel *Channel);
extern channel *SearchChannel (const char *ChannelName);
extern int      ChannelLevel  (const channel *Channel);

extern int      JoinChannel   (user *User, channel *Channel);
extern int      LeaveChannel  (user *User);
extern void     SendChannel   (user *User, const char *Format, ...);
extern void     SendJoin      (user *User, const char *Format, ...);

#endif /* ! defined CHANNEL_H */

/* End of Channel.H */
