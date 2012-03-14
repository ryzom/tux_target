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

/* Crypt.C */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crypt.h"
#include "types.h"
#include "command.h"
#include "server.h"

/* Functions */

/* Crypt() */

char *Crypt(int Language, const char *String) {

   static char String2[STRING_SIZE];
   static const char *CryptTab[] = { 
     "zzZZzZZZZzzzZzzZZzzzzzz", /* le dodo */
     "mmmh MHhhmhhm!!! hmmmh Mhhhmmh hhmMHhmmh! hmmm Hmmm!!!",
     "mais pourquoi ca marche pas ???", /* le mec qui veut pas dire ca */
     "je suis desole, je ne le referais plus...", /* le mec qui pardonne */
     "je suis pas tres intelligent, mais c'est pas ma faute, je suis ne comme ca...",
     "euuuh, move #$7a,D0 ???",
     "je hais l'informatique...",
     "je sens comme un probleme...",
     "j'ai l'impression que j'ecris n'importe quoi !",
     "aujourd'hui, je paye les chocos :)",
     "c'est de la 3d isometrique ?",
     "un jour, je serais le maitre du monde",
     "bon, ok, j'adore IXMCC (krys chat)",
     "quand je serais grand, je serais prof de BD",
     "je recherche des infos sur la reproduction des lichaons en milieu sub-aquatique",
     "j'eteint le giroscope mais il rale encore",
     "je veux aller en DESS SIAD",
     "hey! les gars! qu'est-ce qu'il m'arrive ???",
     "ca code?",
     "ca y est : j'ai pete un plomb !!!",
     "mais ou est ce que j'ai bien pu garer mon radiateur ???",
     "je suis fier comme un vaker",
     "c'est qui Xann ?",
     "je boirais bien une petite soupe au pistou",
     "depuis que je suis sur mtp chat, tout le monde me fait des farces...",
     "je suis encore plus nul en calcul mental qu'en informatique",
     "il pleut des girafes dehors!",
     "Ada est aussi objet que Cobol!",
     "bah oui je suis masochiste, et alors ?",
     "qui veut des jpeg, j'en ai plein !",
     "ca imprime pas...",
     "je vais faire un tour au SH1, c'est plein de filles !"
     "j'ai eu 2 en algo, mais c'est pas grave, j'ai plein d'amis",
     "c'est au pied du mur que l'on voit le mieux le mur !",
     "des fois on perd et des fois on ne gagne pas...",
     "Dark City, c'etait super, j'irais le revoir une deuxieme fois, c'est sur !",
     "pourquoi mon RAC rentre pas dans ce lecteur de disquette ?",
     "mon frigo se prend pour mami nova",
     "comment on gere les fichiers de plus de 4 gigaoctets ???",
     "un jour, je serais beau et intelligent !",
     "ou c'est qu'on eteint la station sun ???",
     "qu'est ce qu'elles sont bien les filles en formation info !!!",
     "mais dites a ce mossieur Cron d'arreter de m'ecrire tous les jours",
     "c'est quoi un cartoch ???",
   };
   int StrPos = 0, CryptPos = 0, Good;

   strncpy(String2,String,STRING_SIZE);


   if (Language > CRYPT_NONE) {
      Good = (int) ((double) RAND_MAX / 100.0 * (double) Language);
      while (String2[StrPos] != '\0') {
         if (String2[StrPos] == ' ' || rand() >= Good) {
            StrPos++;
         } else {
            String2[StrPos++] = '.';
         }
      }
   } else if (Language == LANGUAGE_VOWEL) {
      strcpy(String2,BlueCrypt(String));
   } else if (Language == LANGUAGE_SHUFFLE) {
      strcpy(String2,DragCrypt(String));
   } else if (Language == LANGUAGE_CRAZY) {
      strcpy(String2,CryptTab[rand()%((sizeof(CryptTab))/sizeof(char *)-2)+2]);
   } else if (Language == LANGUAGE_SLEEP) {
      while (String2[StrPos] != '\0') {
         String2[StrPos++] = CryptTab[0][CryptPos];
         CryptPos = (CryptPos + 1) % strlen(CryptTab[0]);
      }
   } else if (Language == LANGUAGE_BAILLON) {
      while (String2[StrPos] != '\0') {
         String2[StrPos++] = CryptTab[1][CryptPos];
         CryptPos = (CryptPos + 1) % strlen(CryptTab[1]);
      }
   } else if (Language == LANGUAGE_NONE) {
      String2[StrPos] = '\0';
   }

   return String2;
}

/* DragCrypt() */

#define WORD_SIZE 64
#define NB_WORD   64

char *DragCrypt(const char *entree) {

  static char sortie[STRING_SIZE];
  char tok[NB_WORD][WORD_SIZE], seps[] = " \t\n";
  char *token;
  int used[STRING_SIZE];
  int nb_tok=0,i;

  strcpy(sortie,"");
  token = strtok( entree, seps );
  while( token != NULL ) {
    strcpy(tok[nb_tok++],token );
    token = strtok( NULL, seps );
  }

  for(i=0;i<nb_tok;i++) used[i]=1;
  
  for(i=0;i<nb_tok;i++) {
    int alea=rand()%(nb_tok-i), realpos=0;
    for(realpos=0;alea!=0;realpos++) {
      if(used[realpos%nb_tok]==1) alea--;
    }
    while(used[realpos%nb_tok]==0) realpos++;
    used[realpos%nb_tok]=0;
    strcat(sortie,tok[realpos%nb_tok]);
    strcat(sortie," ");
  }
  return sortie;
}

/* BlueCrypt() */

#define voyelle "aeiouyAEIOUY"

char *BlueCrypt(const char *S) {

   static char res[STRING_SIZE+1];
   int i;

   for (i=0; *S!='\0'; S++) if (! strchr(voyelle,*S)) res[i++] = *S;
   res[i]='\0';

   return res;
}

/* LanguageName() */

const char *LanguageName(int Language) {

   static char String[16];

   if (Language > CRYPT_NONE) {
      sprintf(String,"CRYPT %d%%",Language);
      return String;
   } else if (Language == LANGUAGE_VOWEL) {
      return "VOWEL";
   } else if (Language == LANGUAGE_SHUFFLE) {
      return "SHUFFLE";
   } else if (Language == LANGUAGE_CRAZY) {
      return "CRAZY";
   } else if (Language == LANGUAGE_SLEEP) {
      return "SLEEP";
   } else if (Language == LANGUAGE_NONE) {
      return "NONE";
   } else if (Language == LANGUAGE_BAILLON) {
      return "BAILLON";
   } else {
      return "NORMAL";
   }
}

/* End of Crypt.C */
