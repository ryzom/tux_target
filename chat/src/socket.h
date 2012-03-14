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

/* Socket.H */

#ifndef SOCKET_H
#define SOCKET_H

#include <time.h>

/* Constants */

#define NAME_SIZE   64
#define LINE_SIZE   256
#define BUFFER_SIZE 65536

/* Types */

typedef struct {
   time_t Start;
   int    InOpNb;
   double InByteNb;
   int    InByteMax;
   int    InBufSize;
   int    OutOpNb;
   double OutByteNb;
   int    OutByteMax;
   int    OutBufSize;
} debug;

typedef struct {
   size_t Pos;
   int    Line;
   int    Mode;
   char   Buffer[BUFFER_SIZE];
} buffer;

typedef struct {
   void  *Object;
   int    Alive;
   int    Socket;
   const char *Error;
   debug  Debug[1];
   buffer In[1];
   buffer Out[1];
} conn;

/* Variables */

extern debug MtpDebug[1];

/* Prototypes */

extern int   InitSocket    (void);
extern int   InstallSocket (int Port);

extern void  UpdateConn    (void);

extern char *GetString     (conn *Conn);
extern int   SendString    (conn *Conn, const char *String);

extern int   IsAliveConn   (const conn *Conn);
extern int   CloseConn     (conn *Conn);

extern int   IsAllowedConn (const char *HostAddr, const char *HostName);
extern void  AddConn       (conn *Conn, const char *HostAddr, const char *HostName);
extern void  RemConn       (const conn *Conn, void *Object);

extern int   SetConnObject (conn *Conn, void *Object);

extern void AsciiToTelnet  (const char Src[], char Dst[]);

#endif /* ! defined SOCKET_H */

/* End of Socket.H */
