/*
 * wvtWin.cpp - windows terminal emulator
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
#include "wvtWin.h"

const TCHAR wvtWin::windowClassName[] = TEXT("wvtClass");
const TCHAR wvtWin::windowTitleName[] = TEXT("wvt");
const int wvtWin::windowInitialWidth = 512;
const int wvtWin::windowInitialHeight = 316;

void wvtWin::messageLoop() {
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

BOOL wvtWin::registerWindowClass() {
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

BOOL wvtWin::createMainWindow(int nCmdShow)  {
  HWND hwnd = CreateWindowEx(
    0,
    windowClassName,
    windowTitleName,
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    windowInitialWidth,
    windowInitialHeight,
    (HWND) NULL,
    (HMENU) NULL,
    hInstance,
    (LPVOID) NULL);

  if (!hwnd) {
    return FALSE;
  }

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  return TRUE;
}

LRESULT CALLBACK wvtWin::windowProcedure(
  HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    if(debugging()) {
      std::ofstream conOut("con");
      conOut << "event " << msg << std::endl;
      conOut.close();
    }

    switch(msg) {
    case WM_CREATE:
      /* initialize data */

      /* create widgets */

      return DefWindowProc(hwnd, msg, wParam, lParam);

    case WM_PAINT:

      /* draw widgets */

      return DefWindowProc(hwnd, msg, wParam, lParam);

    case WM_CLOSE:
      if(!DestroyWindow(hwnd)) {

        /* TODO: signal error */

      } else {

        /* destroy data */

        PostQuitMessage(EXIT_SUCCESS);
      }

      return 0;

    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
