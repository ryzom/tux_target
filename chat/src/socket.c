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

/* Socket.C */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include <arpa/inet.h>
#define TELCMDS
#define TELOPTS
#include <arpa/telnet.h>

#include "socket.h"
#include "types.h"
#include "mtp.h"

/* Constants */

#define NO_SOCKET (-1)

enum { READ, WRITE, READ_WRITE }; /* Socket shutdown */
enum { MD_DATA, MD_IAC, MD_WILL, MD_WONT, MD_DO, MD_DONT }; /* conn Mode field */

/* "Constants" */

static const uchar EchoOn[]     = { IAC, WONT, TELOPT_ECHO, '\0' };
static const uchar EchoOff[]    = { IAC, WILL, TELOPT_ECHO, '\0' };

static const uchar TimingMark[] = { IAC, WILL, TELOPT_TM, '\0' };

/* Types */

typedef struct {
   int Socket;
} ear;

typedef struct node node;

struct node {
   node *Succ;
   node *Pred;
   void *Object;
};

typedef struct {
   node *Head;
   node *Tail;
} list;

/* Variables */

debug MtpDebug[1];

static list ListenList[1], ConnList[1];

/* Prototypes */ 

static int   Accept       (int Socket);
static int   IsAllowed    (const char *HostAddr, const char *HostName);
static int   Connect      (const char *HostName, int Port);

static ear  *NewListen    (int Socket);
static int   DeleteListen (ear *Listen);

static conn *NewConn      (int Socket);
static int   DeleteConn   (conn *Conn);
static int   ReadConn     (conn *Conn);
static int   WriteConn    (conn *Conn);
static int   KillConn     (conn *Conn);

static int   InitDebug    (debug *Debug);

static int   CloseSocket  (int *Socket);

static void  InitList     (list *List);
static void  ClearList    (list *List);
static void  AddTail      (list *List, void *Object);
static void  RemObject    (list *List, void *Object);
static void  RemNode      (list *List, node *Node);

/* Functions */

/* InitSocket() */

int InitSocket(void) {

   InitList(ListenList);
   InitList(ConnList);
   InitDebug(MtpDebug);

   return TRUE;
}

/* InstallSocket() */

int InstallSocket(int Port) {

   int   Socket;
   struct sockaddr_in Address[1];
   int   Length, Option;
   char  HostName[NAME_SIZE+1], DomainName[NAME_SIZE+1];
   ear  *Listen;

   Socket = socket(AF_INET,SOCK_STREAM,0);
   if (Socket == -1) {
      Error("InstallSocket(): socket(): %s",strerror(errno));
      return FALSE;
   }

   if (fcntl(Socket,F_SETFL,O_NONBLOCK) == -1) {
      Error("InstallSocket(): fcntl(O_NONBLOCK): %s",strerror(errno));
   }
   Option = TRUE;
   if (setsockopt(Socket,SOL_SOCKET,SO_REUSEADDR,(char *)&Option,sizeof(Option)) == -1) {
      Error("InstallSocket(): setsockopt(SO_REUSEADDR): %s",strerror(errno));
   }

   memset(Address,0,sizeof(struct sockaddr_in));
   Address->sin_family      = AF_INET;
   Address->sin_addr.s_addr = htonl(INADDR_ANY);
   Address->sin_port        = htons(Port);

   if (bind(Socket,(struct sockaddr *)Address,sizeof(struct sockaddr_in)) == -1) {
      Error("InstallSocket(%d): bind(): %s",Port,strerror(errno));
      CloseSocket(&Socket);
      return FALSE;
   }

   if (listen(Socket,SOMAXCONN) == -1) {
      Error("InstallSocket(): listen(): %s",strerror(errno));
      CloseSocket(&Socket);
      return FALSE;
   }

   Length = sizeof(struct sockaddr_in);
   if (getsockname(Socket,(struct sockaddr *)Address,&Length) == -1) {
      Error("InstallSocket(): getsockname(): %s",strerror(errno));
   }
   if (gethostname(HostName,NAME_SIZE) == -1) {
      Error("InstallSocket(): gethostname(): %s",strerror(errno));
   }
   if (getdomainname(DomainName,NAME_SIZE) == -1) {
      Warning("InstallSocket(): getdomainname(): %s",strerror(errno));
   }

   Listen = NewListen(Socket);
   if (Listen == NULL) {
      Error("InstallSocket(): not enough memory for Listen");
      CloseSocket(&Socket);
      return FALSE;
   }

   AddTail(ListenList,Listen);

   printf("Server socket installed on %s (domain %s), port %d.\n",HostName,DomainName,ntohs(Address->sin_port));

   return TRUE;
}

