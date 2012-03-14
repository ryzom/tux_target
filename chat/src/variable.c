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

/* Variable.C */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "variable.h"
#include "types.h"
#include "mtp.h"
#include "token.h"

/* Functions */

/* Boolean() */

int Boolean(const char *Value) {

   if (Value == NULL) return FALSE;

   return (SameString(Value,"True") || SameString(Value,"On") || SameString(Value,"Yes") || SameString(Value,"1"));
}

/* Date() */

int StringToDate(char *Value, time_t *Result) {

   struct tm mytm;
   const char *Token;
   int intval;

   if (Value == NULL) return FALSE;

   mytm.tm_hour = 12;
   mytm.tm_min  = 0;
   mytm.tm_sec  = 0;

   Token = strtok(Value,"/\n");
   if(Token == NULL) return FALSE;
   intval = atoi(Token)-1;
   if(intval<0 || intval>11) return FALSE;
   mytm.tm_mon = intval;

   Token = strtok(NULL,"/\n");
   if(Token == NULL) return FALSE;
   intval = atoi(Token);
   if(intval<1 || intval>31) return FALSE;
   mytm.tm_mday  = intval;

   Token = strtok(NULL,"/\n");
   if(Token == NULL) return FALSE;
   intval = atoi(Token);
   mytm.tm_year  = intval;

   *Result = mktime(&mytm);
   return (*Result != (time_t)-1);
}

/* NewString() */

char *NewString(const char *String) {

   char *NewString;

   if (String == NULL) String = "";

   NewString = malloc(strlen(String)+1);
   if (NewString == NULL) {
      Error("NewString(): not enough memory for \"%s\"",String);
   } else {
      strcpy(NewString,String);
   }

   return NewString;
}

/* SetString() */

int SetString(char **Var, const char *Value) {

   if (*Var != NULL) free(*Var);
   *Var = NewString(Value);

   return TRUE;
}

/* DeleteString() */

int DeleteString(char **Var) {

   if (*Var != NULL) free(*Var);
   *Var = NULL;

   return TRUE;
}

/* End of Variable.C */
