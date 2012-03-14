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

/* Group.H */

#ifndef GROUP_H
#define GROUP_H

#include "object.h"
#include "list.h"

/* Constants */

#define DEFAULT_GROUP  "NoGroup"
#define REGISTER_GROUP "Register"

/* Types */

typedef struct {
   int   Type;
   char *Id;
   char  Leader[ID_SIZE+1];
   int   Level;
   char *Name;
} group;

/* Variables */

extern list GroupList[1];

/* Prototypes */

extern group *NewGroup    (const char *Id, const char *Leader, int Level, const char *Name);
extern void   DeleteGroup (group *Group);
extern group *SearchGroup (const char *Id);
extern int    GroupLevel  (const group *Group);

#endif /* ! defined GROUP_H */

/* End of Group.H */
