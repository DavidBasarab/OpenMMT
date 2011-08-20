/*************************************************************************
 * OpenMMT - Open Multi-Monitor Taskbar
 * Copyright (C) 2010 Genscripts
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "libsys/sys.h"
#include "libsys/api.h"
#include "openmmt/messages.h"

// Global declarations
#pragma data_seg(".SHARDATA")
  BOOL  g_bHooksInstalled = FALSE;
  HHOOK g_hHookProc   = {0};
  HWND  g_hWndOpenMMT = {0};
  HWND  g_hWndHelper  = {0};
#pragma data_seg()

#pragma comment(linker, "/SECTION:.SHARDATA,RWS")

// Taken from openmmt/windows.cc
// TODO: Create a generic "base" library and import this from it.
static BOOL WndShouldDisplay(HWND hWnd)
{
  if (!hWnd)
    return FALSE;

  LONG_PTR dwExFlags = GetWindowLongPtr(hWnd, GWL_EXSTYLE);

  if ((dwExFlags & WS_EX_TOOLWINDOW) || (dwExFlags & WS_EX_NOACTIVATE) || 
    (dwExFlags & WS_EX_MDICHILD))
    return FALSE;

  LONG_PTR dwFlags = GetWindowLongPtr(hWnd, GWL_STYLE);

  if (dwFlags & WS_DISABLED)
    return FALSE;

  HWND hwParentWindow = (HWND) GetWindowLongPtr(hWnd, GWLP_HWNDPARENT);

  if (hwParentWindow) {
    if ((IsChild(hwParentWindow, hWnd)) && (!(dwExFlags & WS_EX_APPWINDOW)))
      return FALSE;

    if (!(dwExFlags & WS_EX_CONTROLPARENT))
      return FALSE;
  }

  return TRUE;
}

static void SendTaskbarMsg(UINT mMsg, HWND hWnd, LPARAM lParam = NULL)
{
  if (!WndShouldDisplay(hWnd))
    return;

  // Check to see if OpenMMT is still running, if not terminate the hooks.
  if (!IsWindow(g_hWndOpenMMT)) {
    SysHooksStop();

    // Check to see if the helper is still running, if so, tell it to die.
    if (g_hWndHelper && IsWindow(g_hWndHelper))
      PostMessage(g_hWndHelper, WM_QUIT, NULL, NULL);

    return;
  }

  // Note that we use PostMessage instead of SendMessage.
  PostMessage(g_hWndOpenMMT, mMsg, (WPARAM)hWnd, lParam);
}

LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode >= 0) {
    PCWPRETSTRUCT lpCwp = (PCWPRETSTRUCT)lParam;

    switch (lpCwp->message)
    {
    case WM_CREATE:
        SendTaskbarMsg(TASKBAR_WINDOW_ACTIVATE, lpCwp->hwnd);
        break;
         
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
        SendTaskbarMsg(TASKBAR_WINDOW_ACTIVATE, lpCwp->hwnd);
      break;

    case WM_WINDOWPOSCHANGED:
        SendTaskbarMsg(TASKBAR_WINDOW_POSCHANGED, lpCwp->hwnd, lpCwp->lParam);
      break;

    case WM_DESTROY:
    case WM_CLOSE:
        SendTaskbarMsg(TASKBAR_WINDOW_DESTROY, lpCwp->hwnd);
      break;

    case WM_KILLFOCUS:
        SendTaskbarMsg(TASKBAR_WINDOW_SETFOCUS, lpCwp->hwnd);
      break;
    }
  }
  return CallNextHookEx(0, nCode, wParam, lParam);
}

// EOF