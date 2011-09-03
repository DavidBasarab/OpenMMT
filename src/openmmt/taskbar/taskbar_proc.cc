/*************************************************************************
 * OpenMMT - Open Multi-Monitor Taskbar
 * Copyright (C) 2010-2011 Genscripts
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
#include "openmmt/precompiled_headers.h"
#include "openmmt/global_variables.h"
#include "openmmt/resource.h"
#include "openmmt/messages.h"
#include "openmmt/monitors/monitor.h"
#include "openmmt/taskbar/taskbar.h"
#include "openmmt/taskbar/taskbar_event.h"
#include "openmmt/ui/settings_dlg.h"
#include "openmmt/windows/shell.h"
#include "openmmt/windows/windows.h"

LRESULT CALLBACK TaskbarProc(HWND hWnd, UINT msg, WPARAM wParam, 
                             LPARAM lParam)
{
  switch(msg)
  {
  case WM_ACTIVATE:
      TaskbarEvent::OnActivate(hWnd);
    return TRUE;

  case WM_ERASEBKGND:
      TaskbarEvent::OnEraseBackground(hWnd);
    return TRUE;

  case WM_THEMECHANGED:
  case WM_DWMCOMPOSITIONCHANGED:
      TaskbarEvent::OnThemeChange(hWnd);
    return TRUE;

  case WM_CONTEXTMENU:
    {
      POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      ScreenToClient(hWnd, &pt);
      TaskbarEvent::OnContextMenu(hWnd, NULL, pt);
    }
    return TRUE;
  case WM_DISPLAYCHANGE:
      g_pMonitorManager->EnumerateMonitors();
    return 0;

  case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
      case ID_BAR_CLOSE:
        PostQuitMessage(0);
        break;
      case ID_BAR_CASCADE:
        g_pShell->CascadeWindows();
        break;
      case ID_BAR_STACKED:
        g_pShell->TileHorizontally();
        break;
      case ID_BAR_SIDEBSIDE:
        g_pShell->TileVertically();
        break;
      case ID_BAR_SHOWDESKTOP:
        g_pShell->ToggleDesktop();
        break;
      case ID_BAR_TASKMGR:
          ShellExecute(NULL, L"open", L"taskmgr.exe", NULL, NULL, SW_NORMAL);
        break;
      case ID_BAR_PROPS:
        {
          DlgSettings settingsDlg;
          settingsDlg.DoModal();
        }
        break;
      default:
        break;
      }
    }
    break;

  case WM_SYSCOMMAND:
    {
      /** Prevent the user from moving the taskbar for now. */
      if ((wParam & 0xFFF0)==SC_MOVE)
        return TRUE;
    }
    break;

  case WM_CLOSE:
    DestroyWindow(hWnd);
    break;

  default:
      TaskbarEvent::CheckAppBarMsg(hWnd, msg, wParam, lParam);

    return DefWindowProc(hWnd, msg, wParam, lParam);
  }

  return 0;
}

// EOF