/* UpdateConn() */

void UpdateConn(void) {

   fd_set ReadSockets[1], WriteSockets[1];
   int    DescMax, Blocking, N;
   node  *Node, *NodeSucc;
   conn  *Conn;
   ear   *Listen;
   struct timeval TimeOut[1];

   FD_ZERO(ReadSockets);
   FD_ZERO(WriteSockets);
   DescMax  = 0;
   Blocking = TRUE;

   for (Node = ListenList->Head; Node != NULL; Node = NodeSucc) {
      Listen   = Node->Object;
      NodeSucc = Node->Succ;
      if (Listen->Socket != NO_SOCKET) {
         FD_SET(Listen->Socket,ReadSockets);
         if (Listen->Socket > DescMax) DescMax = Listen->Socket;
      } else {
         RemNode(ListenList,Node);
         DeleteListen(Listen);
      }
   }

   for (Node = ConnList->Head; Node != NULL; Node = NodeSucc) {
      Conn     = Node->Object;
      NodeSucc = Node->Succ;
      if (Conn->Socket != NO_SOCKET) {
         if (Conn->Alive && Conn->In->Pos < BUFFER_SIZE) {
            FD_SET(Conn->Socket,ReadSockets);
            if (Conn->Socket > DescMax) DescMax = Conn->Socket;
         }
         if (Conn->Out->Pos != 0) {
            FD_SET(Conn->Socket,WriteSockets);
            if (Conn->Socket > DescMax) DescMax = Conn->Socket;
         }
         if (Conn->In->Line != 0) Blocking = FALSE;
      } else {
         RemNode(ConnList,Node);
         DeleteConn(Conn);
      }
   }

   TimeOut->tv_sec  = 0;
   TimeOut->tv_usec = 0;

   N = select(DescMax+1,ReadSockets,WriteSockets,NULL,(Blocking)?NULL:TimeOut);

   switch (N) {
   case 0 :
      return;
   case -1 :
      if (errno != EINTR) Error("Server(): select(): %s",strerror(errno));
      return;
   }

   for (Node = ListenList->Head; Node != NULL; Node = Node->Succ) {
      Listen = Node->Object;
      if (Listen->Socket != NO_SOCKET && FD_ISSET(Listen->Socket,ReadSockets)) {
         Accept(Listen->Socket);
      }
   }

   for (Node = ConnList->Head; Node != NULL; Node = Node->Succ) {
      Conn = Node->Object;
      if (Conn->Socket != NO_SOCKET && Conn->Alive && Conn->In->Pos < BUFFER_SIZE && FD_ISSET(Conn->Socket,ReadSockets)) {
         if (! ReadConn(Conn)) KillConn(Conn);
      }
      if (Conn->Socket != NO_SOCKET && Conn->Out->Pos != 0 && FD_ISSET(Conn->Socket,WriteSockets)) {
         if (! WriteConn(Conn)) KillConn(Conn);
      }
   }
}

/* Accept() */

