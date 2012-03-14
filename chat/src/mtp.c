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

/* MTP.C */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "mtp.h"
#include "types.h"

/* Constants */

#define LOG_PATH "log/"

/* Functions */

/* Exit() */

void Exit(void) {

   Trace(INOUT_LOG,"[shutdown]");
   exit(EXIT_SUCCESS);
}

/* Warning() */

void Warning(const char *Message, ...) {

   va_list Args;
   char    String[STRING_SIZE];

   va_start(Args,Message);
   vsprintf(String,Message,Args);
   va_end(Args);

   fprintf(stderr,"WARNING : %s...\n",String);
   Trace(ERROR_LOG,"WARNING %s",String);
}

/* Error() */

void Error(const char *Message, ...) {

   va_list Args;
   char    String[STRING_SIZE];

   va_start(Args,Message);
   vsprintf(String,Message,Args);
   va_end(Args);

   fprintf(stderr,"ERROR : %s !\n",String);
   Trace(ERROR_LOG,"ERROR   %s",String);
}

/* FatalError() */

void FatalError(const char *Message, ...) {

   va_list Args;
   char    String[STRING_SIZE];

   va_start(Args,Message);
   vsprintf(String,Message,Args);
   va_end(Args);

   fprintf(stderr,"FATAL ERROR : %s !!!\n",String);
   Trace(ERROR_LOG,"FATAL   %s",String);

   Exit();
}

/* Trace() */

void Trace(const char *LogFile, const char *Message, ...) {

   char     FileName[256], TimeString[32];
   time_t   Time;
   FILE    *Log;
   va_list  Args;

   sprintf(FileName,LOG_PATH "%s",LogFile);
   Log = fopen(FileName,"a");
   if (Log == NULL) return;

   Time = time(NULL);
   strftime(TimeString,(size_t)31,"%x %X ",localtime(&Time));

   fprintf(Log,"%s",TimeString);

   va_start(Args,Message);
   vfprintf(Log,Message,Args);
   va_end(Args);

   fprintf(Log,"\n");

   fclose(Log);
}

/* End of MTP.C */
