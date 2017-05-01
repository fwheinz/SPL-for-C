<<<<<<< HEAD
/*
 * File: platform.c
 * ----------------
 * This file implements the platform-dependent aspects of the library,
 * primarily by passing commands to the Java back end.
 */

/*************************************************************************/
/* Stanford Portable Library                                             */
/* Copyright (C) 2013 by Eric Roberts <eroberts@cs.stanford.edu>         */
/*                                                                       */
/* This program is free software: you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* This program is distributed in the hope that it will be useful,       */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*************************************************************************/

#ifdef windows
#  include <windows.h>
#  include <shlwapi.h>
#  pragma comment(lib, "shlwapi.lib")
#  undef MOUSE_EVENT
#  undef KEY_EVENT
#  undef MOUSE_MOVED
#  undef HELP_KEY
#endif

#ifdef unixlike
#  define pause unixpause
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <dirent.h>
#  include <errno.h>
#  include <pwd.h>
#  include <unistd.h>
#  undef pause
#endif

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "cslib.h"
#include "filelib.h"
#include "gevents.h"
#include "gobjects.h"
#include "gtypes.h"
#include "gwindow.h"
#include "hashmap.h"
#include "platform.h"
#include "queue.h"
#include "simpio.h"
#include "strbuf.h"
#include "strlib.h"
#include "tokenscanner.h"

#include <string.h>
#include <math.h>
#include <regex.h>

/* Constants */

#define MAX_ID_LENGTH 24

/* Package variables */

static string programName = "Test";
static FILE *pin;
static FILE *pout;
static bool initialized = false;
static Queue eventQueue;
static HashMap sourceTable;
static HashMap timerTable;
static HashMap windowTable;
static StringBuffer psb;

#ifdef windows
static HANDLE g_hChildStd_IN_Rd = NULL;
static HANDLE g_hChildStd_IN_Wr = NULL; // this is where we write!
static HANDLE g_hChildStd_OUT_Rd = NULL;
static HANDLE g_hChildStd_OUT_Wr = NULL;
#endif

/* Private function prototypes */

static void initPipe(void);
static void putPipe(string format, ...);
static string getResult();
static void getStatus();
static int getInt();
static int getDouble();
static GDimension getGDimension();
static GEvent parseEvent(string line);
static GEvent parseMouseEvent(TokenScanner scanner, EventType type);
static GEvent parseKeyEvent(TokenScanner scanner, EventType type);
static GEvent parseTimerEvent(TokenScanner scanner, EventType type);
static GEvent parseWindowEvent(TokenScanner scanner, EventType type);
static GEvent parseActionEvent(TokenScanner scanner, EventType type);
static string scanString(TokenScanner scanner);
static int scanInt(TokenScanner scanner);
static double scanDouble(TokenScanner scanner);
static bool scanBool(TokenScanner scanner);
static string quotedString(string str);
static string boolString(bool flag);
static void registerSource(GInteractor interactor);

void initPlatform(void) {
	if (!initialized)
		initPipe();
	sourceTable = newHashMap();
	timerTable = newHashMap();
	windowTable = newHashMap();
	eventQueue = newQueue();
	psb = newStringBuffer();
	initialized = true;
}

string getId(void *ptr) {
	char str[MAX_ID_LENGTH];
	sprintf(str, "0x%lX", (long) ptr);
	return copyString(str);
}

#ifdef windows

/* Windows implementation of interface to Java back end */

extern string *getMainArgArray(void);

static void initPipe() {
	SECURITY_ATTRIBUTES saAttr;
#ifdef PIPEDEBUG   
	fprintf(stderr, "->Initializing pipe.\n");
#endif

	// Set the bInheritHandle flag so pipe handles are inherited.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe for the child process's STDOUT.
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) )
	error(TEXT("StdoutRd CreatePipe"));

	/// Ensure the read handle to the pipe for STDOUT is not inherited.
	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
	error(TEXT("Stdout SetHandleInformation"));

	// Create a pipe for the child process's STDIN.
	if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
	error(TEXT("Stdin CreatePipe"));

	// Ensure the write handle to the pipe for STDIN is not inherited.
	if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
	error(TEXT("Stdin SetHandleInformation"));

	// Create the child process.
	startJavaBackendProcess();
}

static void startJavaBackendProcess() {
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE;

	// todo define, not here...
	TCHAR szCmdline[MAX_PATH*2];
	TCHAR szPath[MAX_PATH];

	if (getApplicationPath(szPath, MAX_PATH) == NULL)
	{
		error(TEXT("getApplicationPath"));
		return;
	}

	if (!SetCurrentDirectory(szPath))
	{
		error(TEXT("SetCurrentDirectory"));
		return;
	}

	ZeroMemory( szCmdline, MAX_PATH*2 );
	strncpy(szCmdline, TEXT("java.exe -jar "), 14);
	strncat(szCmdline, szPath, MAX_PATH);
	strncat(szCmdline, TEXT("\\spl.jar"), 8);
#ifdef PIPEDEBUG   
	strncat(szCmdline, TEXT(" -debug"), 7);
#endif
<<<<<<< HEAD

	// Set up members of the PROCESS_INFORMATION structure.
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

	// Set up members of the STARTUPINFO structure.
	// This structure specifies the STDIN and STDOUT handles for redirection.
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process.
	bSuccess = CreateProcess(NULL,
			szCmdline,// command line
			NULL,// process security attributes
			NULL,// primary thread security attributes
			TRUE,// handles are inherited
			0,// creation flags
			NULL,// use parent's environment
			NULL,// use parent's current directory
			&siStartInfo,// STARTUPINFO pointer
			&piProcInfo);// receives PROCESS_INFORMATION

	// If an error occurs, exit the application.
	if ( ! bSuccess )
	error(TEXT("CreateProcess"));
	else
	{
#ifdef PIPEDEBUG
		fprintf(stderr, "%s: %s\n", TEXT("CreateProcess successful"), szCmdline);
#endif
=======
	
   // Set up members of the PROCESS_INFORMATION structure. 
   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
   // Set up members of the STARTUPINFO structure. 
   // This structure specifies the STDIN and STDOUT handles for redirection.
   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO); 
   siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
   siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
   siStartInfo.hStdInput = g_hChildStd_IN_Rd;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
 
   // Create the child process. 
   bSuccess = CreateProcess(NULL, 
      szCmdline,     // command line 
      NULL,          // process security attributes 
      NULL,          // primary thread security attributes 
      TRUE,          // handles are inherited 
      0,             // creation flags 
      NULL,          // use parent's environment 
      NULL,          // use parent's current directory 
      &siStartInfo,  // STARTUPINFO pointer 
      &piProcInfo);  // receives PROCESS_INFORMATION 
   
   // If an error occurs, exit the application. 
   if ( ! bSuccess ) 
      error(TEXT("CreateProcess"));
   else 
   {
   #ifdef PIPEDEBUG
   	  fprintf(stderr, "%s: %s\n", TEXT("CreateProcess successful"), szCmdline);
   #endif
	   
      // Close handles to the child process and its primary thread.
      // Some applications might keep these handles to monitor the status
      // of the child process, for example. 
>>>>>>> 464dc984d01ab13890ff7e8f97ac77a2321180f6

		// Close handles to the child process and its primary thread.
		// Some applications might keep these handles to monitor the status
		// of the child process, for example.

		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
	}
}

static void putPipe(string format, ...) {
	static DWORD cmdCount = 0;
	DWORD dwWritten;
	BOOL bSuccess = FALSE;

	//patch
	string cmd, jbetrace;
	va_list args;
	int capacity;

	clearStringBuffer(psb);
	va_start(args, format);
	capacity = printfCapacity(format, args);
	va_end(args);
	va_start(args, format);
	sbFormat(psb, capacity, format, args);
	va_end(args);
	cmd = getString(psb);
	int commandSize = strlen(cmd);
#ifdef PIPEDEBUG
	cmdCount++;
	fprintf(stderr, "Sent to pipe (%u): %s\n", cmdCount, cmd);
#endif

	jbetrace = getenv("JBETRACE");
	bSuccess = WriteFile(g_hChildStd_IN_Wr, cmd, commandSize, &dwWritten, NULL);
	bSuccess &= WriteFile(g_hChildStd_IN_Wr, "\n", 1, &dwWritten, NULL);

	if ( ! bSuccess ) error(TEXT("WriteFile to pipe"));
}

static string getResult() {
	int maxMessageLength = 300;

	string result;
	for(;;)
	{
		char message[200];
		readMessageFromBuffer(message, maxMessageLength);

<<<<<<< HEAD
		if (startsWith(message, "result:")) {
#ifdef PIPEDEBUG		  
			fprintf(stderr, "Result contains newLine char at: %d\n", findChar('\n', message, 0));
#endif		 
			result = substring(message, 7, stringLength(message));
=======
      if (startsWith(message, "result:")) {		 
         result = substring(message, 7, stringLength(message));
>>>>>>> 464dc984d01ab13890ff7e8f97ac77a2321180f6
#ifdef PIPEDEBUG		 
			fprintf(stderr, "Parsed result: %s\n", result);
#endif
			return result;
		} else if (startsWith(message, "event:")) {
#ifdef PIPEDEBUG		  
			fprintf(stderr, "Event contains newLine char at: %d\n", findChar('\n', message, 0));
			result = substring(message, 6, stringLength(message));
			fprintf(stderr, "Parsed event: %s\n", result);
#endif		 
			enqueue(eventQueue, parseEvent(message + 6));
		}
	}
}

void readMessageFromBuffer(char* message, int maxLength) {
	DWORD dwRead;
	int bufSize = 1;
	char messageBuffer[bufSize];
	BOOL bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	int bufferPosition = 0;
	char bufferedChar;
	while(bufferPosition < maxLength) {
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, messageBuffer, bufSize, &dwRead, NULL);
		bufferedChar = messageBuffer[0];
		if (bufferedChar == '\n') {
			break;
		}
		message[bufferPosition] = bufferedChar;
		bufferPosition++;
		if( !bSuccess || dwRead == 0 ) break;
	}
	message[bufferPosition-1] = '\0';
#ifdef PIPEDEBUG	  
	fprintf(stderr, "Read message from buffer: %s\n", message);
#endif	  
}

