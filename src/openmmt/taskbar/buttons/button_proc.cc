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
#include "openmmt/precompiled_headers.h"
#include "openmmt/global_variables.h"
#include "openmmt/resource.h"
#include "openmmt/taskbar/taskbar_event.h"
#include "openmmt/taskbar/buttons/button_event.h"

LRESULT CALLBACK ButtonProc(HWND hWnd, UINT msg, WPARAM wParam, 
                            LPARAM lParam)
{
  switch(msg)
  {
    // All rendering will be handled by WM_PAINT.
  case WM_ERASEBKGND:
    return TRUE;

  case WM_CLOSE:
    DestroyWindow(hWnd);
    break;

  case WM_DESTROY:
      ButtonEvent::OnWindowDestroyed(hWnd);
    break;

  case WM_PAINT:
    {
      ButtonEvent::OnPaint(hWnd);
      ValidateRect(hWnd, NULL);
    }
    break;

  case WM_MOUSEMOVE:
      ButtonEvent::OnMouseMove(hWnd, (HDC)wParam, NULL);
    break;

  case WM_LBUTTONDOWN:
      ButtonEvent::OnMouseBeginLeftClick(hWnd, (HDC)wParam, NULL);
    break;

  case WM_LBUTTONUP:
      ButtonEvent::OnMouseEndLeftClick(hWnd, (HDC)wParam, NULL);
    break;

  case WM_CONTEXTMENU:
    {
      POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      ScreenToClient(hWnd, &pt);
      ButtonEvent::OnContextMenu(hWnd, NULL, pt);
    }
    break;

  case WM_COMMAND:
    {
      TaskbarPtr pBar(g_pMonitorManager->FindMonitorTaskbar(hWnd));

      if (pBar == TaskbarPtr())
        break;

      ButtonPtr pBtn(pBar->GetButton(hWnd));

      if (pBtn == ButtonPtr())
        break;

      switch (LOWORD(wParam))
      {
      case ID_BTN_CONTEXT_CLOSE:
        {
          SetForegroundWindow(pBtn->GetAppHandle());
          PostMessage(pBtn->GetAppHandle(), WM_CLOSE, NULL, NULL);

          // TODO: Need to find a better way of doing this
          //g_pAppManager->CheckWindow(pBtn->GetAppHandle());
        }
        break;

      default:
        break;
      }
    }
    break;


  case WM_MOUSEHOVER:
    {
      // If the mouse is hovering and the user has the right click
      // option set, then we won't call this event in case the user
      // starts to drag the button.
      if (!(wParam & MK_LBUTTON))
        ButtonEvent::OnMouseHoover(hWnd, (HDC)wParam, NULL);
    }
    break;

  case WM_MOUSELEAVE:
      ButtonEvent::OnMouseLeave(hWnd, (HDC)wParam, NULL);;
    break;

  case WM_THEMECHANGED:
  case WM_DWMCOMPOSITIONCHANGED:
      ButtonEvent::OnThemeChange(hWnd);
    break;

  default:
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return 0;
}

// EOF
