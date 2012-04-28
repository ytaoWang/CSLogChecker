#include "Error.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define ERRORLINE	1025

void handleError(const char *fmt,...)
{
	char buf[ERRORLINE];
	va_list ap;
	
	memset(buf,0,ERRORLINE);
	va_start(ap,fmt);
	vsnprintf(buf,sizeof(buf),fmt,ap);
	
	fprintf(stderr,BLINK_BLUE_RED);
	fprintf(stderr,"%s",buf);
	fprintf(stderr,RESET);
	fprintf(stderr,"\n");
}

void handleSyscallError(const char *fmt,...)
{
	char buf[ERRORLINE];
	va_list ap;
	
	memset(buf,0,ERRORLINE);
	va_start(ap,fmt);
	vsnprintf(buf,sizeof(buf),fmt,ap);
	
	fprintf(stderr,BLINK_BLACK_RED);
	fprintf(stderr,"%s:%s",buf,strerror(errno));
	fprintf(stderr,RESET);
	fprintf(stderr,"\n");
}

void handleFatalSyscallError(const char*fmt,...)
{
	char buf[ERRORLINE];
	va_list ap;
	
	memset(buf,0,ERRORLINE);
	va_start(ap,fmt);
	vsnprintf(buf,sizeof(buf),fmt,ap);
	
	fprintf(stderr,BLINK_GREEN_RED);
	fprintf(stderr,"%s:%s",buf,strerror(errno));
	fprintf(stderr,RESET);
	fprintf(stderr,"\n");
	exit(-1);
}

void handleFatalError(const char *fmt,...)
{
	char buf[ERRORLINE];
	va_list ap;

	memset(buf,0,ERRORLINE);
	va_start(ap,fmt);
	vsnprintf(buf,sizeof(buf),fmt,ap);
	
	fprintf(stderr,BLINK_WHITE_RED);
	fprintf(stderr,"%s",buf);
	fprintf(stderr,RESET);
	fprintf(stderr,"\n");
	exit(-1);
}

void handleStateError(const char *fmt,...)
{
	char buf[ERRORLINE];
	va_list ap;
	
	memset(buf,0,ERRORLINE);
	va_start(ap,fmt);
	vsnprintf(buf,sizeof(buf),fmt,ap);
	
	fprintf(stderr,BLINK_BLUE_BLACK);
	fprintf(stderr,"%s",buf);
	fprintf(stderr,RESET);
	fprintf(stderr,"\n");
}

void handleDebug(const char *fmt,...)
{
	char buf[ERRORLINE];
	va_list ap;
	
	memset(buf,0,ERRORLINE);
	va_start(ap,fmt);
	vsnprintf(buf,sizeof(buf),fmt,ap);
	
	fprintf(stderr,BLINK_WHITE_BLUE);
	fprintf(stderr,"%s",buf);
	fprintf(stderr,RESET);
	fprintf(stderr,"\n");
}
