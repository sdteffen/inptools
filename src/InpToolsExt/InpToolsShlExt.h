/**
 * Inptools Tools to work with EPANET INP files
 * 
 * InpToolsShlExt.h: Declare the Shell extension
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

#ifndef __INPTOOLSSHLEXT_H_
#define __INPTOOLSSHLEXT_H_

#include <shlobj.h>
#include <comdef.h>
#include <libintl.h>

#include "resource.h"

#define MENU_INPTOOLS_ABOUT 0
#define MENU_HELP 1
#define MENU_BINARY_RESULT 2
#define MENU_CSV_RESULT 3
#define MENU_INPPROJ 4
#define MENU_REPORT_DE 5
#define MENU_EPANET 6
#define _ gettext
#define PACKAGE "inptools"

/**
 * CInpToolsShlExt
 */
class ATL_NO_VTABLE CInpToolsShlExt:public CComObjectRootEx <
    CComSingleThreadModel >, public CComCoClass < CInpToolsShlExt,
    &CLSID_InpToolsShlExt >, public IShellExtInit, public IContextMenu {

      public:
	CInpToolsShlExt() {

	} DECLARE_REGISTRY_RESOURCEID(IDR_INPTOOLSSHLEXT)
	 DECLARE_NOT_AGGREGATABLE(CInpToolsShlExt)

	 DECLARE_PROTECT_FINAL_CONSTRUCT()

	 BEGIN_COM_MAP(CInpToolsShlExt)
	 COM_INTERFACE_ENTRY(IShellExtInit)
	 COM_INTERFACE_ENTRY(IContextMenu)
	 END_COM_MAP()

      public:
	// IShellExtInit
	 STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);


	// IContextMenu
	STDMETHODIMP GetCommandString(UINT, UINT, UINT *, LPSTR, UINT);

	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);

	STDMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);

      protected:
	TCHAR m_szFile[MAX_PATH];

	TCHAR m_szEpanet2w[MAX_PATH];

	TCHAR m_szEpanet2[MAX_PATH];

	TCHAR m_szDllPath[MAX_PATH];

	LPCMINVOKECOMMANDINFO m_pCmdInfo;

	DWORD CreateCsvFile(char *, char *);

	DWORD OpenHelp(TCHAR *language);

	DWORD RunEpanet2csv(char *, char *, char *);
	
	DWORD RunProcess(char *, char *, bool);


};



#endif				//__INPTOOLSSHLEXT_H_
