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

/* List.H */

#ifndef LIST_H
#define LIST_H

#include "object.h"

/* Types */

typedef struct Node {
   struct Node *Next;
   struct Node *Prev;
   object      *Object;
} node;

typedef struct {
   int   Type;
   char *Id;
   node *Head;
   node *Tail;
} list;

typedef int (*CMP_FCT)(const void *Node1, const void *Node2);

/* Variables */

extern list Lists[1];

/* Prototypes */

extern void  InitList     (list *List, const char *ListName);
extern void  ClearList    (list *List);

extern void  AddHead      (list *List, void *Object);
extern void  AddTail      (list *List, void *Object);
extern void  RemObject    (list *List, void *Object);

extern void *SearchObject (list *List, const char *Name, int Type);
extern void  SortList     (list *List, CMP_FCT NodeCmp);
extern list *SortList2    (list *List, CMP_FCT NodeCmp);

#endif /* ! defined LIST_H */

/* End of List.H */
