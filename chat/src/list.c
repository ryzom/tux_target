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

/* List.C */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "list.h"
#include "types.h"
#include "object.h"
#include "mtp.h"
#include "server.h"
#include "token.h"
#include "variable.h"

/* Variables */

list Lists[1];

/* Prototypes */

static void RemNode(list *List, node *Node);

/* Functions */

/* InitList() */

void InitList(list *List, const char *ListName) {

   List->Type = LIST;
   List->Id   = NewString(ListName);
   List->Head = NULL;
   List->Tail = NULL;
}

/* ClearList() */

void ClearList(list *List) {

   node *Node, *NodeNext;

   for (Node = List->Head; Node != NULL; Node = NodeNext) {
      NodeNext = Node->Next;
      free(Node);
   }

   List->Head = NULL;
   List->Tail = NULL;
}

/* AddHead() */

void AddHead(list *List, void *Object) {

   node *Node;

   Node = malloc(sizeof(node));
   if (Node == NULL) FatalError("Not enough memory for Node in AddHead()");

   Node->Prev   = NULL;
   Node->Next   = List->Head;
   Node->Object = Object;

   if (List->Head != NULL) {
      List->Head->Prev = Node;
   } else {
      List->Tail = Node;
   }
   List->Head = Node;
}

/* AddTail() */

void AddTail(list *List, void *Object) {

   node *Node;

   Node = malloc(sizeof(node));
   if (Node == NULL) FatalError("Not enough memory for Node in AddTail()");

   Node->Prev   = List->Tail;
   Node->Next   = NULL;
   Node->Object = Object;

   if (List->Tail != NULL) {
      List->Tail->Next = Node;
   } else {
      List->Head = Node;
   }
   List->Tail = Node;
}

/* RemObject() */

void RemObject(list *List, void *Object) {

   node *Node;

   for (Node = List->Head; Node != NULL; Node = Node->Next) {
      if (Node->Object == Object) {
         RemNode(List,Node);
         return;
      }
   }
}

/* RemNode() */

static void RemNode(list *List, node *Node) {

   if (Node->Prev != NULL) {
      Node->Prev->Next = Node->Next;
   } else {
      List->Head = Node->Next;
   }

   if (Node->Next != NULL) {
      Node->Next->Prev = Node->Prev;
   } else {
      List->Tail = Node->Prev;
   }

   free(Node);
}

/* SearchObject() */

void *SearchObject(list *List, const char *Name, int Type) {

   node   *Node;
   object *Object;

   for (Node = List->Head; Node != NULL; Node = Node->Next) {
      Object = Node->Object;
      if (SameString(Object->Id,Name) && (Object->Type & Type) != 0) return Object;
      if (Object->Type == LIST) {
         Object = SearchObject((list*)Object,Name,Type);
         if (Object != NULL) return Object;
      }
   }

   return NULL;
}

/* SortList() */

void SortList(list *List, CMP_FCT NodeCmp) {

   list  NewList[1];
   node *Greatest, *Ptr;
  
   InitList(NewList,List->Id);

   if (List->Head != List->Tail) {
      while (List->Head != NULL) {
         Greatest = List->Head;  
         for (Ptr = Greatest->Next; Ptr != NULL; Ptr = Ptr->Next) {
            if ((*NodeCmp)(Ptr->Object,Greatest->Object) > 0) Greatest = Ptr;
         }
         AddHead(NewList,Greatest->Object);
         RemNode(List,Greatest);
      }
      *List = *NewList;
   }
}

/* SortList2() */

list *SortList2(list *List, CMP_FCT NodeCmp) {
   static list NewList[1], SortList[1];
   node *Node, *Greatest, *Ptr;
  
   InitList(NewList,"SortPointerList");

   /* fait une liste de pointeur sur la liste */
   InitList(NewList,"PointerList");
   for (Node = List->Head; Node != NULL; Node = Node->Next) {
      AddHead(NewList, Node->Object);
   }

   if (NewList->Head != NewList->Tail) {
      while (NewList->Head != NULL) {
         Greatest = NewList->Head;
         for (Ptr = Greatest->Next; Ptr != NULL; Ptr = Ptr->Next) {
            if ((*NodeCmp)(Ptr->Object,Greatest->Object) > 0) Greatest = Ptr;
         }
	 AddHead(SortList,Greatest->Object);
         RemNode(NewList,Greatest);
      }
      return SortList;
   }
   return NewList;
}

/* End of List.C */
