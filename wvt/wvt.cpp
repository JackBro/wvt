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

#include <Windows.h>

#include <cassert>
#include <iostream>
#include <fstream>

inline BOOL debugging() {
#ifdef _DEBUG
  return true;
#else
  return false;
#endif
}

class WIN_Context {
  HINSTANCE hInstance;

public:

  WIN_Context(HINSTANCE inst) {
    hInstance = inst;
  }

  void messageLoop();
  BOOL registerWindowClass();
  BOOL createMainWindow(int nCmdShow);

  static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT uint, WPARAM wParam, LPARAM lParam);

  static const TCHAR windowClassName[];
  static const  TCHAR windowTitleName[];
  static const int windowInitialWidth;
  static const int windowInitialHeight;
};

const TCHAR WIN_Context::windowClassName[] = TEXT("testClass");
const TCHAR WIN_Context::windowTitleName[] = TEXT("testTitle");
const int WIN_Context::windowInitialWidth = 512;
const int WIN_Context::windowInitialHeight = 316;

int CALLBACK WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if(debugging()) {
      BOOL allocOk = AllocConsole();
      assert(allocOk);

      std::ofstream conOut("con");
      conOut << "DEBUGGING: ON";
      conOut.close();
    }

    WIN_Context* winContext = new WIN_Context(hInstance);

    if( winContext->registerWindowClass() &&
      winContext->createMainWindow(nCmdShow) ) {
        winContext->messageLoop();
    } else {
      return EXIT_FAILURE;
    }

    delete winContext;
    return EXIT_SUCCESS;
}

void WIN_Context::messageLoop() {
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

BOOL WIN_Context::registerWindowClass() {
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

BOOL WIN_Context::createMainWindow(int nCmdShow)  {
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

LRESULT CALLBACK WIN_Context::windowProcedure(
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
