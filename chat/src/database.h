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

/* DataBase.H */

#ifndef DATABASE_H
#define DATABASE_H

/* Constants */

#define ENCRYPTED_SIZE 13

/* Variables */

extern int DataBaseChanged;

/* Prototypes */

extern int   ReadDataBase  (void);
extern int   WriteDataBase (void);

extern char *CryptPassword (const char *Password);
extern int   CheckPassword (const char *String, const char *Encrypted);

#endif /* ! defined DATABASE_H */

/* End of DataBase.H */