static int Accept(int ListenSocket) {

   int     Socket, AddressLen;
   conn   *Conn;
   struct sockaddr_in Address[1];
   struct hostent *Host;
   char    HostAddr[NAME_SIZE+1], HostName[NAME_SIZE+1];
   int     Ident, Option;

   AddressLen = sizeof(struct sockaddr_in);
   Socket = accept(ListenSocket,(struct sockaddr *)Address,&AddressLen);
   if (Socket == -1) {
      if (errno != EINTR) Error("Server(): accept(): %s",strerror(errno));
      return FALSE;
   }
   strncpy(HostAddr,inet_ntoa(Address->sin_addr),NAME_SIZE);

   Host = gethostbyaddr((const char *)&Address->sin_addr.s_addr,sizeof(Address->sin_addr.s_addr),AF_INET);
   if (Host != NULL) {
      strncpy(HostName,Host->h_name,NAME_SIZE);
   } else {
      if (errno != EINTR) Warning("Server(): gethostbyaddr(): %s",strerror(errno));
      strcpy(HostName,"");
   }

/*
   Ident = Connect(HostAddr,113);
   printf("Ident = %d\n",Ident);

   if (Ident != NO_SOCKET) {

      char String[STRING_SIZE+1];
      int N;
      char *P1, *P2;

      sprintf(String,"%d,%d\r\n",4000,ntohs(Address->sin_port));
      write(Ident,String,strlen(String));

      N = read(Ident,String,(size_t)1024);
      CloseSocket(&Ident);

      printf("N = %d\n",N);

      if (N > 0) {
         String[N] = '\0';
         printf("String = \"%s\"\n",String);
         for (P1 = strtok(String," \t\r\n:"); (P2 = strtok(NULL," \t\r\n:")) != NULL; P1 = P2)
            ;
         printf("userid = \"%s\"\n",P1);
      }
   }
*/

/*
   if ((Address->sin_addr.s_net != 134 || Address->sin_addr.s_host != 206 || Address->sin_addr.s_lh < 10 || Address->sin_addr.s_lh > 13)
    && (Address->sin_addr.s_net != 127 || Address->sin_addr.s_host != 0 || Address->sin_addr.s_lh != 0)) {
*/

   if (TRUE) {

      if (Host != NULL) {
         Trace(HOST_LOG,"%-15.15s (%s)",HostAddr,HostName);
         if (! IsAllowedConn(HostAddr,HostName)) {
            Trace(BANNED_LOG,"%-15.15s (%s)",HostAddr,HostName);
            CloseSocket(&Socket);
            return FALSE;
         }
      } else {
         Trace(HOST_LOG,"%-15.15s",HostAddr);
         if (! IsAllowedConn(HostAddr,NULL)) {
            Trace(BANNED_LOG,"%-15.15s",HostAddr);
            CloseSocket(&Socket);
            return FALSE;
         }
      }
   }

   if (fcntl(Socket,F_SETFL,O_NONBLOCK) == -1) {
      Error("Server(): fcntl(O_NONBLOCK): %s",strerror(errno));
   }
   Option = TRUE;
   if (setsockopt(Socket,SOL_SOCKET,SO_KEEPALIVE,(char *)&Option,sizeof(Option)) == -1) {
      Warning("Server(): setsockopt(SO_KEEPALIVE): %s",strerror(errno));
   }
   Option = TRUE;
   if (setsockopt(Socket,IPPROTO_TCP,TCP_NODELAY,(char *)&Option,sizeof(Option)) == -1) {
      Warning("Server(): setsockopt(TCP_NODELAY): %s",strerror(errno));
   }

   Conn = NewConn(Socket);
   if (Conn == NULL) {
      Error("Accept(): not enough memory for Conn");
      CloseSocket(&Socket);
      return FALSE;
   }

   AddTail(ConnList,Conn);

/*
   if (Host != NULL) {
      SetString(&Conn->Host,HostName);
   } else {
      SetString(&Conn->Host,HostAddr);
   }
*/

   AddConn(Conn,HostAddr,(Host!=NULL)?HostName:NULL);

   return TRUE;
}

/* IsAllowed() */

static int IsAllowed(const char *HostAddr, const char *HostName) {

/*
   if (HostName != NULL) {
      if (strstr(HostName,".datasync.com")     != NULL
       || strstr(HostName,".ametro.net")       != NULL
       || strstr(HostName,"rinx.com")          != NULL
       || strstr(HostName,".bc.wave.home.com") != NULL
       || strstr(HostName,".blx.netdoor.com")  != NULL) {
         return FALSE;
      }
   }
*/

/*
   if (strstr(HostAddr,"216.41.16.") != NULL
    || strstr(HostAddr,"172.16.")    != NULL) {
      return FALSE;
   }
*/

   return TRUE;
}

/* GetString() */

