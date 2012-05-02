#ifndef _ERROR_H_
#define _ERROR_H_
#include <cstdio>
#include <errno.h>
/*
formating code
0 All attributes off
1 Bold on
4 Underscore(on monochrome display adapter only)
5 Blink on
7 Reverse video on
8 Concealed on
Forgeground Colors 
30 Black 31 Red 32 Green 33 Yellow 34 Blue 35 Magenta 36 Cyan 37 White
Background Colors
40 Black 41 Red 42 Green 43 Yellow 44 Blue 45 Magenta 46 Cyan 47 White
*/

#define RESET	"\033[0m"
#define BLACK	"\033[30m"
#define RED	"\033[31m"
#define GREEN	"\033[32m"
#define YELLOW	"\033[33m"
#define BLUE	"\033[34m"
#define MAGENTA	"\033[35m"
#define CYAN	"\033[36m"
#define WHITE	"\033[37m"

#define BOLDBLACK	"\033[1;30m"
#define BOLDRED		"\033[1;31m"
#define BOLDGREEN	"\033[1;32m"
#define BOLDYELLOW	"\033[1;33m"
#define BOLDBLUE	"\033[1;34m"
#define BOLDMAGENTA	"\033[1;35m"
#define BOLDCYAN	"\033[1;36m"
#define BOLDWHITE	"\033[1;37m"

#define BLINK_WHITE_BLUE	"\033[1;5;37;44m"

#define BLINK_RED_BLACK		"\033[1;5;31;40m"
#define BLINK_GREEN_BLACK	"\033[1;5;32;40m"
#define BLINK_YELLOW_BLACK	"\033[1;5;33;40m"
#define BLINK_BLUE_BLACK	"\033[1;5;34;40m"
#define BLINK_MAGENTA_BLACK	"\033[1;5;35;40m"
#define BLINK_CYAN_BLACK	"\033[1;5;36;40m"
#define BLINK_WHITE_BLACK	"\033[1;5;37;40m"

#define BLINK_BLACK_RED		"\033[1;5;30;41m"
#define BLINK_GREEN_RED		"\033[1;5;31;41m"
#define BLINK_YELLOW_RED	"\033[1;5;33;41m"
#define BLINK_BLUE_RED		"\033[1;5;34;41m"
#define BLINK_MAGENTA_RED	"\033[1;5;35;41m"
#define BLINK_CYAN_RED		"\033[1;5;36;41m"
#define BLINK_WHITE_RED		"\033[1;5;37;41m"


#define RED_MSG(str)	printf(BLINK_RED_BLACK"%s"RESET"\n",str);

extern void handleError(const char*,...);
extern void handleSyscallError(const char*,...);
extern void handleFatalSyscallError(const char*,...);
extern void handleFatalError(const char*,...);
extern void handleStateError(const char*,...);
extern void handleDebug(const char*,...);

const int  SUCCESSFUL = 0;

const int FAILED = -1;

const int USING	=	0;
const int RECYLE = 1;

#endif