TCHAR* getApplicationPath(TCHAR* dest, size_t destSize) {
	if (!dest) return NULL;
	if (MAX_PATH > destSize) return NULL;
	DWORD length = GetModuleFileName( NULL, dest, destSize );
	PathRemoveFileSpec(dest);
	return dest;
}

#else

/* Linux/Mac implementation of interface to Java back end */

extern string *getMainArgArray(void);

static void initPipe(void) {
<<<<<<< HEAD
	int toJBE[2], fromJBE[2], child;
	string option, classpath;

	classpath = getenv("CLASSPATH");
	if (classpath == NULL) {
		classpath = "./spl.jar";
	}

	programName = getRoot(getTail(getMainArgArray()[0]));
	pipe(toJBE);
	pipe(fromJBE);
	child = fork();
	if (child == 0) {
		dup2(toJBE[0], 0);
		close(toJBE[0]);
		close(toJBE[1]);
		dup2(fromJBE[1], 1);
		close(fromJBE[0]);
		close(fromJBE[1]);
=======
   #ifdef PIPEDEBUG   
      fprintf(stderr, "->Initializing pipe.\n");
   #endif
   int toJBE[2], fromJBE[2], child;
   string option, classpath;

   classpath = getenv("CLASSPATH");
   if (classpath == NULL) {
      classpath = "./spl.jar";
   }

   programName = getRoot(getTail(getMainArgArray()[0]));
   pipe(toJBE);
   pipe(fromJBE);
   child = fork();
   if (child == 0) {
      dup2(toJBE[0], 0);
      close(toJBE[0]);
      close(toJBE[1]);
      dup2(fromJBE[1], 1);
      close(fromJBE[0]);
      close(fromJBE[1]);
>>>>>>> 464dc984d01ab13890ff7e8f97ac77a2321180f6
#ifdef __APPLE__
		option = concat("-Xdock:name=", programName);
		execlp("java", "java", option, "-classpath", classpath, "stanford/spl/JavaBackEnd", programName, NULL);
#else
		execlp("java", "java", "-classpath", classpath,
				"stanford/spl/JavaBackEnd", programName, NULL);
#endif
<<<<<<< HEAD
		error("Could not exec spl.jar");
	} else {
		pin = fdopen(fromJBE[0], "r");
		pout = fdopen(toJBE[1], "w");
		close(fromJBE[1]);
		close(toJBE[0]);
	}
}

static void putPipe(string format, ...) {
	string cmd, jbetrace;
	va_list args;
	int capacity;

	clearStringBuffer(psb);
	va_start(args, format);
	capacity = printfCapacity(format, args);
	va_end(args);
	va_start(args, format);
	sbFormat(psb, capacity, format, args);
	va_end(args);
	cmd = getString(psb);
	jbetrace = getenv("JBETRACE");
	if (jbetrace != NULL && (jbetrace[0] == 'T' || jbetrace[0] == 't')) {
		fprintf(stderr, "-> %s\n", cmd);
	}
	fprintf(pout, "%s\n", cmd);
	fflush(pout);
}

static string getResult() {
	string line, result, jbetrace, array;

	jbetrace = getenv("JBETRACE");
	while (true) {
		line = readLine(pin);
		if (line == NULL)
			exit(1);
		if (jbetrace != NULL && (jbetrace[0] == 'T' || jbetrace[0] == 't')) {
			fprintf(stderr, "<- %s\n", line);
		}
		if (startsWith(line, "result:")) {
			result = substring(line, 7, stringLength(line));
			freeBlock(line);
			return result;
		} else if (startsWith(line, "event:")) {
			enqueue(eventQueue, parseEvent(line + 6));
			freeBlock(line);
		} else if (startsWith(line, "array:")) {
			array = substring(line, 6, stringLength(line));
			return array;
		}

	}
=======
      error("Could not exec spl.jar");
   } else {
      pin = fdopen(fromJBE[0], "r");
      pout = fdopen(toJBE[1], "w");
      close(fromJBE[1]);
      close(toJBE[0]);
      #ifdef PIPEDEBUG
        fprintf(stderr, "%s\n", "CreateProcess successful");
      #endif
   }
}

static void putPipe(string format, ...) {
   string cmd, jbetrace;
   va_list args;
   int capacity;

   clearStringBuffer(psb);
   va_start(args, format);
   capacity = printfCapacity(format, args);
   va_end(args);
   va_start(args, format);
   sbFormat(psb, capacity, format, args);
   va_end(args);
   cmd = getString(psb);
   jbetrace = getenv("JBETRACE");
   if (jbetrace != NULL && (jbetrace[0] == 'T' || jbetrace[0] == 't')) {
      fprintf(stderr, "-> %s\n", cmd);
   }
   fprintf(pout, "%s\n", cmd);
#ifdef PIPEDEBUG
   fprintf(stderr, "Sent to pipe: %s\n", cmd);
#endif
   fflush(pout);
}

static string getResult() {
   string line, result, jbetrace;

   jbetrace = getenv("JBETRACE");
   while (true) {
      line = readLine(pin);
      if (line == NULL) exit(1);
      if (jbetrace != NULL && (jbetrace[0] == 'T' || jbetrace[0] == 't')) {
         fprintf(stderr, "<- %s\n", line);
      }
      if (startsWith(line, "result:")) {
         result = substring(line, 7, stringLength(line));
#ifdef PIPEDEBUG      
         fprintf(stderr, "Parsed result: %s\n", line);
#endif
         freeBlock(line);
         return result;
      } else if (startsWith(line, "event:")) {
         enqueue(eventQueue, parseEvent(line + 6));
#ifdef PIPEDEBUG       
         result = substring(line, 6, stringLength(line));
         fprintf(stderr, "Parsed event: %s\n", result);
#endif
         freeBlock(line);
      }
   }
>>>>>>> 464dc984d01ab13890ff7e8f97ac77a2321180f6
}

#endif

static void getStatus() {
	string result;

	result = getResult();
	if (!stringEqual(result, "ok"))
		error(result);
	freeBlock(result);
}

static int getInt() {
	string str;
	int result;

	str = getResult();
	result = stringToInteger(str);
	freeBlock(str);
	return result;
}

static int getDouble() {
	string str;
	double result;

	str = getResult();
	result = stringToReal(str);
	freeBlock(str);
	return result;
}

static GDimension getGDimension() {
	string str;
	double width, height;

	str = getResult();
	sscanf(str, "GDimension(%lg,%lg)", &width, &height);
	freeBlock(str);
	return createGDimension(width, height);
}

static int getBool() {
	string str;
	bool result;

	str = getResult();
	result = (str[0] == 'T' || str[0] == 't');
	freeBlock(str);
	return result;
}

static GPixelArray getGPixelArrayOp(GObject gobj) {
	string str;
	unsigned int ** pixmap;

	double width, height;
	unsigned long value, alpha, red, green, blue;

	str = getResult();
	sscanf(str, "GPixelArray(%lg,%lg)", &width, &height);
	freeBlock(str);

	//initialisierte PixelMatrix m x n
	pixmap = (unsigned int **) calloc(height, sizeof(unsigned int*));
	for (int i = 0; i < height; i++) {
		pixmap[i] = (int *) calloc(width, sizeof(unsigned int));
	}

	for (int j = 0; j < height; j++) {
		putPipe("GImage.getPixel(\"0x%lX\", %d, %d)", (long) gobj, j, 0);
		/*
		for (int i = 0; i < width; i++) {
			//getPixel ( imageid, x,y)
			//putPipe("GImage.getPixel(\"0x%lX\", %d, %d)", (long) gobj, j, i);

			//sscanf(str, "GPixel(%u)", &value);
			//pixmap[j][i] = (value);

		}
		*/
		str = getResult();
		//fprintf(stderr, "%s\n",str);
	}
	return createGPixelArray(width, height, pixmap);
}

