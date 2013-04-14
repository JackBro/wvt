/*
 * wvtMainWin.h - windows terminal emulator
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

#pragma once

#include <Windows.h>

#include "wvt.h"
#include "wvtWin.h"

class wvtMainWin : public wvtWin {
  HWND childPane;

  BOOL registerWindowClass();
  BOOL createMainWindow(const int nCmdShow);
  void constructChildPane(const HWND hwnd);
  void destroyChildPane();

  HWND retrieveChildPane() const {
    return childPane;
  }

public:

  wvtMainWin(HINSTANCE inst);
  BOOL init();
  void loop();

  static const TCHAR wvtMainWin::windowClassName[];
  static const TCHAR wvtMainWin::windowTitleName[];
  static const int wvtMainWin::windowInitialWidth;
  static const int wvtMainWin::windowInitialHeight;

  static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
