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

/* Token.H */

#ifndef TOKEN_H
#define TOKEN_H

#include "object.h"

/* Constants */

/*
enum { TK_NUMBER, TK_ID, TK_FILE, TK_WORD, TK_STRING };
*/

#define PASSWORD_SIZE 8

/* Types */

typedef struct {
   int     Type;
   char   *String;
   object *Object;
   int     Value;
} arg;

/* Prototypes */

extern int   IsNumber      (const char *String);
extern int   IsId          (const char *String);
extern int   IsPassword    (const char *String);
extern int   IsFile        (const char *String);
extern int   IsWord        (const char *String);
extern int   IsString      (const char *String);

extern int   IsEmptyString (const char *String);
extern int   SameChar      (char Char1, char Char2);
extern int   SameString    (const char *String1, const char *String2);
extern int   StartWith     (const char *String1, const char *String2);
extern int   Contain       (const char *String1, const char *String2);

extern char *ToLower       (const char *String);

#endif /* ! defined TOKEN_H */

/* End of Token.H */
