/*
 * wvt.cpp - windows terminal emulator
 *
 * Copyright (c) 2013 Joseph M DeLiso
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>

#include <cassert>
#include <iostream>
#include <fstream>

#include <sstream>
#include <string>

#include "wvt.h"
#include "wvtMainWin.h"
#include "wvtMessageWin.h"

DWORD WINAPI mesgThread(LPVOID lpParam);
LONG WINAPI crashHandler(EXCEPTION_POINTERS * exceptionInfo);

int CALLBACK WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    /* allocate a console if we're debugging */
    if(debugging()) {
      BOOL allocOk = AllocConsole();
      assert(allocOk);

      std::ofstream conOut("con");
      conOut << "DEBUGGING: ON" << std::endl;
      conOut.close();
    }

    /* install unfiltered exception handler to try and produce better crash data */
    SetUnhandledExceptionFilter(crashHandler);

    /* declare message win and thread handle */
    wvtMessageWin mesgWin(hInstance);
    HANDLE mesgThreadHandle;
    DWORD mesgThreadId;

    /* create message win thread */
    if((mesgThreadHandle = CreateThread(
      NULL,
      0,
      mesgThread,
      &mesgWin,
      0,
      &mesgThreadId)) == NULL) {
        SCREAM_AND_DIE("CreateThread(... mesgThread ...)");
    }

    /* create top-level win and loop */
    wvtMainWin win(hInstance);

    if(win.init()) {
      win.loop();
    } else {
      return EXIT_FAILURE;
    }

    LRESULT msgResult = PostMessage(mesgWin.getHWND(), WM_CLOSE, 0, 0);

    /* TODO: examine this */
    (void) msgResult;

    /* wait for message thread */
    if(WaitForSingleObject(mesgThreadHandle, INFINITE)
      != WAIT_OBJECT_0) {
        SCREAM_AND_DIE("WaitForSingleObject");
    }

    return EXIT_SUCCESS;
}

DWORD WINAPI mesgThread(LPVOID lpParam) {
  wvtMessageWin* mesgWinPtr = (wvtMessageWin*) lpParam;

  if(mesgWinPtr->init()) {
    mesgWinPtr->loop();
  }

  return 0;
}

/* a little utility function that returns true if this is a debug build */
BOOL debugging() {
#ifdef _DEBUG
  return true;
#else
  return false;
#endif
}

/* this function should be called immediately following the failure indicating
* return code of a win32 api function. it will create a messagebox with as
* detailed of a description of the error as possible. you shouldn't call this
* function directly, but should use the provided macro instead so that the
* message includes source information. see SCREAM_AND_DIE(msg). */
void __screamAndDie(const char* file, const int line, const char* userMessage) {
  static const int MAX_ERR_MSG_LEN = 1024;
  std::wstringstream errStream;
  LPTSTR sysErrMsg;

  /* prolog in error message lifted from macros */
  errStream
    << "in " << file
    << " at line " << line
    << " : " << userMessage
    << " | ";

  /* it doesn't make sense to check this for failure */
  DWORD errCode = GetLastError();

  /* attempt to get windows to pretty-print the error code */
  if(FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    errCode,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR)&sysErrMsg,
    0, NULL ) == 0) {

      /* couldn't retrieve system formatted error string */
      errStream << "unspecified: " << errCode;

  } else {
    /* copy the pretty printed failure reason to the errStream */
    errStream << sysErrMsg;

    /* and return it to the heap */
    if(LocalFree(sysErrMsg) != NULL) {
      /* failed to free errMsgStr! */
    }
  }

  /* allocate a buffer on the stack and write the stringstream back to it */
  TCHAR errMsg[MAX_ERR_MSG_LEN];
  memset(errMsg, '\0', sizeof(errMsg));
  errStream.read(errMsg, sizeof(errMsg));

  /* create the error messagebox (scream) */
  MessageBox(NULL, errMsg, TEXT("FATAL"), MB_ICONERROR);

  /* die */
  ExitProcess(errCode);
}

#pragma warning( suppress: 4702 ) /* for suppressing dead code warning in release mode */
LONG WINAPI crashHandler(EXCEPTION_POINTERS * exceptionInfo) {
  (void) exceptionInfo;

  SCREAM_AND_DIE("caught unfiltered exception");

  return 0; /* NOTE: this never executes */
}
