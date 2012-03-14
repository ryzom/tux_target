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

/* Variable.H */

#ifndef VARIABLE_H
#define VARIABLE_H

#include <time.h>

/* Prototypes */

extern int   Boolean      (const char *Value);
extern int   StringToDate (char *Value, time_t *Result);
extern char *NewString    (const char *String);
extern int   SetString    (char **Var, const char *Value);
extern int   DeleteString (char **Var);

#endif /* ! defined VARIABLE_H */

/* End of Variable.H */
