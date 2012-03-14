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

/* MTP.H */

#ifndef MTP_H
#define MTP_H

/* Constants */

#define BANNED_LOG   "banned"
#define COMMAND_LOG  "command"
#define DENIED_LOG   "denied"
#define ERROR_LOG    "error"
#define HOST_LOG     "host"
#define INOUT_LOG    "inout"
#define PASSWORD_LOG "password"
#define SECURITY_LOG "security"
#define SIGNAL_LOG   "signal"
#define SOCKET_LOG   "socket"
#define TELNET_LOG   "telnet"
#define WALL_LOG     "wall"

/* Prototypes */

extern void Exit       (void);

extern void Warning    (const char *Message, ...);
extern void Error      (const char *Message, ...);
extern void FatalError (const char *Message, ...);
extern void Trace      (const char *LogFile, const char *Message, ...);

#endif /* ! defined MTP_H */

/* End of MTP.H */