static GPixelArray getGPixelArrayOp2(GObject gobj) {
	string str;
	unsigned int ** pixmap;

	double width, height;
	unsigned long value, alpha, red, green, blue;

	str = getResult();
	sscanf(str, "GPixelArray(%lg,%lg)", &width, &height);
	freeBlock(str);

	//Test speedup Optionen

	unsigned int teiler, result, rest, pixelssent;


	for(teiler = 1; teiler<=100;teiler++) {
		result = (unsigned int) width / teiler;
		rest = ((unsigned int) width)% teiler;
		if(result <= 25)
			if(rest==0){
				result;
				break;
			}
	}

	fprintf(stderr, "Wähle teiler %u, PixelSendPerRequest: %u bei Weite: %lf \n", teiler, result, width);



	//initialisierte PixelMatrix m x n
	pixmap = (unsigned int **) calloc(height, sizeof(unsigned int*));
	for (int i = 0; i < height; i++) {
		pixmap[i] = (int *) calloc(width, sizeof(unsigned int));
	}


	// "GPixel( len = 7
	// %u, first and next len =3 result -1 * 3
	// %u last len = 2 + 2
	//)" len = 2 + 2
	// -----------------------
	// 7 + (setteiler -1) *3 + 2 +2 = 11 + (result-1) * 3

	//'^(?:GPixel[(])'  	len = 14
	//'(?:|-)([0-9]+). '	len = 16 * result -1
	//'(?:|-)([0-9]+).'		len = 15
	//------------------------------------
	//		12+10 * result


	//"^[A-ZA-Za-z(]+"		len = 14
	//"(-[0-9]+|[0-9]+)[, ]{2}"	len = 23
	//"(-[0-9]+|[0-9]+)[)]"		len = 19

	size_t result_size = (11 + (result -1)*3);
	size_t regex_size = (14 +19 +(result-1)*23);

	fprintf(stderr, "result_size: %d, regex_size: %d\n",result_size,regex_size);
	string result_str = (string) calloc(result_size, sizeof(char));
	sprintf(result_str, "%s%s", result_str, "GPixel(");

	string regex_str = (string) calloc(regex_size, sizeof(char));
	string start = "^[A-ZA-Za-z(]+";
	string pattern1 = "(-[0-9]+|[0-9]+)[, ]{2}";
	string pattern2 ="(-[0-9]+|[0-9]+)[)]";
	//string pattern1 = "([0-9]+)(?:, )";
	//string pattern2 ="([0-9]+)(?:.)$";
	sprintf(regex_str, "%s%s", regex_str, start);

	for(int i=0; i<(result - 1);i++){
		sprintf(result_str, "%s%s", result_str, "%u,");
		sprintf(regex_str, "%s%s", regex_str, pattern1);
	}

	sprintf(result_str, "%s%s", result_str, "%u)");
	sprintf(regex_str, "%s%s", regex_str, pattern2);


	fprintf(stderr,"%s\n",result_str);
	fprintf(stderr,"%s\n",regex_str);
	int reti;
	char msgbuf[100];



	for (int j = 0; j < height; j++) {
		/*
		regex_t regex;
		reti = regcomp(&regex, regex_str, REG_EXTENDED );
		//reti = regcomp(&regex, "^a[[:alnum:]]", REG_EXTENDED);
		if (reti) {
			fprintf(stderr, "Could not compile regex\n");
			exit(1);
		}
		*/
		// Manual handling of inner for loop
		for (int i = 0; i < teiler; i++) {

			//getPixel ( imageid, x,y, pixelcount )
			putPipe("GImage.getPixelT(\"0x%lX\", %d, %d, %d)", (long) gobj, j, i*result, result);
			//putPipe("GImage.getPixel(\"0x%lX\", %d, %d)", (long) gobj, j, i);

			str = getResult();
			//fprintf(stderr, "%s\n", str);

			regmatch_t match[(result+1)];

		//printf("Match!\n");
			//printf("0: [%.*s]\n", match[0].rm_eo - match[0].rm_so, line + match[0].rm_so);
			//printf("1: [%.*s]\n", match[1].rm_eo - match[1].rm_so, line + match[1].rm_so);


			/* Execute regular expression */
			//reti = regexec(&regex, "abc", result, pmatch, 0);
			/*
			reti = regexec(&regex, str, (result+1), match, 0);
			if (!reti) {
				//puts("Match");

				for(int k=1;k<result+1;k++){
			    	//fprintf(stderr," %d | %d\n",match[i].rm_so, match[i].rm_eo);
			    	//fprintf(stderr,"%d: [%.*s]\n", i, match[i].rm_eo - match[i].rm_so, str + match[i].rm_so);
			    	//fprintf(stderr,"read [%d|%d] ... ", k, result);
			    	sscanf(str + match[k].rm_so, "%d", &value);
			    	//fprintf(stderr,"OK\n");
			    	//fprintf(stderr, "J:%d, I:%d, K:%d, pixmap[%d][%d] = %d\n", j, i, k,j,(i*result)+(k-1),value);
			    	pixmap[j][(i*result)+(k-1)] = (value);
			    }

			}
			else if (reti == REG_NOMATCH) {
			    //puts("No match");
			}
			else {
			    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
			    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
			}
			*/
			/* Free memory allocated to the pattern buffer by regcomp() */

			/*
			for(int k=0; i< result; i++){
				//fprintf(stderr, "%s\n", str);
				sscanf(str, "%d", &value);
				pixmap[j][(i*result)+k] = (value);
				str = substring(str, floor(log10(abs(value))) + 3, stringLength(str));
			}
			*/
			//sscanf(str, "GPixel(%u)", &value);
			//pixmap[j][i] = (value);
			//fprintf(stderr,"Next J\n");
		}
		//regfree(&regex);
	}
	return createGPixelArray(width, height, pixmap);
}

static GEvent parseEvent(string line) {
	TokenScanner scanner;
	string name;
	GEvent e;

	scanner = newTokenScanner();
	ignoreWhitespace(scanner);
	scanNumbers(scanner);
	scanStrings(scanner);
	setInputString(scanner, line);
	name = nextToken(scanner);
	e = NULL;
	if (stringEqual(name, "mousePressed")) {
		e = parseMouseEvent(scanner, MOUSE_PRESSED);
	} else if (stringEqual(name, "mouseReleased")) {
		e = parseMouseEvent(scanner, MOUSE_RELEASED);
	} else if (stringEqual(name, "mouseClicked")) {
		e = parseMouseEvent(scanner, MOUSE_CLICKED);
	} else if (stringEqual(name, "mouseMoved")) {
		e = parseMouseEvent(scanner, MOUSE_MOVED);
	} else if (stringEqual(name, "mouseDragged")) {
		e = parseMouseEvent(scanner, MOUSE_DRAGGED);
	} else if (stringEqual(name, "keyPressed")) {
		e = parseKeyEvent(scanner, KEY_PRESSED);
	} else if (stringEqual(name, "keyReleased")) {
		e = parseKeyEvent(scanner, KEY_RELEASED);
	} else if (stringEqual(name, "keyTyped")) {
		e = parseKeyEvent(scanner, KEY_TYPED);
	} else if (stringEqual(name, "actionPerformed")) {
		e = parseActionEvent(scanner, ACTION_PERFORMED);
	} else if (stringEqual(name, "timerTicked")) {
		e = parseTimerEvent(scanner, TIMER_TICKED);
	} else if (stringEqual(name, "windowClosed")) {
		e = parseWindowEvent(scanner, WINDOW_CLOSED);
		closeGWindow(getGWindow(e));
	} else if (stringEqual(name, "windowResized")) {
		e = parseWindowEvent(scanner, WINDOW_RESIZED);
	} else if (stringEqual(name, "lastWindowClosed")) {
		exit(0);
	} else {
		/* Ignore for now */
	}
	freeTokenScanner(scanner);
	freeBlock(name);
	return e;
}

static GEvent parseMouseEvent(TokenScanner scanner, EventType type) {
	string id;
	int modifiers;
	double time, x, y;
	GEvent e;

	verifyToken(scanner, "(");
	id = scanString(scanner);
	verifyToken(scanner, ",");
	time = scanDouble(scanner);
	verifyToken(scanner, ",");
	modifiers = scanInt(scanner);
	verifyToken(scanner, ",");
	x = scanDouble(scanner);
	verifyToken(scanner, ",");
	y = scanDouble(scanner);
	verifyToken(scanner, ")");
	e = newGMouseEvent(type, get(windowTable, id), x, y);
	setEventTime(e, time);
	setModifiers(e, modifiers);
	freeBlock(id);
	return e;
}

static GEvent parseKeyEvent(TokenScanner scanner, EventType type) {
	string id;
	int modifiers, keyChar, keyCode;
	double time;
	GEvent e;

	verifyToken(scanner, "(");
	id = scanString(scanner);
	verifyToken(scanner, ",");
	time = scanDouble(scanner);
	verifyToken(scanner, ",");
	modifiers = scanInt(scanner);
	verifyToken(scanner, ",");
	keyChar = scanInt(scanner);
	verifyToken(scanner, ",");
	keyCode = scanInt(scanner);
	verifyToken(scanner, ")");
	e = newGKeyEvent(type, get(windowTable, id), keyChar, keyCode);
	setEventTime(e, time);
	setModifiers(e, modifiers);
	freeBlock(id);
	return e;
}

static GEvent parseTimerEvent(TokenScanner scanner, EventType type) {
	string id;
	double time;
	GEvent e;

	verifyToken(scanner, "(");
	id = scanString(scanner);
	verifyToken(scanner, ",");
	time = scanDouble(scanner);
	verifyToken(scanner, ")");
	e = newGTimerEvent(type, get(timerTable, id));
	setEventTime(e, time);
	freeBlock(id);
	return e;
}

static GEvent parseWindowEvent(TokenScanner scanner, EventType type) {
	string id;
	double time;
	GEvent e;

	verifyToken(scanner, "(");
	id = scanString(scanner);
	verifyToken(scanner, ",");
	time = scanDouble(scanner);
	verifyToken(scanner, ")");
	e = newGWindowEvent(type, get(windowTable, id));
	setEventTime(e, time);
	freeBlock(id);
	return e;
}

static GEvent parseActionEvent(TokenScanner scanner, EventType type) {
	string id, action;
	double time;
	GEvent e;

	verifyToken(scanner, "(");
	id = scanString(scanner);
	verifyToken(scanner, ",");
	action = scanString(scanner);
	verifyToken(scanner, ",");
	time = scanDouble(scanner);
	verifyToken(scanner, ")");
	e = newGActionEvent(type, get(sourceTable, id), action);
	setEventTime(e, time);
	freeBlock(id);
	freeBlock(action);
	return e;
}

static string scanString(TokenScanner scanner) {
	string token, result;

	token = nextToken(scanner);
	result = getStringValue(token);
	freeBlock(token);
	return result;
}

static int scanInt(TokenScanner scanner) {
	string token;
	int sign, result;

	token = nextToken(scanner);
	sign = 1;
	if (stringEqual(token, "-")) {
		sign = -1;
		freeBlock(token);
		token = nextToken(scanner);
	}
	result = stringToInteger(token);
	freeBlock(token);
	return sign * result;
}

static double scanDouble(TokenScanner scanner) {
	string token;
	double result;
	int sign;

	token = nextToken(scanner);
	sign = 1;
	if (stringEqual(token, "-")) {
		sign = -1;
		freeBlock(token);
		token = nextToken(scanner);
	}
	result = stringToReal(token);
	freeBlock(token);
	return sign * result;
}

static bool scanBool(TokenScanner scanner) {
	string token;
	bool result;

	token = nextToken(scanner);
	result = startsWith(token, "t");
	freeBlock(token);
	return result;
}