char *GetString(conn *Conn) {

   size_t Len, LineSize;
   static char String[LINE_SIZE+1];

   if (Conn == NULL) {
      Error("GetString(): Conn = NULL");
      return NULL;
   } else if (Conn->Socket == NO_SOCKET) {
      Error("GetString(): Conn->Socket = NO_SOCKET");
      return NULL;
   } else if (! Conn->Alive) {
      Error("GetString(): Conn->Alive = FALSE");
      return NULL;
   } else if (Conn->In->Line == 0) {
      Error("GetString(): Conn->In->Line = 0");
      return NULL;
   }

   Len      = strlen(Conn->In->Buffer);
   LineSize = (Len <= LINE_SIZE) ? Len : LINE_SIZE;

   memcpy(String,Conn->In->Buffer,LineSize);
   String[LineSize] = '\0';

   Len++; /* '\0' */
   Conn->In->Pos -= Len;
   if (Conn->In->Pos != 0) memmove(Conn->In->Buffer,&Conn->In->Buffer[Len],Conn->In->Pos);
   Conn->In->Line--;

   return String;
}

/* SendString() */

int SendString(conn *Conn, const char *String) {

   size_t Len;

   if (Conn == NULL) {
      Error("SendString(): Conn = NULL");
      return FALSE;
   } else if (Conn->Socket == NO_SOCKET) {
      Error("SendString(): Conn->Socket = NO_SOCKET");
      return FALSE;
   } else if (! Conn->Alive) {
      Error("SendString(): Conn->Alive = FALSE");
      return FALSE;
   } else if (String == NULL) {
      Error("SendString(): String = NULL");
      return FALSE;
   }

   Len = strlen(String);
   if (Len == 0) {
      Error("SendString(): Len = 0");
      return FALSE;
   }

   if (Conn->Out->Pos + Len > BUFFER_SIZE) {
      Warning("SendString(): User write buffer overflow");
      Conn->Error = "User write buffer overflow";
      CloseConn(Conn);
      return FALSE;
   }

   memcpy(&Conn->Out->Buffer[Conn->Out->Pos],String,Len);
   Conn->Out->Pos += Len;

   if (Conn->Out->Pos > Conn->Debug->OutBufSize) Conn->Debug->OutBufSize = Conn->Out->Pos;
   if (Conn->Out->Pos > MtpDebug->OutBufSize)    MtpDebug->OutBufSize    = Conn->Out->Pos;

   return TRUE;
}

/* Connect() */

static int Connect(const char *HostName, int Port) {

   struct sockaddr_in Address[1];
   struct hostent *Host;
   int INetAddr, Socket;

   memset(Address,0,sizeof(struct sockaddr_in));
   Address->sin_family = AF_INET;
   Address->sin_port   = htons(Port);

   Host     = gethostbyname(HostName);
   INetAddr = inet_addr(HostName);
   if (Host != NULL) {
      memcpy(&Address->sin_addr,Host->h_addr,(size_t)Host->h_length);
   } else if (INetAddr != -1) {
      memcpy(&Address->sin_addr,&INetAddr,sizeof(INetAddr));
   } else {
      Error("Connect(\"%s\"): inet_addr(): %s",HostName,Port,strerror(errno));
      return NO_SOCKET;
   }

   Socket = socket(AF_INET,SOCK_STREAM,0);
   if (Socket == NO_SOCKET) {
      Error("Connect(): socket(): %s",strerror(errno));
      return NO_SOCKET;
   }

   if (connect(Socket,(struct sockaddr *)&Address,sizeof(Address)) == -1) {
      CloseSocket(&Socket);
      Warning("Connect(\"%s\",%d): connect(): %s",HostName,Port,strerror(errno));
      return NO_SOCKET;
   }

   return Socket;
}

/* NewListen() */

static ear *NewListen(int Socket) {

   ear *Listen;

   Listen = calloc(1,sizeof(ear));
   if (Listen == NULL) {
      Error("NewListen(): not enough memory for Listen");
      return NULL;
   }

   Listen->Socket = Socket;

   return Listen;
}

/* DeleteListen() */

static int DeleteListen(ear *Listen) {

   if (Listen == NULL) {
      Error("DeleteListen(): Listen = NULL");
      return FALSE;
   }

   free(Listen);

   return TRUE;
}

/* NewConn() */

static conn *NewConn(int Socket) {

   conn *Conn;

   Conn = calloc(1,sizeof(conn));
   if (Conn == NULL) {
      Error("NewConn(): not enough memory for Conn");
      return NULL;
   }

   Conn->Object    = NULL;
   Conn->Alive     = TRUE;
   Conn->Socket    = Socket;
   Conn->Error     = NULL;
   Conn->In->Pos   = 0;
   Conn->In->Line  = 0;
   Conn->In->Mode  = MD_DATA;
   Conn->Out->Pos  = 0;
   Conn->Out->Line = 0;
   Conn->Out->Mode = MD_DATA;
   InitDebug(Conn->Debug);

   return Conn;
}

