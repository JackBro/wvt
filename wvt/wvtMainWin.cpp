/*
 * wvtMainWin.cpp - windows terminal emulator
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

#include "wvt.h"
#include "wvtMainWin.h"

const TCHAR wvtMainWin::windowClassName[] = TEXT("wvtClass");
const TCHAR wvtMainWin::windowTitleName[] = TEXT("wvt");
const int wvtMainWin::windowInitialWidth = 512;
const int wvtMainWin::windowInitialHeight = 316;

/* public methods */

wvtMainWin::wvtMainWin(HINSTANCE inst) : wvtWin(inst) {
   childPane = NULL;
}

BOOL wvtMainWin::init() {
   return registerWindowClass() && createMainWindow(SW_SHOW);
}

void wvtMainWin::loop() {
  MSG msg;
  BOOL bRet;

  while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0) {
    if( bRet == -1 ) {
      /* TODO: signal error */
    } else {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

/* private methods */

BOOL wvtMainWin::registerWindowClass() {
  WNDCLASSEX wcx;

  wcx.cbSize = sizeof(wcx);
  wcx.style = CS_HREDRAW | CS_VREDRAW;
  wcx.lpfnWndProc = windowProcedure;
  wcx.cbClsExtra = 0;
  wcx.cbWndExtra = 0;
  wcx.hInstance = hInstance;
  wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcx.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  wcx.lpszMenuName =  NULL;
  wcx.lpszClassName = windowClassName;
  wcx.hIconSm = NULL;

  return RegisterClassEx(&wcx);
}

BOOL wvtMainWin::createMainWindow(const int nCmdShow)  {
  hwnd = CreateWindowEx(
    0,
    windowClassName,
    windowTitleName,
    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    windowInitialWidth,
    windowInitialHeight,
    (HWND) NULL,
    (HMENU) NULL,
    hInstance,
    (LPVOID) this);

  if (hwnd == NULL) {
    SCREAM_AND_DIE("CreateWindowEx(... root ...) failed");
  }

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  return TRUE;
}

void wvtMainWin::constructChildPane(const HWND hwnd) {
   assert(childPane == NULL);
   assert(hwnd != NULL);

   childPane = CreateWindowEx(
      WS_EX_ACCEPTFILES | WS_EX_CLIENTEDGE | WS_EX_TOPMOST,
      TEXT("Edit"),
      NULL,
      WS_CHILD | WS_VSCROLL | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
      0, 0, 0, 0,
      hwnd,
      NULL,
      hInstance,
      this );

   if(childPane == NULL) {
      SCREAM_AND_DIE("CreateWindowEx(... child ...) failed");
   }
}

void wvtMainWin::destroyChildPane() {
   assert(childPane != NULL);

   if(DestroyWindow(childPane) == 0) {
      /* TODO: signal error */
   } else {
      childPane = NULL;
   }
}

LRESULT CALLBACK wvtMainWin::windowProcedure(
  HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    static wvtMainWin* enclosingInst = NULL;

    if(debugging()) {
      std::ofstream conOut("con");
      conOut << "event " << msg << std::endl;
      conOut.close();
    }

    switch(msg) {
    case WM_CREATE:
      /* initialize data */

      assert(enclosingInst == NULL);
      enclosingInst = (wvtMainWin*) ((CREATESTRUCT*)lParam)->lpCreateParams;
      assert(enclosingInst != NULL);

      /* create widgets. note: WM_CREATE is sent before CreateWindowEx returns, so we must
       * pass hwnd here, too */
      enclosingInst->constructChildPane(hwnd);

      if(debugging()) {
         std::ofstream conOut("con");
         conOut << "CREATE " << msg << std::endl;
         conOut.close();
      }

      return 0;

    case WM_SIZE:
      if(enclosingInst != NULL && enclosingInst->retrieveChildPane() != NULL) {
         if(MoveWindow(enclosingInst->retrieveChildPane(), 
            0, 0,
            LOWORD(lParam),
            HIWORD(lParam),
            TRUE) == 0) {
               SCREAM_AND_DIE("MoveWindow(... child ...) failed");
         }
      }
      return 0;

    case WM_CLOSE:
      assert(enclosingInst != NULL);

      if(!DestroyWindow(hwnd)) {

        /* TODO: signal error */

      } else {

        enclosingInst->destroyChildPane();
        PostQuitMessage(EXIT_SUCCESS);
      }

      return 0;

    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