static string quotedString(string str) {
	StringBuffer sb;
	char ch;
	int i;

	sb = newStringBuffer();
	pushChar(sb, '"');
	for (i = 0; str[i] != '\0'; i++) {
		ch = str[i];
		switch (ch) {
		case '\a':
			sbprintf(sb, "\\a");
			break;
		case '\b':
			sbprintf(sb, "\\b");
			break;
		case '\f':
			sbprintf(sb, "\\f");
			break;
		case '\n':
			sbprintf(sb, "\\n");
			break;
		case '\r':
			sbprintf(sb, "\\r");
			break;
		case '\t':
			sbprintf(sb, "\\t");
			break;
		case '\v':
			sbprintf(sb, "\\v");
			break;
		case '"':
			sbprintf(sb, "\\%03o", ch);
			break;
		case '\\':
			sbprintf(sb, "\\\\");
			break;
		default:
			if (isprint(ch)) {
				pushChar(sb, ch);
			} else {
				sbprintf(sb, "\\%03o", ch & 0xFF);
			}
		}
	}
	pushChar(sb, '"');
	str = copyString(getString(sb));
	freeStringBuffer(sb);
	return str;
}

static string boolString(bool flag) {
	return (flag) ? "true" : "false";
}

/* GWindow operations */

void createGWindowOp(GWindow gw, double width, double height, GObject top) {
	string id;

	initPlatform();
	id = getId(gw);
	put(windowTable, id, gw);
	freeBlock(id);
	putPipe("GWindow.create(\"0x%lX\", %g, %g, \"0x%lX\")", (long) gw, width,
			height, (long) top);
	//milliseconds on windows, secs on linux
#ifdef windows
	Sleep(2000);
#else
	sleep(2);
#endif
	getStatus();
}

void deleteGWindowOp(GWindow gw) {
	string id;

	id = getId(gw);
	remove(windowTable, id);
	freeBlock(id);
	putPipe("GWindow.delete(\"0x%lX\")", (long) gw);
}

void closeGWindowOp(GWindow gw) {
	putPipe("GWindow.close(\"0x%lX\")", (long) gw);
}

void requestFocusOp(GWindow gw) {
	putPipe("GWindow.requestFocus(\"0x%lX\")", (long) gw);
}

void clearOp(GWindow gw) {
	putPipe("GWindow.clear(\"0x%lX\")", (long) gw);
}

void repaintOp(GWindow gw) {
	putPipe("GWindow.repaint(\"0x%lX\")", (long) gw);
}

void setVisibleGWindowOp(GWindow gw, bool flag) {
	putPipe("GWindow.setVisible(\"0x%lX\", %s)", (long) gw, boolString(flag));
}

void setResizableOp(GWindow gw, bool flag) {
	putPipe("GWindow.setResizable(\"0x%lX\", %s)", (long) gw, boolString(flag));
}

void setWindowTitleOp(GWindow gw, string title) {
	title = quotedString(title);
	putPipe("GWindow.setWindowTitle(\"0x%lX\", %s)", (long) gw, title);
	freeBlock(title);
}

void setRegionAlignmentOp(GWindow gw, string region, string align) {
	putPipe("GWindow.setRegionAlignment(\"0x%lX\", \"%s\", \"%s\")", (long) gw,
			region, align);
}

void addToRegionOp(GWindow gw, GObject gobj, string region) {
	putPipe("GWindow.addToRegion(\"0x%lX\", \"0x%lX\", \"%s\")", (long) gw,
			(long) gobj, region);
}

double getScreenWidthOp(void) {
	putPipe("GWindow.getScreenWidth()");
	return getDouble();
}

double getScreenHeightOp(void) {
	putPipe("GWindow.getScreenHeight()");
	return getDouble();
}

void exitGraphicsOp(void) {
	putPipe("GWindow.exitGraphics()");
	exit(0);
}

/* GTimer operations */

void createTimerOp(GTimer timer, double delay) {
	string id;

	id = getId(timer);
	putHashMap(timerTable, id, timer);
	freeBlock(id);
	putPipe("GTimer.create(\"0x%lX\", %g)", (long) timer, delay);
}

void deleteTimerOp(GTimer timer) {
	string id;

	id = getId(timer);
	putHashMap(timerTable, id, timer);
	freeBlock(id);
	putPipe("GTimer.deleteTimer(\"0x%lX\")", (long) timer);
}

void startTimerOp(GTimer timer) {
	putPipe("GTimer.startTimer(\"0x%lX\")", (long) timer);
}

void stopTimerOp(GTimer timer) {
	putPipe("GTimer.stopTimer(\"0x%lX\")", (long) timer);
}

void pauseOp(double milliseconds) {
	putPipe("GTimer.pause(%g)", milliseconds);
	getStatus();
}

/* Sound operations */

void createSoundOp(Sound sound, string filename) {
	filename = quotedString(filename);
	putPipe("Sound.create(\"0x%lX\", %s)", (long) sound, filename);
	freeBlock(filename);
	getStatus();
}

void deleteSoundOp(Sound sound) {
	putPipe("Sound.delete(\"0x%lX\")", (long) sound);
}

void playSoundOp(Sound sound) {
	putPipe("Sound.play(\"0x%lX\")", (long) sound);
}

/* GObject operations */

void deleteGObjectOp(GObject gobj) {
	putPipe("GObject.delete(\"0x%lX\")", (long) gobj);
}

void setVisibleGObjectOp(GObject gobj, bool flag) {
	putPipe("GObject.setVisible(\"0x%lX\", %s)", (long) gobj, boolString(flag));
}

void setColorOp(GObject gobj, string color) {
	putPipe("GObject.setColor(\"0x%lX\", \"%s\")", (long) gobj, color);
}

void setLocationOp(GObject gobj, double x, double y) {
	putPipe("GObject.setLocation(\"0x%lX\", %g, %g)", (long) gobj, x, y);
}

void drawOp(GWindow gw, GObject gobj) {
	putPipe("GWindow.draw(\"0x%lX\", \"0x%lX\")", (long) gw, (long) gobj);
}

void setSizeOp(GObject gobj, double width, double height) {
	putPipe("GObject.setSize(\"0x%lX\", %g, %g)", (long) gobj, width, height);
}

void setFrameRectangleOp(GObject gobj, double x, double y, double width,
		double height) {
	putPipe("GArc.setFrameRectangle(\"0x%lX\", %g, %g, %g, %g)", (long) gobj, x,
			y, width, height);
}

void setFilledOp(GObject gobj, bool flag) {
	putPipe("GObject.setFilled(\"0x%lX\", %s)", (long) gobj, boolString(flag));
}

void setFillColorOp(GObject gobj, string color) {
	putPipe("GObject.setFillColor(\"0x%lX\", \"%s\")", (long) gobj, color);
}

/* GRect operations */

void createGRectOp(GRect rect, double width, double height) {
	initPlatform();
	putPipe("GRect.create(\"0x%lX\", %g, %g)", (long) rect, width, height);
}

void createGRoundRectOp(GRoundRect rect, double width, double height,
		double corner) {
	initPlatform();
	putPipe("GRoundRect.create(\"0x%lX\", %g, %g, %g)", (long) rect, width,
			height, corner);
}

void createG3DRectOp(G3DRect rect, double width, double height, bool raised) {
	initPlatform();
	putPipe("G3DRect.create(\"0x%lX\", %g, %g, %s)", (long) rect, width, height,
			boolString(raised));
}

void setRaisedOp(G3DRect rect, bool raised) {
	putPipe("G3DRect.setRaised(\"0x%lX\", %s)", (long) rect,
			boolString(raised));
}

/* GOval operations */

void createGOvalOp(GOval oval, double width, double height) {
	initPlatform();
	putPipe("GOval.create(\"0x%lX\", %g, %g)", (long) oval, width, height);
}

/* GLine operations */

void createGLineOp(GLine line, double x0, double y0, double x1, double y1) {
	initPlatform();
	putPipe("GLine.create(\"0x%lX\", %g, %g, %g, %g)", (long) line, x0, y0, x1,
			y1);
}

void setStartPointOp(GLine line, double x, double y) {
	putPipe("GLine.setStartPoint(\"0x%lX\", %g, %g)", (long) line, x, y);
}

void setEndPointOp(GLine line, double x, double y) {
	putPipe("GLine.setEndPoint(\"0x%lX\", %g, %g)", (long) line, x, y);
}

/* GArc operations */

void createGArcOp(GArc arc, double width, double height, double start,
		double sweep) {
	putPipe("GArc.create(\"0x%lX\", %g, %g, %g, %g)", (long) arc, width, height,
			start, sweep);
}

void setStartAngleOp(GArc arc, double angle) {
	putPipe("GArc.setStartAngle(\"0x%lX\", %g)", (long) arc, angle);
}

void setSweepAngleOp(GArc arc, double angle) {
	putPipe("GArc.setSweepAngle(\"0x%lX\", %g)", (long) arc, angle);
}

/* GLabel operations */

void createGLabelOp(GLabel label, string str) {
	str = quotedString(str);
	putPipe("GLabel.create(\"0x%lX\", %s)", (long) label, str);
	freeBlock(str);
}

void setFontOp(GLabel label, string font) {
	putPipe("GLabel.setFont(\"0x%lX\", \"%s\")", (long) label, font);
}

void setLabelOp(GLabel label, string str) {
	str = quotedString(str);
	putPipe("GLabel.setLabel(\"0x%lX\", %s)", (long) label, str);
	freeBlock(str);
}

double getFontAscentOp(GLabel label) {
	putPipe("GLabel.getFontAscent(\"0x%lX\")", (long) label);
	return getDouble();
}

double getFontDescentOp(GLabel label) {
	putPipe("GLabel.initDescent(\"0x%lX\")", (long) label);
	return getDouble();
}

GDimension getGLabelSizeOp(GLabel label) {
	putPipe("GLabel.getGLabelSize(\"0x%lX\")", (long) label);
	return getGDimension();
}

/* GImage operations */

GDimension createGImageOp(GObject gobj, string filename) {
	filename = quotedString(filename);
	putPipe("GImage.create(\"0x%lX\", %s)", (long) gobj, filename);
	freeBlock(filename);
	return getGDimension();
}

