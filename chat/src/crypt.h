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

/* Crypt.H */

#ifndef CRYPT_H
#define CRYPT_H

/* Constants */

enum {
   LANGUAGE_MIN     = -6,
   LANGUAGE_BAILLON = -6,
   LANGUAGE_VOWEL   = -5,
   LANGUAGE_SHUFFLE = -4,
   LANGUAGE_CRAZY   = -3,
   LANGUAGE_SLEEP   = -2,
   LANGUAGE_NONE    = -1,
   CRYPT_NONE       = 0,
   CRYPT_FULL       = 100,
   LANGUAGE_MAX     = 100 
};

/* Prototypes */

extern char       *Crypt        (int Language, const char *String);
extern char       *DragCrypt    (const char *entree);
extern char       *BlueCrypt    (const char *S);
extern const char *LanguageName (int Language);

#endif /* ! defined CRYPT_H */

/* End of Crypt.H */
