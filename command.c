#include "command.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>


char* makeFileList(char* path)
{
	DIR* theCWD = opendir(path);
	struct dirent buf;
	struct dirent* cur = NULL;
	readdir_r(theCWD,&buf,&cur);
	int ttlBytes = 0;
	while(cur != NULL) {
		ttlBytes += strlen(cur->d_name) + 1;
		readdir_r(theCWD,&buf,&cur);
	}
	char* txt = malloc((ttlBytes + 10)*sizeof(char));
	rewinddir(theCWD);
	readdir_r(theCWD,&buf,&cur);
	*txt = 0;
	while(cur != NULL) {
		strcat(txt,cur->d_name);
		strcat(txt,"\n");
		readdir_r(theCWD,&buf,&cur);
	}
	closedir(theCWD);
	return txt;
}

int getFileSize(char* fName)
{
	FILE* f = fopen(fName,"r");
	fseek(f,0,SEEK_END);
	long sz = ftell(f);
	fclose(f);
	return (int)sz;
}

void sendFileOverSocket(char* fName,int chatSocket)
{
   FILE* fd=fopen(fName, "r");
   int x=getFileSize(fName);
   char* buf=(char*)malloc(sizeof(char)*x);
   fread(buf,x, sizeof(char), fd);
   int rem=x;
   int sent=0;
   while(sent<x){
       int status=send(chatSocket, buf+sent, rem, 0);
       rem-=status;
       sent+=status;
   }
      fclose(fd);
      free(buf);
}

void receiveFileOverSocket(int sid,char* fname,char* ext,int fSize)
{
  char fn[512];
  strcpy(fn,fname);
  strcat(fn,ext);
  FILE* fd = fopen(fn,"w");
  char* buf=(char*)malloc(sizeof(char)*fSize);
  int added=0;
  int rem=fSize;
  while(added<fSize){
    int status=recv(sid, buf+added, rem, 0);
    added+=status;
    rem-=status;
}
fwrite(buf, fSize, sizeof(char), fd);
fclose(fd);
free(buf);
}