/* DeleteConn() */

static int DeleteConn(conn *Conn) {

   if (Conn == NULL) {
      Error("DeleteConn(): Conn = NULL");
      return FALSE;
   }

   free(Conn);

   return TRUE;
}

/* SetConnObject() */

int SetConnObject(conn *Conn, void *Object) {

   if (Conn == NULL) {
      Error("SetConnObject(): Conn = NULL");
      return FALSE;
   }

   Conn->Object = Object;

   return TRUE;
}

/* IsAliveConn() */

int IsAliveConn(const conn *Conn) {

   if (Conn == NULL) {
      Error("IsAliveConn(): Conn = NULL");
      return FALSE;
   }

   return Conn->Socket != NO_SOCKET && Conn->Alive;
}

/* ReadConn() */

static int ReadConn(conn *Conn) {

   int    N, I, ErrNo;
   char  *StrError;
   uchar  Buffer[BUFFER_SIZE], Char;

   if (Conn == NULL) {
      Error("ReadConn(): Conn = NULL");
      return FALSE;
   } else if (Conn->Socket == NO_SOCKET) {
      Error("ReadConn(): Conn->Socket = NO_SOCKET");
      return FALSE;
   } else if (! Conn->Alive) {
      Error("ReadConn(): Conn->Alive = FALSE");
      return FALSE;
   }

   N = read(Conn->Socket,(char *)Buffer,(size_t)(BUFFER_SIZE-Conn->In->Pos));

   switch (N) {
   case 0 :
      Conn->Error = "Connection closed by client";
      return FALSE;
      break;
   case -1 :
      ErrNo    = errno;
      StrError = strerror(ErrNo);
      Trace(SOCKET_LOG,"ReadConn(): read(): %s",StrError);
      if (ErrNo != EINTR) {
         Warning("ReadConn(): read(): %s",StrError);
         Conn->Error = StrError;
         return FALSE;
      }
      return TRUE;
      break;
   }

   Conn->Debug->InOpNb++;
   Conn->Debug->InByteNb += N;
   if (N > Conn->Debug->InByteMax) Conn->Debug->InByteMax = N;

   MtpDebug->InOpNb++;
   MtpDebug->InByteNb += N;
   if (N > MtpDebug->InByteMax) MtpDebug->InByteMax = N;

   for (I = 0; I < N; I++) {

      Char = Buffer[I];

      switch (Conn->In->Mode) {

      case MD_DATA :

         if (Char == IAC) {
            Conn->In->Mode = MD_IAC;
         } else if (Char == '\n') {
            Conn->In->Buffer[Conn->In->Pos++] = '\0';
            Conn->In->Line++;
         } else if (Char >= ' ') { /* isprint(Char) */
            Conn->In->Buffer[Conn->In->Pos++] = (char) Char;
         }
         break;

      case MD_IAC :

         switch (Char) {
         case WILL :
            Conn->In->Mode = MD_WILL;
            break;
         case WONT :
            Conn->In->Mode = MD_WONT;
            break;
         case DO :
            Conn->In->Mode = MD_DO;
            break;
         case DONT :
            Conn->In->Mode = MD_DONT;
            break;
         case IAC : /* 255 */
            Conn->In->Mode = MD_DATA;
            Conn->In->Buffer[Conn->In->Pos++] = (char) IAC;
            break;
         default :
            Conn->In->Mode = MD_DATA;
            if (TELCMD_OK(Char)) {
               Trace(TELNET_LOG,"RECV IAC %s",TELCMD(Char));
            } else {
               Trace(TELNET_LOG,"RECV IAC %d",Char);
               Trace(SOCKET_LOG,"ReadConn(): IAC %d",Char);
               Conn->Error = "Invalid TELNET command";
               return FALSE;
            }
            switch (Char) {
            case ABORT :
            case BREAK :
            case xEOF :
            case IP :
            case SUSP :
               Trace(SOCKET_LOG,"ReadConn(): IAC %s",TELCMD(Char));
               Conn->Error = TELCMD(Char);
               return FALSE;
               break;
            }
            break;
         }
         break;

      case MD_WILL :

         Conn->In->Mode = MD_DATA;
         if (TELOPT_OK(Char)) {
            Trace(TELNET_LOG,"RECV IAC WILL %s",TELOPT(Char));
         } else {
            Trace(TELNET_LOG,"RECV IAC WILL %d",Char);
            Trace(SOCKET_LOG,"ReadConn(): IAC WILL %d",Char);
            return FALSE;
         }
         break;

      case MD_WONT :

         Conn->In->Mode = MD_DATA;
         if (TELOPT_OK(Char)) {
            Trace(TELNET_LOG,"RECV IAC WONT %s",TELOPT(Char));
         } else {
            Trace(TELNET_LOG,"RECV IAC WONT %d",Char);
            Trace(SOCKET_LOG,"ReadConn(): IAC WONT %d",Char);
            return FALSE;
         }
         break;

      case MD_DO :

         Conn->In->Mode = MD_DATA;
         if (TELOPT_OK(Char)) {
            Trace(TELNET_LOG,"RECV IAC DO %s",TELOPT(Char));
            if (Char == TELOPT_TM) {
               Trace(TELNET_LOG,"SENT IAC WILL %s",TELOPT(TELOPT_TM));
               SendString(Conn,(const char *)TimingMark);
            }
         } else {
            Trace(TELNET_LOG,"RECV IAC DO %d",Char);
            Trace(SOCKET_LOG,"ReadConn(): IAC DO %d",Char);
            return FALSE;
         }
         break;

      case MD_DONT :

         Conn->In->Mode = MD_DATA;
         if (TELOPT_OK(Char)) {
            Trace(TELNET_LOG,"RECV IAC DONT %s",TELOPT(Char));
         } else {
            Trace(TELNET_LOG,"RECV IAC DONT %d",Char);
            Trace(SOCKET_LOG,"ReadConn(): IAC DONT %d",Char);
            return FALSE;
         }
         break;
      }
   }

   if (Conn->In->Pos > Conn->Debug->InBufSize) Conn->Debug->InBufSize = Conn->In->Pos;
   if (Conn->In->Pos > MtpDebug->InBufSize)    MtpDebug->InBufSize    = Conn->In->Pos;

   return TRUE;
}

