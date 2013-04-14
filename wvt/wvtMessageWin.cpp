/*
 * wvtMessageWin.cpp - windows terminal emulator
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
#include "wvtMessageWin.h"

const TCHAR wvtMessageWin::messageWindowClassName[] = TEXT("wvtMessageClass");

BOOL wvtMessageWin::registerWindowClass() {
  WNDCLASSEX wcx;

  wcx.cbSize = sizeof(wcx);
  wcx.style = 0;
  wcx.lpfnWndProc = windowProcedure;
  wcx.cbClsExtra = 0;
  wcx.cbWndExtra = 0;
  wcx.hInstance = hInstance;
  wcx.hIcon = NULL;
  wcx.hCursor = NULL;
  wcx.hbrBackground = NULL;
  wcx.lpszMenuName =  NULL;
  wcx.lpszClassName = messageWindowClassName;
  wcx.hIconSm = NULL;

  return RegisterClassEx(&wcx);
}

BOOL wvtMessageWin::createMessageWindow()  {
  hwnd = CreateWindowEx(
    0,
    messageWindowClassName,
    NULL,
    0,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    0,
    0,
    HWND_MESSAGE,
    (HMENU) NULL,
    hInstance,
    (LPVOID) this);

  if (hwnd == NULL) {
    SCREAM_AND_DIE("CreateWindowEx(... root ...) failed");
  }

  UpdateWindow(hwnd);

  return TRUE;
}

wvtMessageWin::wvtMessageWin(HINSTANCE inst) : wvtWin(inst) {

}

BOOL wvtMessageWin::init() {
  return registerWindowClass() && createMessageWindow();
}

void wvtMessageWin::loop() {
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

LRESULT CALLBACK wvtMessageWin::windowProcedure(
  HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
      case WM_CLOSE:
        if(!DestroyWindow(hwnd)) {
           /* TODO: signal error */
        } else {
          PostQuitMessage(EXIT_SUCCESS);
        }

        return 0;

      default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
