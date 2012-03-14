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

/* Token.C */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "types.h"
#include "object.h"
#include "server.h"

/* Functions */

/* IsNumber() */

int IsNumber(const char *String) {

   if (String == NULL) return FALSE;

   do {
      if (! isdigit(*String++)) return FALSE;
   } while (*String != '\0');

   return TRUE;
}

/* IsId() */

int IsId(const char *String) {

   if (String == NULL || strlen(String) > ID_SIZE) return FALSE;

   if (! isalpha(*String++)) return FALSE;

   while (*String != '\0') {
      if (! isalnum(*String++)) return FALSE;
   }

   return TRUE;
}

/* IsPassword() */

int IsPassword(const char *String) {

   if (String == NULL || strlen(String) > PASSWORD_SIZE) return FALSE;

   while (*String != '\0') {
      if (! isprint(*String++)) return FALSE;
   }

   return TRUE;
}

/* IsFile() */

int IsFile(const char *String) {

   if (String == NULL || strlen(String) > NAME_SIZE) return FALSE;

   do {
      if (! isalnum(*String++)) return FALSE;
   } while (*String != '\0');

   return TRUE;
}

/* IsWord() */

int IsWord(const char *String) {

   if (String == NULL) return FALSE;

   do {
      if (! isgraph(*String++)) return FALSE;
   } while (*String != '\0');

   return TRUE;
}

/* IsString() */

int IsString(const char *String) {

   if (String == NULL) return FALSE;

   do {
      if (! isprint(*String++)) return FALSE;
   } while (*String != '\0');

   return TRUE;
}

/* IsEmptyString() */

int IsEmptyString(const char *String) {

   return String == NULL || String[0] == '\0';
}

/* SameChar() */

int SameChar(const char Char1, const char Char2) {

   return toupper(Char1) == toupper(Char2);
}

/* SameString() */

int SameString(const char *String1, const char *String2) {

   while (toupper(*String1) == toupper(*String2)) {
      if (*String1 == '\0') return TRUE;
      String1++;
      String2++;
   }

   return FALSE;
}

/* StartWith() */

int StartWith(const char *String1, const char *String2) {

   while (toupper(*String1) == toupper(*String2)) {
      if (*String1 == '\0') return TRUE;
      String1++;
      String2++;
   }

   return *String2 == '\0';
}

/* Contain() */

int Contain(const char *String1, const char *String2) {

   return strstr(String1,String2) != NULL;
}

/* ToLower() */

char *ToLower(const char *String) {

   int I;
   static char ToLower[STRING_SIZE];

   for (I = 0; I < STRING_SIZE-1 && String[I] != '\0'; I++) ToLower[I] = tolower(String[I]);
   ToLower[I] = '\0';

   return ToLower;
}

/* End of Token.C */