GPixelArray createGPixelArrayOp(GObject gobj) {
	putPipe("GImage.getPixelArray(\"0x%lX\")", (long) gobj);
	//return getGPixelArrayOp(gobj);
	return getGPixelArrayOp(gobj);
}

/* GPolygon operations */

void createGPolygonOp(GPolygon poly) {
	putPipe("GPolygon.create(\"0x%lX\")", (long) poly);
}

void addVertexOp(GPolygon poly, double x, double y) {
	putPipe("GPolygon.addVertex(\"0x%lX\", %g, %g)", (long) poly, x, y);
}

/* GCompound operations */

void createGCompoundOp(GCompound compound) {
	initPlatform();
	putPipe("GCompound.create(\"0x%lX\")", (long) compound);
}

void addOp(GCompound compound, GObject gobj) {
	putPipe("GCompound.add(\"0x%lX\", \"0x%lX\")", (long) compound,
			(long) gobj);
}

void removeOp(GObject gobj) {
	putPipe("GObject.remove(\"0x%lX\")", (long) gobj);
}

void sendForwardOp(GObject gobj) {
	putPipe("GObject.sendForward(\"0x%lX\")", (long) gobj);
}

void sendToFrontOp(GObject gobj) {
	putPipe("GObject.sendToFront(\"0x%lX\")", (long) gobj);
}

void sendBackwardOp(GObject gobj) {
	putPipe("GObject.sendBackward(\"0x%lX\")", (long) gobj);
}

void sendToBackOp(GObject gobj) {
	putPipe("GObject.sendToBack(\"0x%lX\")", (long) gobj);
}

/* GEvent operations */

GEvent getNextEventOp(int mask) {
	if (isEmpty(eventQueue)) {
		putPipe("GEvent.getNextEvent(%d)", mask);
		getResult();
		if (isEmpty(eventQueue))
			return NULL;
	}
	return dequeue(eventQueue);
}

GEvent waitForEventOp(int mask) {
	while (isEmpty(eventQueue)) {
		putPipe("GEvent.waitForEvent(%d)", mask);
		getResult();
	}
	return dequeue(eventQueue);
}

/* GInteractor operations */

void setActionCommandOp(GInteractor interactor, string cmd) {
	cmd = quotedString(cmd);
	putPipe("GInteractor.setActionCommand(\"0x%lX\", %s)", (long) interactor,
			cmd);
	freeBlock(cmd);
}

GDimension getSizeOp(GInteractor interactor) {
	putPipe("GInteractor.getSize(\"0x%lX\")", (long) interactor);
	return getGDimension();
}

void createGButtonOp(GButton button, string label) {
	registerSource(button);
	label = quotedString(label);
	putPipe("GButton.create(\"0x%lX\", %s)", (long) button, label);
	freeBlock(label);
}

void createGCheckBoxOp(GCheckBox chkbox, string label) {
	registerSource(chkbox);
	label = quotedString(label);
	putPipe("GCheckBox.create(\"0x%lX\", %s)", (long) chkbox, label);
	freeBlock(label);
}

bool isSelectedOp(GCheckBox chkbox) {
	putPipe("GCheckBox.isSelected(\"0x%lX\")", (long) chkbox);
	return getBool();
}

void setSelectedOp(GCheckBox chkbox, bool state) {
	putPipe("GCheckBox.isSelected(\"0x%lX\", %s)", (long) chkbox,
			boolString(state));
}

void createGSliderOp(GSlider slider, int min, int max, int value) {
	registerSource(slider);
	putPipe("GSlider.create(\"0x%lX\", %d, %d, %d)", (long) slider, min, max,
			value);
}

int getValueOp(GSlider slider) {
	putPipe("GSlider.getValue(\"0x%lX\")", (long) slider);
	return getInt();
}

void setValueOp(GSlider slider, int value) {
	putPipe("GSlider.setValue(\"0x%lX\", %d)", (long) slider, value);
}

void createGTextFieldOp(GTextField field, int nChars) {
	registerSource(field);
	putPipe("GTextField.create(\"0x%lX\", %d)", (long) field, nChars);
}

string getTextOp(GTextField field) {
	putPipe("GTextField.getText(\"0x%lX\")", (long) field);
	return getResult();
}

void setTextOp(GTextField field, string str) {
	str = quotedString(str);
	putPipe("GTextField.getText(\"0x%lX\", %s)", (long) field, str);
	freeBlock(str);
}

void createGChooserOp(GChooser chooser) {
	registerSource(chooser);
	putPipe("GChooser.create(\"0x%lX\")", (long) chooser);
}

void addItemOp(GChooser chooser, string item) {
	item = quotedString(item);
	putPipe("GChooser.addItem(\"0x%lX\", %s)", (long) chooser, item);
	freeBlock(item);
}

string getSelectedItemOp(GChooser chooser) {
	putPipe("GChooser.getSelectedItem(\"0x%lX\")", (long) chooser);
	return getResult();
}

void setSelectedItemOp(GChooser chooser, string item) {
	item = quotedString(item);
	putPipe("GChooser.setSelectedItem(\"0x%lX\", %s)", (long) chooser, item);
	freeBlock(item);
}

static void registerSource(GInteractor interactor) {
	string id;

	id = getId(interactor);
	putHashMap(sourceTable, id, interactor);
	freeBlock(id);
}
=======
/*
 * File: platform.c
 * ----------------
 * This file implements the platform-dependent aspects of the library,
 * primarily by passing commands to the Java back end.
 */

/*************************************************************************/
/* Stanford Portable Library                                             */
/* Copyright (C) 2013 by Eric Roberts <eroberts@cs.stanford.edu>         */
/*                                                                       */
/* This program is free software: you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* This program is distributed in the hope that it will be useful,       */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*************************************************************************/

#ifdef windows
#  include <windows.h>
#  include <shlwapi.h>
#  pragma comment(lib, "shlwapi.lib")
#  undef MOUSE_EVENT
#  undef KEY_EVENT
#  undef MOUSE_MOVED
#  undef HELP_KEY
#endif

#ifdef unixlike
#  define pause unixpause
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <dirent.h>
#  include <errno.h>
#  include <pwd.h>
#  include <unistd.h>
#  undef pause
#endif

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "cslib.h"
#include "filelib.h"
#include "gevents.h"
#include "gobjects.h"
#include "gtypes.h"
#include "gwindow.h"
#include "hashmap.h"
#include "platform.h"
#include "queue.h"
#include "simpio.h"
#include "strbuf.h"
#include "strlib.h"
#include "tokenscanner.h"

#include <string.h>
#include <math.h>
//#include <regex.h>

/* Constants */

#define MAX_ID_LENGTH 24

/* Package variables */

static string programName = "Test";
static FILE *pin;
static FILE *pout;
static bool initialized = false;
static Queue eventQueue;
static HashMap sourceTable;
static HashMap timerTable;
static HashMap windowTable;
static StringBuffer psb;

#ifdef windows
static HANDLE g_hChildStd_IN_Rd = NULL;
static HANDLE g_hChildStd_IN_Wr = NULL; // this is where we write!
static HANDLE g_hChildStd_OUT_Rd = NULL;
static HANDLE g_hChildStd_OUT_Wr = NULL;
#endif

/* Private function prototypes */

static void initPipe(void);
static void putPipe(string format, ...);
static string getResult();
static void getStatus();
static int getInt();
static int getDouble();
static GDimension getGDimension();
static GEvent parseEvent(string line);
static GEvent parseMouseEvent(TokenScanner scanner, EventType type);
static GEvent parseKeyEvent(TokenScanner scanner, EventType type);
static GEvent parseTimerEvent(TokenScanner scanner, EventType type);
static GEvent parseWindowEvent(TokenScanner scanner, EventType type);
static GEvent parseActionEvent(TokenScanner scanner, EventType type);
static string scanString(TokenScanner scanner);
static int scanInt(TokenScanner scanner);
static double scanDouble(TokenScanner scanner);
static bool scanBool(TokenScanner scanner);
static string quotedString(string str);
static string boolString(bool flag);
static void registerSource(GInteractor interactor);

void initPlatform(void) {
	if (!initialized)
		initPipe();
	sourceTable = newHashMap();
	timerTable = newHashMap();
	windowTable = newHashMap();
	eventQueue = newQueue();
	psb = newStringBuffer();
	initialized = true;
}

string getId(void *ptr) {
	char str[MAX_ID_LENGTH];
	sprintf(str, "0x%lX", (long) ptr);
	return copyString(str);
}

#ifdef windows

/* Windows implementation of interface to Java back end */

extern string *getMainArgArray(void);

static void initPipe() {
	SECURITY_ATTRIBUTES saAttr;
#ifdef PIPEDEBUG   
	fprintf(stderr, "->Initializing pipe.\n");
#endif

	// Set the bInheritHandle flag so pipe handles are inherited.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe for the child process's STDOUT.
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) )
	error(TEXT("StdoutRd CreatePipe"));

	/// Ensure the read handle to the pipe for STDOUT is not inherited.
	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
	error(TEXT("Stdout SetHandleInformation"));

	// Create a pipe for the child process's STDIN.
	if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
	error(TEXT("Stdin CreatePipe"));

	// Ensure the write handle to the pipe for STDIN is not inherited.
	if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
	error(TEXT("Stdin SetHandleInformation"));

	// Create the child process.
	startJavaBackendProcess();
}

