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

/* Channel.C */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#include "channel.h"
#include "types.h"
#include "group.h"
#include "list.h"
#include "mtp.h"
#include "object.h"
#include "token.h"
#include "user.h"
#include "variable.h"

/* Variables */

list ChannelList[1];

/* Functions */

/* NewChannel() */

channel *NewChannel(const char *Id) {

   channel *Channel;

   if (Id == NULL) {
      Error("NewChannel(): Id = NULL");
      return NULL;
   } else if (! IsId(Id)) {
      Error("NewChannel(): Id = \"%s\" is not an ID",Id);
      return NULL;
   } else if (SearchObject(Lists,Id,OBJECT) != NULL) {
      Error("NewChannel(): Id = \"%s\" is already used",Id);
      return NULL;
   } else if ((Channel = calloc(1,sizeof(channel))) == NULL) {
      Error("NewChannel(): not enough memory for channel \"%s\"",Id);
      return NULL;
   }

   Channel->Type = CHANNEL;
   Channel->Id   = NewString(Id);
   strcpy(Channel->Owner,DEFAULT_USER);
   strcpy(Channel->Group,DEFAULT_GROUP);
   Channel->Topic     = NewString("");
   Channel->UserNb    = 0;
   Channel->Closed    = FALSE;
   Channel->Hidden    = FALSE;
   Channel->Invite    = FALSE;
   Channel->Protected = FALSE;
   Channel->Resident  = FALSE;

   return Channel;
}

/* DeleteChannel() */

void DeleteChannel(channel *Channel) {

   if (Channel == NULL) {
      Error("DeleteChannel(): Channel = NULL");
      return;
   } else if (Channel->Type != CHANNEL) {
      Error("DeleteChannel(): \"%s\" is not a channel",Channel->Id);
      return;
   }

   DeleteString(&Channel->Id);
   DeleteString(&Channel->Topic);

   free(Channel);
}

/* SearchChannel() */

channel *SearchChannel(const char *Id) {

   if (Id == NULL) {
      Error("SearchChannel(): Id = NULL");
      return NULL;
   } else if (! IsId(Id)) {
      Error("SearchChannel(): Id = \"%s\" is not an ID",Id);
      return NULL;
   }

   return (channel *) SearchObject(ChannelList,Id,CHANNEL);
}

/* ChannelLevel() */

int ChannelLevel(const channel *Channel) {

   if (Channel == NULL) {
      Error("ChannelLevel(): Channel = NULL");
      return 0;
   } else if (Channel->Type != CHANNEL) {
      Error("ChannelLevel(): \"%s\" is not a channel",Channel->Id);
      return 0;
   }

   return GroupLevel(SearchGroup(Channel->Group));
}

/* JoinChannel() */

int JoinChannel(user *User, channel *Channel) {

   if (User == NULL) {
      Error("JoinChannel(): User = NULL");
      return FALSE;
   } else if (Channel == NULL) {
      Error("JoinChannel(): Channel = NULL");
      return FALSE;
   } else if (User->Type != USER) {
      Error("JoinChannel(): \"%s\" couldn't join \"%s\" (not a user)",User->Id,Channel->Id);
      return FALSE;
   } else if (Channel->Type != CHANNEL) {
      Error("JoinChannel(): \"%s\" couldn't join \"%s\" (not a channel)",User->Id,Channel->Id);
      return FALSE;
   }

   if (User->Channel[0] != '\0') LeaveChannel(User);
   strcpy(User->Channel,Channel->Id);
   Channel->UserNb++;

   if (Channel->Topic[0] != '\0') SendUser(User,"<Mtp> %s topic : %s\n",Channel->Id,Channel->Topic);

   strcpy(User->Invite,"");

   return TRUE;
}

/* LeaveChannel() */

int LeaveChannel(user *User) {

   channel *Channel;

   if (User == NULL) {
      Error("LeaveChannel(): User = NULL");
      return FALSE;
   } else if (User->Type != USER) {
      Error("LeaveChannel(): \"%s\" couldn't leave \"%s\" (not a user)",User->Id,User->Channel);
      return FALSE;
   }

   Channel = SearchChannel(User->Channel);
   if (Channel == NULL) {
      Error("LeaveChannel(): \"%s\" couldn't leave \"%s\" (unknown channel)",User->Id,User->Channel);
      return FALSE;
   }

   Channel->UserNb--;
   if (Channel->UserNb < 0) {
      Error("LeaveChannel(): After \"%s\" left \"%s\", there were %d users in it",User->Id,User->Channel,Channel->UserNb);
      Channel->UserNb = 0;
   }
   if (Channel->UserNb == 0 && ! Channel->Resident) {
      RemObject(ChannelList,Channel);
      DeleteChannel(Channel);
   }

   strcpy(User->Channel,"");

   return TRUE;
}

/* SendChannel() */

void SendChannel(user *Except, const char *Format, ...) {

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
      if (IsConnected(User) && SameString(User->Channel,Except->Channel) && User != Except) {
         SendString(User->Conn,(User->Away)?String3:String2);
      }
   }
}

/* SendJoin() */

void SendJoin(user *Except, const char *Format, ...) {

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
      if (IsConnected(User) && SameString(User->Channel,Except->Channel) && User->Join && User != Except) {
         SendString(User->Conn,(User->Away)?String3:String2);
      }
   }
}

/* End of Channel.C */
