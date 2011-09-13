/**
 * Inptools Tools to work with EPANET INP files
 * 
 * InpToolsExt.cpp: Implement DLL exports
 * 
 * (c) 2008, 2009 Steffen Macke <sdteffen@sdteffen.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

/**
 * Hint: proxy/stub information
 * To create proxy/stub DLL, run 
 * nmake -f InpToolsExtps.mk
 */

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "InpToolsExt.h"

#include "InpToolsExt_i.c"
#include "InpToolsShlExt.h"
CComModule _Module;
BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_InpToolsShlExt, CInpToolsShlExt) END_OBJECT_MAP()
/**
 * DLL entry point
 */
extern
    "C"
    BOOL
    WINAPI
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved */ )
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		_Module.Init(ObjectMap, hInstance, &LIBID_INPTOOLSEXTLib);
		DisableThreadLibraryCalls(hInstance);
	}

	else if (dwReason == DLL_PROCESS_DETACH)
		_Module.Term();
	return TRUE;		// OK
}


/**
 * Determine if DLL can be removed from memory by OLE.
 */
STDAPI DllCanUnloadNow(void)
{
	return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}


/**
 * Return class factory for the given type.
 */
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}


/**
 * Add registry entries. Add object, typelib and interfaces in typelib.
 */
STDAPI DllRegisterServer(void)
{
	return _Module.RegisterServer(FALSE);
}


/**
 * Remove registry entries.
 */
STDAPI DllUnregisterServer(void)
{
	return _Module.UnregisterServer(FALSE);
}