static void startJavaBackendProcess() {
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE;

	// todo define, not here...
	TCHAR szCmdline[MAX_PATH*2];
	TCHAR szPath[MAX_PATH];

	if (getApplicationPath(szPath, MAX_PATH) == NULL)
	{
		error(TEXT("getApplicationPath"));
		return;
	}

	if (!SetCurrentDirectory(szPath))
	{
		error(TEXT("SetCurrentDirectory"));
		return;
	}

	ZeroMemory( szCmdline, MAX_PATH*2 );
	strncpy(szCmdline, TEXT("java.exe -jar "), 14);
	strncat(szCmdline, szPath, MAX_PATH);
	strncat(szCmdline, TEXT("\\spl.jar"), 8);
#ifdef PIPEDEBUG   
	strncat(szCmdline, TEXT(" -debug"), 7);
#endif

	// Set up members of the PROCESS_INFORMATION structure.
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

	// Set up members of the STARTUPINFO structure.
	// This structure specifies the STDIN and STDOUT handles for redirection.
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process.
	bSuccess = CreateProcess(NULL,
			szCmdline,// command line
			NULL,// process security attributes
			NULL,// primary thread security attributes
			TRUE,// handles are inherited
			0,// creation flags
			NULL,// use parent's environment
			NULL,// use parent's current directory
			&siStartInfo,// STARTUPINFO pointer
			&piProcInfo);// receives PROCESS_INFORMATION

	// If an error occurs, exit the application.
	if ( ! bSuccess )
	error(TEXT("CreateProcess"));
	else
	{
#ifdef PIPEDEBUG
		fprintf(stderr, "%s: %s\n", TEXT("CreateProcess successful"), szCmdline);
#endif

		// Close handles to the child process and its primary thread.
		// Some applications might keep these handles to monitor the status
		// of the child process, for example.

		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
	}
}

static void putPipe(string format, ...) {
	static DWORD cmdCount = 0;
	DWORD dwWritten;
	BOOL bSuccess = FALSE;

	//patch
	string cmd, jbetrace;
	va_list args;
	int capacity;

	clearStringBuffer(psb);
	va_start(args, format);
	capacity = printfCapacity(format, args);
	va_end(args);
	va_start(args, format);
	sbFormat(psb, capacity, format, args);
	va_end(args);
	cmd = getString(psb);
	int commandSize = strlen(cmd);
#ifdef PIPEDEBUG
	cmdCount++;
	fprintf(stderr, "Sent to pipe (%u): %s\n", cmdCount, cmd);
#endif

	jbetrace = getenv("JBETRACE");
	bSuccess = WriteFile(g_hChildStd_IN_Wr, cmd, commandSize, &dwWritten, NULL);
	bSuccess &= WriteFile(g_hChildStd_IN_Wr, "\n", 1, &dwWritten, NULL);

	if ( ! bSuccess ) error(TEXT("WriteFile to pipe"));
}

static string getResult() {
	int maxMessageLength = 300;

	string result;
	for(;;)
	{
		char message[maxMessageLength];
		readMessageFromBuffer(message, maxMessageLength);

		if (startsWith(message, "result:")) {
#ifdef PIPEDEBUG		  
			fprintf(stderr, "Result contains newLine char at: %d\n", findChar('\n', message, 0));
#endif		 
			result = substring(message, 7, stringLength(message));
#ifdef PIPEDEBUG		 
			fprintf(stderr, "Parsed result: %s\n", result);
#endif
			return result;
		} else if (startsWith(message, "event:")) {
#ifdef PIPEDEBUG		  
			fprintf(stderr, "Event contains newLine char at: %d\n", findChar('\n', message, 0));
			result = substring(message, 6, stringLength(message));
			fprintf(stderr, "Parsed event: %s\n", result);
#endif		 
			enqueue(eventQueue, parseEvent(message + 6));
		}
	}
}

void readMessageFromBuffer(char* message, int maxLength) {
	DWORD dwRead;
	int bufSize = 1;
	char messageBuffer[bufSize];
	BOOL bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	int bufferPosition = 0;
	char bufferedChar;
	while(bufferPosition < maxLength) {
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, messageBuffer, bufSize, &dwRead, NULL);
		bufferedChar = messageBuffer[0];
		if (bufferedChar == '\n') {
			break;
		}
		message[bufferPosition] = bufferedChar;
		bufferPosition++;
		if( !bSuccess || dwRead == 0 ) break;
	}
	message[bufferPosition-1] = '\0';
#ifdef PIPEDEBUG	  
	fprintf(stderr, "Read message from buffer: %s\n", message);
#endif	  
}

void readPixelArrayFromBuffer(unsigned int **array , int width, int height){
	DWORD dwRead;
	int bufSize = 1;
	char messageBuffer[bufSize];
	BOOL bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	int x,y = 0;
	char bufferedChar;
	DWORD color=0;
	while(y<height) {
		while(x < width) {
			for(int i=3;i>=0;){
				bSuccess = ReadFile(g_hChildStd_OUT_Rd, messageBuffer, bufSize, &dwRead, NULL);
				if( bSuccess ){
					color |= messageBuffer[0]<<8*i; 
					fprintf(stderr,"%x\n",color);
					i--;
				}
			}
			array[y][x]= color;
			//fprintf(stderr, "Get [%d,%d]: %u\n", y,x,color);
			color=0;
			x++;
			if( !bSuccess || dwRead == 0) break;
		}
		y++;
		x=0;
	}
#ifdef PIPEDEBUG	  
	fprintf(stderr, "Read PixelArray from buffer: [%d, %d]\n", y, x);
#endif	  
	
}

TCHAR* getApplicationPath(TCHAR* dest, size_t destSize) {
	if (!dest) return NULL;
	if (MAX_PATH > destSize) return NULL;
	DWORD length = GetModuleFileName( NULL, dest, destSize );
	PathRemoveFileSpec(dest);
	return dest;
}

#else

/* Linux/Mac implementation of interface to Java back end */

extern string *getMainArgArray(void);

static void initPipe(void) {
	int toJBE[2], fromJBE[2], child;
	string option, classpath;

	classpath = getenv("CLASSPATH");
	if (classpath == NULL) {
		classpath = "./spl.jar";
	}

	programName = getRoot(getTail(getMainArgArray()[0]));
	pipe(toJBE);
	pipe(fromJBE);
	child = fork();
	if (child == 0) {
		dup2(toJBE[0], 0);
		close(toJBE[0]);
		close(toJBE[1]);
		dup2(fromJBE[1], 1);
		close(fromJBE[0]);
		close(fromJBE[1]);
#ifdef __APPLE__
		option = concat("-Xdock:name=", programName);
		execlp("java", "java", option, "-classpath", classpath, "stanford/spl/JavaBackEnd", programName, NULL);
#else
		execlp("java", "java", "-classpath", classpath,
				"stanford/spl/JavaBackEnd", programName, NULL);
#endif
		error("Could not exec spl.jar");
	} else {
		pin = fdopen(fromJBE[0], "r");
		pout = fdopen(toJBE[1], "w");
		close(fromJBE[1]);
		close(toJBE[0]);
	}
}

static void putPipe(string format, ...) {
	string cmd, jbetrace;
	va_list args;
	int capacity;

	clearStringBuffer(psb);
	va_start(args, format);
	capacity = printfCapacity(format, args);
	va_end(args);
	va_start(args, format);
	sbFormat(psb, capacity, format, args);
	va_end(args);
	cmd = getString(psb);
	jbetrace = getenv("JBETRACE");
	if (jbetrace != NULL && (jbetrace[0] == 'T' || jbetrace[0] == 't')) {
		fprintf(stderr, "-> %s\n", cmd);
	}
	fprintf(pout, "%s\n", cmd);
	fflush(pout);
}

static string getResult() {
	string line, result, jbetrace, array;

	jbetrace = getenv("JBETRACE");
	while (true) {
		line = readLine(pin);
		if (line == NULL)
			exit(1);
		if (jbetrace != NULL && (jbetrace[0] == 'T' || jbetrace[0] == 't')) {
			fprintf(stderr, "<- %s\n", line);
		}
		if (startsWith(line, "result:")) {
			result = substring(line, 7, stringLength(line));
			freeBlock(line);
			return result;
		} else if (startsWith(line, "event:")) {
			enqueue(eventQueue, parseEvent(line + 6));
			freeBlock(line);
		} else if (startsWith(line, "array:")) {
			array = substring(line, 6, stringLength(line));
			return array;
		}
	}
}

#endif

static void getStatus() {
	string result;

	result = getResult();
	if (!stringEqual(result, "ok"))
		error(result);
	freeBlock(result);
}

static int getInt() {
	string str;
	int result;

	str = getResult();
	result = stringToInteger(str);
	freeBlock(str);
	return result;
}

static int getDouble() {
	string str;
	double result;

	str = getResult();
	result = stringToReal(str);
	freeBlock(str);
	return result;
}

static GDimension getGDimension() {
	string str;
	double width, height;

	str = getResult();
	sscanf(str, "GDimension(%lg,%lg)", &width, &height);
	freeBlock(str);
	return createGDimension(width, height);
}

static int getBool() {
	string str;
	bool result;

	str = getResult();
	result = (str[0] == 'T' || str[0] == 't');
	freeBlock(str);
	return result;
}

static GEvent parseEvent(string line) {
	TokenScanner scanner;
	string name;
	GEvent e;

	scanner = newTokenScanner();
	ignoreWhitespace(scanner);
	scanNumbers(scanner);
	scanStrings(scanner);
	setInputString(scanner, line);
	name = nextToken(scanner);
	e = NULL;
	if (stringEqual(name, "mousePressed")) {
		e = parseMouseEvent(scanner, MOUSE_PRESSED);
	} else if (stringEqual(name, "mouseReleased")) {
		e = parseMouseEvent(scanner, MOUSE_RELEASED);
	} else if (stringEqual(name, "mouseClicked")) {
		e = parseMouseEvent(scanner, MOUSE_CLICKED);
	} else if (stringEqual(name, "mouseMoved")) {
		e = parseMouseEvent(scanner, MOUSE_MOVED);
	} else if (stringEqual(name, "mouseDragged")) {
		e = parseMouseEvent(scanner, MOUSE_DRAGGED);
	} else if (stringEqual(name, "keyPressed")) {
		e = parseKeyEvent(scanner, KEY_PRESSED);
	} else if (stringEqual(name, "keyReleased")) {
		e = parseKeyEvent(scanner, KEY_RELEASED);
	} else if (stringEqual(name, "keyTyped")) {
		e = parseKeyEvent(scanner, KEY_TYPED);
	} else if (stringEqual(name, "actionPerformed")) {
		e = parseActionEvent(scanner, ACTION_PERFORMED);
	} else if (stringEqual(name, "timerTicked")) {
		e = parseTimerEvent(scanner, TIMER_TICKED);
	} else if (stringEqual(name, "windowClosed")) {
		e = parseWindowEvent(scanner, WINDOW_CLOSED);
		closeGWindow(getGWindow(e));
	} else if (stringEqual(name, "windowResized")) {
		e = parseWindowEvent(scanner, WINDOW_RESIZED);
	} else if (stringEqual(name, "lastWindowClosed")) {
		exit(0);
	} else {
		/* Ignore for now */
	}
	freeTokenScanner(scanner);
	freeBlock(name);
	return e;
}

