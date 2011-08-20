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
#include "openmmt/openmmt.h"
#include <boost/thread.hpp>

CAppModule _Module;


int Run (HINSTANCE hInstance)
{
  CMessageLoop msgLoop;

  _Module.AddMessageLoop(&msgLoop);

  boost::thread thread_openmmt(OpenMMT_Run, hInstance);
  int nRet = msgLoop.Run();

  thread_openmmt.join();
  _Module.RemoveMessageLoop();

  return 0;
}

int wWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPWSTR lpCmdLine, __in int nShowCmd )
{
  InitCommonControls();
  _Module.Init(NULL, hInstance);

  int nRet = Run(hInstance);

  _Module.Term();

  return nRet;
}

// EOF