/* WriteConn() */

int WriteConn(conn *Conn) {

   int   N, ErrNo;
   char *StrError;

   if (Conn == NULL) {
      Error("WriteConn(): Conn = NULL");
      return FALSE;
   } else if (Conn->Socket == NO_SOCKET) {
      Error("WriteConn(): Conn->Socket = NO_SOCKET");
      return FALSE;
   } else if (Conn->Out->Pos == 0) {
      Error("WriteConn(): Conn->Out->Pos = 0");
      return FALSE;
   }

   N = write(Conn->Socket,Conn->Out->Buffer,Conn->Out->Pos);

   switch (N) {
   case 0 :
      Trace(SOCKET_LOG,"WriteConn(): write() = 0");
      Warning("WriteConn(): write() = 0");
      Conn->Error = "User socket write() returned 0";
      return FALSE;
      break;
   case -1 :
      ErrNo    = errno;
      StrError = strerror(ErrNo);
      Trace(SOCKET_LOG,"WriteConn(): write(): %s",StrError);
      if (ErrNo != EINTR) {
         Warning("WriteConn(): write(): %s",StrError);
         Conn->Error = StrError;
         return FALSE;
      }
      return TRUE;
      break;
   }

   Conn->Debug->OutOpNb++;
   Conn->Debug->OutByteNb += N;
   if (N > Conn->Debug->OutByteMax) Conn->Debug->OutByteMax = N;

   MtpDebug->OutOpNb++;
   MtpDebug->OutByteNb += N;
   if (N > MtpDebug->OutByteMax) MtpDebug->OutByteMax = N;

   Conn->Out->Pos -= N;
   if (Conn->Out->Pos != 0) {
      memmove(Conn->Out->Buffer,&Conn->Out->Buffer[N],Conn->Out->Pos);
   } else {
      if (! Conn->Alive) KillConn(Conn);
   }

   return TRUE;
}

/* CloseConn() */