static GEvent parseMouseEvent(TokenScanner scanner, EventType type) {
	string id;
	int modifiers;
	double time, x, y;
	GEvent e;

	verifyToken(scanner, "(");
	id = scanString(scanner);
	verifyToken(scanner, ",");
	time = scanDouble(scanner);
	verifyToken(scanner, ",");
	modifiers = scanInt(scanner);
	verifyToken(scanner, ",");
	x = scanDouble(scanner);
	verifyToken(scanner, ",");
	y = scanDouble(scanner);
	verifyToken(scanner, ")");
	e = newGMouseEvent(type, get(windowTable, id), x, y);
	setEventTime(e, time);
	setModifiers(e, modifiers);
	freeBlock(id);
	return e;
}

static GEvent parseKeyEvent(TokenScanner scanner, EventType type) {
	string id;
	int modifiers, keyChar, keyCode;
	double time;
	GEvent e;

	verifyToken(scanner, "(");
	id = scanString(scanner);
	verifyToken(scanner, ",");
	time = scanDouble(scanner);
	verifyToken(scanner, ",");
	modifiers = scanInt(scanner);
	verifyToken(scanner, ",");
	keyChar = scanInt(scanner);
	verifyToken(scanner, ",");
	keyCode = scanInt(scanner);
	verifyToken(scanner, ")");
	e = newGKeyEvent(type, get(windowTable, id), keyChar, keyCode);
	setEventTime(e, time);
	setModifiers(e, modifiers);
	freeBlock(id);
	return e;
}

static GEvent parseTimerEvent(TokenScanner scanner, EventType type) {
	string id;
	double time;
	GEvent e;

	verifyToken(scanner, "(");
	id = scanString(scanner);
	verifyToken(scanner, ",");
	time = scanDouble(scanner);
	verifyToken(scanner, ")");
	e = newGTimerEvent(type, get(timerTable, id));
	setEventTime(e, time);
	freeBlock(id);
	return e;
}

static GEvent parseWindowEvent(TokenScanner scanner, EventType type) {
	string id;
	double time;
	GEvent e;

	verifyToken(scanner, "(");
	id = scanString(scanner);
	verifyToken(scanner, ",");
	time = scanDouble(scanner);
	verifyToken(scanner, ")");
	e = newGWindowEvent(type, get(windowTable, id));
	setEventTime(e, time);
	freeBlock(id);
	return e;
}

static GEvent parseActionEvent(TokenScanner scanner, EventType type) {
	string id, action;
	double time;
	GEvent e;

	verifyToken(scanner, "(");
	id = scanString(scanner);
	verifyToken(scanner, ",");
	action = scanString(scanner);
	verifyToken(scanner, ",");
	time = scanDouble(scanner);
	verifyToken(scanner, ")");
	e = newGActionEvent(type, get(sourceTable, id), action);
	setEventTime(e, time);
	freeBlock(id);
	freeBlock(action);
	return e;
}

static string scanString(TokenScanner scanner) {
	string token, result;

	token = nextToken(scanner);
	result = getStringValue(token);
	freeBlock(token);
	return result;
}

static int scanInt(TokenScanner scanner) {
	string token;
	int sign, result;

	token = nextToken(scanner);
	sign = 1;
	if (stringEqual(token, "-")) {
		sign = -1;
		freeBlock(token);
		token = nextToken(scanner);
	}
	result = stringToInteger(token);
	freeBlock(token);
	return sign * result;
}

static double scanDouble(TokenScanner scanner) {
	string token;
	double result;
	int sign;

	token = nextToken(scanner);
	sign = 1;
	if (stringEqual(token, "-")) {
		sign = -1;
		freeBlock(token);
		token = nextToken(scanner);
	}
	result = stringToReal(token);
	freeBlock(token);
	return sign * result;
}

static bool scanBool(TokenScanner scanner) {
	string token;
	bool result;

	token = nextToken(scanner);
	result = startsWith(token, "t");
	freeBlock(token);
	return result;
}

static string quotedString(string str) {
	StringBuffer sb;
	char ch;
	int i;

	sb = newStringBuffer();
	pushChar(sb, '"');
	for (i = 0; str[i] != '\0'; i++) {
		ch = str[i];
		switch (ch) {
		case '\a':
			sbprintf(sb, "\\a");
			break;
		case '\b':
			sbprintf(sb, "\\b");
			break;
		case '\f':
			sbprintf(sb, "\\f");
			break;
		case '\n':
			sbprintf(sb, "\\n");
			break;
		case '\r':
			sbprintf(sb, "\\r");
			break;
		case '\t':
			sbprintf(sb, "\\t");
			break;
		case '\v':
			sbprintf(sb, "\\v");
			break;
		case '"':
			sbprintf(sb, "\\%03o", ch);
			break;
		case '\\':
			sbprintf(sb, "\\\\");
			break;
		default:
			if (isprint(ch)) {
				pushChar(sb, ch);
			} else {
				sbprintf(sb, "\\%03o", ch & 0xFF);
			}
		}
	}
	pushChar(sb, '"');
	str = copyString(getString(sb));
	freeStringBuffer(sb);
	return str;
}

static string boolString(bool flag) {
	return (flag) ? "true" : "false";
}

/* GWindow operations */

void createGWindowOp(GWindow gw, double width, double height, GObject top) {
	string id;

	initPlatform();
	id = getId(gw);
	put(windowTable, id, gw);
	freeBlock(id);
	putPipe("GWindow.create(\"0x%lX\", %g, %g, \"0x%lX\")", (long) gw, width,
			height, (long) top);
	//milliseconds on windows, secs on linux
#ifdef windows
	Sleep(2000);
#else
	sleep(2);
#endif
	getStatus();
}

void deleteGWindowOp(GWindow gw) {
	string id;

	id = getId(gw);
	remove(windowTable, id);
	freeBlock(id);
	putPipe("GWindow.delete(\"0x%lX\")", (long) gw);
}

void closeGWindowOp(GWindow gw) {
	putPipe("GWindow.close(\"0x%lX\")", (long) gw);
}

void requestFocusOp(GWindow gw) {
	putPipe("GWindow.requestFocus(\"0x%lX\")", (long) gw);
}

void clearOp(GWindow gw) {
	putPipe("GWindow.clear(\"0x%lX\")", (long) gw);
}

void repaintOp(GWindow gw) {
	putPipe("GWindow.repaint(\"0x%lX\")", (long) gw);
}

void setVisibleGWindowOp(GWindow gw, bool flag) {
	putPipe("GWindow.setVisible(\"0x%lX\", %s)", (long) gw, boolString(flag));
}

void setResizableOp(GWindow gw, bool flag) {
	putPipe("GWindow.setResizable(\"0x%lX\", %s)", (long) gw, boolString(flag));
}

void setWindowTitleOp(GWindow gw, string title) {
	title = quotedString(title);
	putPipe("GWindow.setWindowTitle(\"0x%lX\", %s)", (long) gw, title);
	freeBlock(title);
}

void setRegionAlignmentOp(GWindow gw, string region, string align) {
	putPipe("GWindow.setRegionAlignment(\"0x%lX\", \"%s\", \"%s\")", (long) gw,
			region, align);
}

void addToRegionOp(GWindow gw, GObject gobj, string region) {
	putPipe("GWindow.addToRegion(\"0x%lX\", \"0x%lX\", \"%s\")", (long) gw,
			(long) gobj, region);
}

double getScreenWidthOp(void) {
	putPipe("GWindow.getScreenWidth()");
	return getDouble();
}

double getScreenHeightOp(void) {
	putPipe("GWindow.getScreenHeight()");
	return getDouble();
}

void exitGraphicsOp(void) {
	putPipe("GWindow.exitGraphics()");
	exit(0);
}

/* GTimer operations */

void createTimerOp(GTimer timer, double delay) {
	string id;

	id = getId(timer);
	putHashMap(timerTable, id, timer);
	freeBlock(id);
	putPipe("GTimer.create(\"0x%lX\", %g)", (long) timer, delay);
}

void deleteTimerOp(GTimer timer) {
	string id;

	id = getId(timer);
	putHashMap(timerTable, id, timer);
	freeBlock(id);
	putPipe("GTimer.deleteTimer(\"0x%lX\")", (long) timer);
}

void startTimerOp(GTimer timer) {
	putPipe("GTimer.startTimer(\"0x%lX\")", (long) timer);
}

void stopTimerOp(GTimer timer) {
	putPipe("GTimer.stopTimer(\"0x%lX\")", (long) timer);
}

void pauseOp(double milliseconds) {
	putPipe("GTimer.pause(%g)", milliseconds);
	getStatus();
}

/* Sound operations */

void createSoundOp(Sound sound, string filename) {
	filename = quotedString(filename);
	putPipe("Sound.create(\"0x%lX\", %s)", (long) sound, filename);
	freeBlock(filename);
	getStatus();
}

void deleteSoundOp(Sound sound) {
	putPipe("Sound.delete(\"0x%lX\")", (long) sound);
}

void playSoundOp(Sound sound) {
	putPipe("Sound.play(\"0x%lX\")", (long) sound);
}

/* GObject operations */

