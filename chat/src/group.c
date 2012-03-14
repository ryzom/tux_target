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

/* Group.C */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#include "group.h"
#include "types.h"
#include "list.h"
#include "object.h"
#include "mtp.h"
#include "server.h"
#include "token.h"
#include "user.h"
#include "variable.h"

/* Variables */

list GroupList[1];

/* Functions */

/* NewGroup() */

group *NewGroup(const char *Id, const char *Leader, int Level, const char *Name) {

   group *Group;

   if (Id == NULL) {
      Error("NewGroup(): Id = NULL");
      return NULL;
   } else if (! IsId(Id)) {
      Error("NewGroup(): Id = \"%s\" is not an ID",Id);
      return NULL;
   } else if (SearchObject(Lists,Id,OBJECT) != NULL) {
      Error("NewGroup(): Id = \"%s\" is already used",Id);
      return NULL;
   } else if ((Group = calloc(1,sizeof(group))) == NULL) {
      Error("NewGroup(): not enough memory for group \"%s\"",Id);
      return NULL;
   }

   if (Leader == NULL || ! IsId(Leader)) {
      Warning("\"%s\" has no leader",Id);
      Leader = SUPER_USER;
   }
   if (Name == NULL) Name = "";

   Group->Type  = GROUP;
   Group->Id    = NewString(Id);
   strcpy(Group->Leader,Leader);
   Group->Level = Level;
   Group->Name  = NewString(Name);

   return Group;
}

/* DeleteGroup() */

void DeleteGroup(group *Group) {

   if (Group == NULL) {
      Error("DeleteGroup(): Group = NULL");
      return;
   } else if (Group->Type != GROUP) {
      Error("DeleteGroup(): \"%s\" is not a group",Group->Id);
      return;
   }

   DeleteString(&Group->Id);
   DeleteString(&Group->Name);

   free(Group);
}

/* SearchGroup() */

group *SearchGroup(const char *Id) {

   if (Id == NULL) {
      Error("SearchGroup(): Id = NULL");
      return NULL;
   } else if (! IsId(Id)) {
      Error("SearchGroup(): Id = \"%s\" is not an ID",Id);
      return NULL;
   }

   return (group *) SearchObject(GroupList,Id,GROUP);
}

/* GroupLevel() */

int GroupLevel(const group *Group) {

   if (Group == NULL) {
      Error("GroupLevel(): Group = NULL");
      return 0;
   } else if (Group->Type != GROUP) {
      Error("GroupLevel(): \"%s\" is not a group",Group->Id);
      return 0;
   }

   return Group->Level;
}

/* End of Group.C */