int CloseConn(conn *Conn) {

   if (Conn == NULL) {
      Error("CloseConn(): Conn = NULL");
      return FALSE;
   } else if (Conn->Socket == NO_SOCKET) {
      Error("CloseConn(): Conn->Socket = NO_SOCKET");
      return FALSE;
   }

   if (Conn->Alive) {
      if (Conn->Out->Pos == 0) CloseSocket(&Conn->Socket);
      Conn->Alive = FALSE;
      RemConn(Conn,Conn->Object);
      Conn->Object = NULL;
   }

   return TRUE;
}

/* KillConn() */

static int KillConn(conn *Conn) {

   if (Conn == NULL) {
      Error("KillConn(): Conn = NULL");
      return FALSE;
   } else if (Conn->Socket == NO_SOCKET) {
      Error("KillConn(): Conn->Socket = NO_SOCKET");
      return FALSE;
   }

   CloseSocket(&Conn->Socket);

   if (Conn->Alive) {
      Conn->Alive = FALSE;
      RemConn(Conn,Conn->Object);
      Conn->Object = NULL;
   }

   return TRUE;
}

/* InitDebug() */

static int InitDebug(debug *Debug) {

   if (Debug == NULL) {
      Error("InitDebug(): Debug = NULL");
      return FALSE;
   }

   Debug->Start      = time(NULL);
   Debug->InOpNb     = 0;
   Debug->InByteNb   = 0.0;
   Debug->InByteMax  = 0;
   Debug->InBufSize  = 0;
   Debug->OutOpNb    = 0;
   Debug->OutByteNb  = 0.0;
   Debug->OutByteMax = 0;
   Debug->OutBufSize = 0;

   return TRUE;
}

/* CloseSocket() */

static int CloseSocket(int *Socket) {

   node *Node;
   ear  *Listen;

   if (Socket == NULL) {
      Error("CloseSocket(): Socket = NULL");
      return FALSE;
   } else if (*Socket == NO_SOCKET) {
      Error("CloseSocket(): *Socket = NO_SOCKET");
      return FALSE;
   }

   for (Node = ListenList->Head; Node != NULL; Node = Node->Succ) {
      Listen = Node->Object;
      if (*Socket == Listen->Socket) break;
   }
   if (Node == NULL && shutdown(*Socket,READ_WRITE) == -1) {
      Error("CloseSocket(): shutdown(): %s",strerror(errno));
   }

   if (close(*Socket) == -1) {
      Error("CloseSocket(): close(): %s",strerror(errno));
      return FALSE;
   }

   *Socket = NO_SOCKET;

   return TRUE;
}

/* InitList() */

static void InitList(list *List) {

   List->Head = NULL;
   List->Tail = NULL;
}

/* ClearList() */

static void ClearList(list *List) {

   node *Node, *NodeSucc;

   for (Node = List->Head; Node != NULL; Node = NodeSucc) {
      NodeSucc = Node->Succ;
      free(Node);
   }

   List->Head = NULL;
   List->Tail = NULL;
}

/* AddTail() */

static void AddTail(list *List, void *Object) {

   node *Node;

   Node = malloc(sizeof(node));
   if (Node == NULL) FatalError("Not enough memory for Node in AddTail()");

   Node->Pred   = List->Tail;
   Node->Succ   = NULL;
   Node->Object = Object;

   if (List->Tail != NULL) {
      List->Tail->Succ = Node;
   } else {
      List->Head = Node;
   }
   List->Tail = Node;
}

/* RemObject() */

static void RemObject(list *List, void *Object) {

   node *Node;

   for (Node = List->Head; Node != NULL; Node = Node->Succ) {
      if (Node->Object == Object) {
         RemNode(List,Node);
         return;
      }
   }
}

/* RemNode() */

static void RemNode(list *List, node *Node) {

   if (Node->Pred != NULL) {
      Node->Pred->Succ = Node->Succ;
   } else {
      List->Head = Node->Succ;
   }

   if (Node->Succ != NULL) {
      Node->Succ->Pred = Node->Pred;
   } else {
      List->Tail = Node->Pred;
   }

   free(Node);
}

/* AsciiToTelnet() */

void AsciiToTelnet(const char Src[], char Dst[]) {

   for (; *Src != '\0'; Src++, Dst++) {
      switch (*Src) {
      case '\n' :
         *Dst++ = '\r';
         break;
      case (char) IAC :
         *Dst++ = IAC;
         break;
      }
      *Dst = *Src;
   }
   *Dst = '\0';
}

/* End of Socket.C */