void deleteGObjectOp(GObject gobj) {
	putPipe("GObject.delete(\"0x%lX\")", (long) gobj);
}

void setVisibleGObjectOp(GObject gobj, bool flag) {
	putPipe("GObject.setVisible(\"0x%lX\", %s)", (long) gobj, boolString(flag));
}

void setColorOp(GObject gobj, string color) {
	putPipe("GObject.setColor(\"0x%lX\", \"%s\")", (long) gobj, color);
}

void setLocationOp(GObject gobj, double x, double y) {
	putPipe("GObject.setLocation(\"0x%lX\", %g, %g)", (long) gobj, x, y);
}

void drawOp(GWindow gw, GObject gobj) {
	putPipe("GWindow.draw(\"0x%lX\", \"0x%lX\")", (long) gw, (long) gobj);
}

void setSizeOp(GObject gobj, double width, double height) {
	putPipe("GObject.setSize(\"0x%lX\", %g, %g)", (long) gobj, width, height);
}

void setFrameRectangleOp(GObject gobj, double x, double y, double width,
		double height) {
	putPipe("GArc.setFrameRectangle(\"0x%lX\", %g, %g, %g, %g)", (long) gobj, x,
			y, width, height);
}

void setFilledOp(GObject gobj, bool flag) {
	putPipe("GObject.setFilled(\"0x%lX\", %s)", (long) gobj, boolString(flag));
}

void setFillColorOp(GObject gobj, string color) {
	putPipe("GObject.setFillColor(\"0x%lX\", \"%s\")", (long) gobj, color);
}

/* GRect operations */

void createGRectOp(GRect rect, double width, double height) {
	initPlatform();
	putPipe("GRect.create(\"0x%lX\", %g, %g)", (long) rect, width, height);
}

void createGRoundRectOp(GRoundRect rect, double width, double height,
		double corner) {
	initPlatform();
	putPipe("GRoundRect.create(\"0x%lX\", %g, %g, %g)", (long) rect, width,
			height, corner);
}

void createG3DRectOp(G3DRect rect, double width, double height, bool raised) {
	initPlatform();
	putPipe("G3DRect.create(\"0x%lX\", %g, %g, %s)", (long) rect, width, height,
			boolString(raised));
}

void setRaisedOp(G3DRect rect, bool raised) {
	putPipe("G3DRect.setRaised(\"0x%lX\", %s)", (long) rect,
			boolString(raised));
}

/* GOval operations */

void createGOvalOp(GOval oval, double width, double height) {
	initPlatform();
	putPipe("GOval.create(\"0x%lX\", %g, %g)", (long) oval, width, height);
}

/* GLine operations */

void createGLineOp(GLine line, double x0, double y0, double x1, double y1) {
	initPlatform();
	putPipe("GLine.create(\"0x%lX\", %g, %g, %g, %g)", (long) line, x0, y0, x1,
			y1);
}

void setStartPointOp(GLine line, double x, double y) {
	putPipe("GLine.setStartPoint(\"0x%lX\", %g, %g)", (long) line, x, y);
}

void setEndPointOp(GLine line, double x, double y) {
	putPipe("GLine.setEndPoint(\"0x%lX\", %g, %g)", (long) line, x, y);
}

/* GArc operations */

void createGArcOp(GArc arc, double width, double height, double start,
		double sweep) {
	putPipe("GArc.create(\"0x%lX\", %g, %g, %g, %g)", (long) arc, width, height,
			start, sweep);
}

void setStartAngleOp(GArc arc, double angle) {
	putPipe("GArc.setStartAngle(\"0x%lX\", %g)", (long) arc, angle);
}

void setSweepAngleOp(GArc arc, double angle) {
	putPipe("GArc.setSweepAngle(\"0x%lX\", %g)", (long) arc, angle);
}

/* GLabel operations */

void createGLabelOp(GLabel label, string str) {
	str = quotedString(str);
	putPipe("GLabel.create(\"0x%lX\", %s)", (long) label, str);
	freeBlock(str);
}

void setFontOp(GLabel label, string font) {
	putPipe("GLabel.setFont(\"0x%lX\", \"%s\")", (long) label, font);
}

void setLabelOp(GLabel label, string str) {
	str = quotedString(str);
	putPipe("GLabel.setLabel(\"0x%lX\", %s)", (long) label, str);
	freeBlock(str);
}

double getFontAscentOp(GLabel label) {
	putPipe("GLabel.getFontAscent(\"0x%lX\")", (long) label);
	return getDouble();
}

double getFontDescentOp(GLabel label) {
	putPipe("GLabel.initDescent(\"0x%lX\")", (long) label);
	return getDouble();
}

GDimension getGLabelSizeOp(GLabel label) {
	putPipe("GLabel.getGLabelSize(\"0x%lX\")", (long) label);
	return getGDimension();
}

/* GImage operations */

GDimension createGImageOp(GObject gobj, string filename) {
	filename = quotedString(filename);
	putPipe("GImage.create(\"0x%lX\", %s)", (long) gobj, filename);
	freeBlock(filename);
	return getGDimension();
}

/* GPolygon operations */

void createGPolygonOp(GPolygon poly) {
	putPipe("GPolygon.create(\"0x%lX\")", (long) poly);
}

void addVertexOp(GPolygon poly, double x, double y) {
	putPipe("GPolygon.addVertex(\"0x%lX\", %g, %g)", (long) poly, x, y);
}

/* GCompound operations */

void createGCompoundOp(GCompound compound) {
	initPlatform();
	putPipe("GCompound.create(\"0x%lX\")", (long) compound);
}

void addOp(GCompound compound, GObject gobj) {
	putPipe("GCompound.add(\"0x%lX\", \"0x%lX\")", (long) compound,
			(long) gobj);
}

void removeOp(GObject gobj) {
	putPipe("GObject.remove(\"0x%lX\")", (long) gobj);
}

void sendForwardOp(GObject gobj) {
	putPipe("GObject.sendForward(\"0x%lX\")", (long) gobj);
}

void sendToFrontOp(GObject gobj) {
	putPipe("GObject.sendToFront(\"0x%lX\")", (long) gobj);
}

void sendBackwardOp(GObject gobj) {
	putPipe("GObject.sendBackward(\"0x%lX\")", (long) gobj);
}

void sendToBackOp(GObject gobj) {
	putPipe("GObject.sendToBack(\"0x%lX\")", (long) gobj);
}

/* GEvent operations */

GEvent getNextEventOp(int mask) {
	if (isEmpty(eventQueue)) {
		putPipe("GEvent.getNextEvent(%d)", mask);
		getResult();
		if (isEmpty(eventQueue))
			return NULL;
	}
	return dequeue(eventQueue);
}

GEvent waitForEventOp(int mask) {
	while (isEmpty(eventQueue)) {
		putPipe("GEvent.waitForEvent(%d)", mask);
		getResult();
	}
	return dequeue(eventQueue);
}

/* GInteractor operations */

void setActionCommandOp(GInteractor interactor, string cmd) {
	cmd = quotedString(cmd);
	putPipe("GInteractor.setActionCommand(\"0x%lX\", %s)", (long) interactor,
			cmd);
	freeBlock(cmd);
}

GDimension getSizeOp(GInteractor interactor) {
	putPipe("GInteractor.getSize(\"0x%lX\")", (long) interactor);
	return getGDimension();
}

void createGButtonOp(GButton button, string label) {
	registerSource(button);
	label = quotedString(label);
	putPipe("GButton.create(\"0x%lX\", %s)", (long) button, label);
	freeBlock(label);
}

void createGCheckBoxOp(GCheckBox chkbox, string label) {
	registerSource(chkbox);
	label = quotedString(label);
	putPipe("GCheckBox.create(\"0x%lX\", %s)", (long) chkbox, label);
	freeBlock(label);
}

bool isSelectedOp(GCheckBox chkbox) {
	putPipe("GCheckBox.isSelected(\"0x%lX\")", (long) chkbox);
	return getBool();
}

void setSelectedOp(GCheckBox chkbox, bool state) {
	putPipe("GCheckBox.isSelected(\"0x%lX\", %s)", (long) chkbox,
			boolString(state));
}

void createGSliderOp(GSlider slider, int min, int max, int value) {
	registerSource(slider);
	putPipe("GSlider.create(\"0x%lX\", %d, %d, %d)", (long) slider, min, max,
			value);
}

int getValueOp(GSlider slider) {
	putPipe("GSlider.getValue(\"0x%lX\")", (long) slider);
	return getInt();
}

void setValueOp(GSlider slider, int value) {
	putPipe("GSlider.setValue(\"0x%lX\", %d)", (long) slider, value);
}

void createGTextFieldOp(GTextField field, int nChars) {
	registerSource(field);
	putPipe("GTextField.create(\"0x%lX\", %d)", (long) field, nChars);
}

string getTextOp(GTextField field) {
	putPipe("GTextField.getText(\"0x%lX\")", (long) field);
	return getResult();
}

void setTextOp(GTextField field, string str) {
	str = quotedString(str);
	putPipe("GTextField.getText(\"0x%lX\", %s)", (long) field, str);
	freeBlock(str);
}

void createGChooserOp(GChooser chooser) {
	registerSource(chooser);
	putPipe("GChooser.create(\"0x%lX\")", (long) chooser);
}

void addItemOp(GChooser chooser, string item) {
	item = quotedString(item);
	putPipe("GChooser.addItem(\"0x%lX\", %s)", (long) chooser, item);
	freeBlock(item);
}

string getSelectedItemOp(GChooser chooser) {
	putPipe("GChooser.getSelectedItem(\"0x%lX\")", (long) chooser);
	return getResult();
}

void setSelectedItemOp(GChooser chooser, string item) {
	item = quotedString(item);
	putPipe("GChooser.setSelectedItem(\"0x%lX\", %s)", (long) chooser, item);
	freeBlock(item);
}

static void registerSource(GInteractor interactor) {
	string id;

	id = getId(interactor);
	putHashMap(sourceTable, id, interactor);
	freeBlock(id);
}
>>>>>>> 590e64b
