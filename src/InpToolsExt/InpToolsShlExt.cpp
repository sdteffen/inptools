/**
 * Inptools Tools to work with EPANET INP files
 * 
 * InpToolsShlExt.cpp: Implement the Shell extension
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
#include "stdafx.h"
#include "InpToolsExt.h"
#include "InpToolsShlExt.h"
#include <process.h>
#include "libintl.h"

/**
 * @class CInpToolsShlExt
 */
STDMETHODIMP
    CInpToolsShlExt::Initialize(LPCITEMIDLIST pidlFolder,
				LPDATAOBJECT pDataObj, HKEY hProgID)
{

	FORMATETC fmt = {
		CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL
	};

	STGMEDIUM stg = {
		TYMED_HGLOBAL
	};

	HDROP hDrop;


	if (FAILED(pDataObj->GetData(&fmt, &stg))) {

		return E_INVALIDARG;

	}


	hDrop = (HDROP) GlobalLock(stg.hGlobal);


	if (NULL == hDrop)

		return E_INVALIDARG;


	UINT uNumFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

	HRESULT hr = S_OK;


	if (0 == uNumFiles) {

		GlobalUnlock(stg.hGlobal);

		ReleaseStgMedium(&stg);

		return E_INVALIDARG;

	}


	if (0 == DragQueryFile(hDrop, 0, m_szFile, MAX_PATH))

		hr = E_INVALIDARG;


	GlobalUnlock(stg.hGlobal);

	ReleaseStgMedium(&stg);


	return hr;

}



STDMETHODIMP
    CInpToolsShlExt::QueryContextMenu(HMENU hmenu, UINT uMenuIndex,
				      UINT uidFirstCmd, UINT uidLastCmd,
				      UINT uFlags)
{

	HMENU hPopup;

	UINT uidCmd = uidFirstCmd;

	HKEY hkey;

	WORD lwPathChars = MAX_PATH + 32;

	char szPath[MAX_PATH + 32];

	char *szPathStart;

	char *szPathEnd;

	int iEpanetMenuFlags = MF_BYPOSITION | MF_GRAYED;


	if (uFlags & CMF_DEFAULTONLY)

		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);


	/**
	 * Gettext
	 */
	HMODULE hmodule;

	TCHAR szDllPath[MAX_PATH];


	/**
	 * @todo Proper error handling.
	 */
	hmodule = GetModuleHandle("InpToolsExt.dll");

	if (!hmodule) {

		MessageBox(NULL, "Could not obtain module handle",
			   "Inptools Shell Extension", MB_ICONERROR);

		return E_INVALIDARG;

	}


	if (!GetModuleFileName(hmodule, szDllPath, MAX_PATH)) {

		MessageBox(NULL, "Could not obtain module path",
			   "Inptools Shell Extension", MB_ICONERROR);

		return E_INVALIDARG;

	}

	szDllPath[strlen(szDllPath) - strlen("InpToolsExt.dll")] = 0;


	char exe_path[MAX_PATH] = "";

	char localedir[MAX_PATH] = "";


	strncpy(exe_path, szDllPath, MAX_PATH);

	char *exe_folder = strrchr(exe_path, '\\');

	if (exe_folder) {

		++exe_folder;

		if (exe_folder) {

			*exe_folder = 0;

			snprintf(localedir, MAX_PATH, "%s\\..\\locale",
				 exe_path);

			bindtextdomain(PACKAGE, localedir);

		}

	}

	textdomain(PACKAGE);



  /**
   * Try to find EPANET installation.
   */
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		     "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\EPANET 2.0",
		     0, KEY_QUERY_VALUE, &hkey);

	if (ERROR_SUCCESS ==
	    RegQueryValueEx(hkey, "UninstallString", NULL, NULL,
			    (LPBYTE) szPath, (LPDWORD) & lwPathChars)) {

		szPathStart = strstr(szPath, "\"/UNINST=");

		if (NULL != szPathStart) {

			szPathStart += 9;

			szPathEnd = strstr(szPathStart, "\"");

			if (NULL != szPathEnd) {

				ZeroMemory(m_szEpanet2w, MAX_PATH);

				strncpy(m_szEpanet2w, szPathStart,
					szPathEnd - szPathStart - 10);

				szPathEnd =
				    m_szEpanet2w + strlen(m_szEpanet2w);

				strncpy(szPathEnd, "Epanet2w.exe", 12);

				/**
				 * @todo Check if the executable exists
				 */
				iEpanetMenuFlags = MF_BYPOSITION;

			}

		}

	}

	hPopup = CreatePopupMenu();

	InsertMenu(hPopup, 0, MF_BYPOSITION, uidCmd++, _("About"));
	InsertMenu(hPopup, 0, MF_BYPOSITION, uidCmd++, _("Help"));
	InsertMenu(hPopup, 0, MF_BYPOSITION, uidCmd++,
		   _("Create binary result file"));
	InsertMenu(hPopup, 0, MF_BYPOSITION, uidCmd++,
		   _("Create CSV result files"));
	InsertMenu(hPopup, 0, MF_BYPOSITION, uidCmd++,
		   _("Project from GK3 to WGS84"));
	InsertMenu(hPopup, 0, MF_BYPOSITION, uidCmd++,
		   _("Create report in German"));
	InsertMenu(hPopup, 0, iEpanetMenuFlags, uidCmd++,
		   _("Open with EPANET"));
	MENUITEMINFO mii = {
		sizeof(MENUITEMINFO)
	};


	mii.fMask = MIIM_SUBMENU | 0x00000040 | MIIM_ID;

	mii.wID = uidCmd++;

	mii.hSubMenu = hPopup;

	mii.dwTypeData = _("InpTools");


	InsertMenuItem(hmenu, uMenuIndex, TRUE, &mii);


	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL,
			    uidCmd - uidFirstCmd);

}


STDMETHODIMP
    CInpToolsShlExt::GetCommandString(UINT idCmd, UINT uFlags,
				      UINT * pwReserved,
				      LPSTR pszName, UINT cchMax)
{

	USES_CONVERSION;


	if ((MENU_EPANET != idCmd) && (MENU_INPPROJ != idCmd))

		return E_INVALIDARG;


	if (uFlags & GCS_HELPTEXT) {

		LPCTSTR szText =
		    _("This is the inptools shell extension's help.");


		if (uFlags & GCS_UNICODE) {

			lstrcpynW((LPWSTR) pszName, T2CW(szText), cchMax);

		}

		else {

			lstrcpynA(pszName, T2CA(szText), cchMax);

		}


		return S_OK;

	}


	return E_INVALIDARG;

}


STDMETHODIMP CInpToolsShlExt::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{

	OPENFILENAME ofn;
	OPENFILENAME ofn2;

	char szFileName[MAX_PATH] = "";

	char szOutFileName[MAX_PATH] = "";

	HMODULE hmodule;

	/**
	 * EPANET binary result file.
	 */
	char szBinaryFileName[MAX_PATH] = "";

	if (0 != HIWORD(pCmdInfo->lpVerb))

		return E_INVALIDARG;

	m_pCmdInfo = pCmdInfo;

	/**
	 * @todo Proper error handling.
	 */
	hmodule = GetModuleHandle("InpToolsExt.dll");

	if (!hmodule) {
		MessageBox(pCmdInfo->hwnd,
			   _("Could not obtain module handle"),
			   _("Inptools Shell Extension"),
			   MB_ICONERROR);
		return E_INVALIDARG;
	}

	if (!GetModuleFileName
		(hmodule, m_szDllPath, MAX_PATH)) {
		MessageBox(pCmdInfo->hwnd,
			   _("Could not obtain module path"),
			   _("Inptools Shell Extension"),
			   MB_ICONERROR);
		return E_INVALIDARG;
	}

	m_szDllPath[strlen(m_szDllPath) -
		  strlen("InpToolsExt.dll")] = 0;

	wsprintf(m_szEpanet2, "\"%sepanet2.exe\"",
				 m_szDllPath);

	switch (LOWORD(pCmdInfo->lpVerb)) {
	case MENU_INPTOOLS_ABOUT:
		{
			MessageBox(pCmdInfo->hwnd,
				   _("Inptools 0.2.1\nCopyright (c) 2008, 2009 Steffen Macke\nPortions Copyright (c) 1999, 2000 Frank Wamerdam\n\nhttp://inptools.epanet.de"),
				   _("Inptools Shell Extension"),
				   MB_ICONINFORMATION);
			return S_OK;
		}
		break;

	case MENU_HELP:
		{
			if(E_FAIL == OpenHelp("de"))
				return OpenHelp("en");
			return S_OK;
		}
		break;

	case MENU_INPPROJ:
		{

			TCHAR szCommand[MAX_PATH];
			TCHAR szParams[3 * MAX_PATH + 10];
			WORD lwPathChars = MAX_PATH;

			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = pCmdInfo->hwnd;
			ofn.lpstrFilter =
			    _
			    ("EPANET INP files (*.inp)\0*.inp\0All files (*.*)\0*.*\0");

			ofn.lpstrFile = szOutFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags =
			    OFN_EXPLORER | OFN_OVERWRITEPROMPT |
			    OFN_HIDEREADONLY;
			ofn.lpstrDefExt = "inp";

			if(!GetSaveFileName(&ofn))
				return E_INVALIDARG;

			wsprintf(szCommand, "\"%sinpproj.exe\"",
				 m_szDllPath);

			wsprintf(szParams, "\"%s\" \"%s\"", m_szFile,
				 szOutFileName);

			RunProcess(szCommand, szParams, TRUE);

			return S_OK;

		}

		break;


	case MENU_REPORT_DE:
		{

			TCHAR szCommand[MAX_PATH];
			TCHAR szParams[2 * MAX_PATH + 8];
			WORD lwPathChars = MAX_PATH;
			
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = pCmdInfo->hwnd;

			ofn.lpstrFilter =
			    _
			    ("Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0");

			ofn.lpstrFile = szOutFileName;

			ofn.nMaxFile = MAX_PATH;

			ofn.Flags =
			    OFN_EXPLORER | OFN_OVERWRITEPROMPT |
			    OFN_HIDEREADONLY;

			ofn.lpstrDefExt = "txt";

			if(!GetSaveFileName(&ofn))
				return E_INVALIDARG;

			wsprintf(szCommand, "\"%sepanet2.exe\"",
				 m_szDllPath);

			wsprintf(szParams, "\"%s\" \"%s\"", m_szFile,
				 szOutFileName);

			char szLang[MAX_PATH] = "";

			DWORD nEnvironmentRead = 0;

			nEnvironmentRead =
			    GetEnvironmentVariable("LANG", szLang,
						   MAX_PATH);

			if ((nEnvironmentRead > 0)
			    && (nEnvironmentRead < MAX_PATH))

				SetEnvironmentVariable("LANG", "de");

			RunProcess(szCommand, szParams, TRUE);

			if ((nEnvironmentRead > 0)
			    && (nEnvironmentRead < MAX_PATH))

				SetEnvironmentVariable("LANG", szLang);

			return S_OK;

		}

		break;
	case MENU_BINARY_RESULT:
		{
			TCHAR szCommand[MAX_PATH];
			TCHAR szParams[2 * MAX_PATH + 8];
			WORD lwPathChars = MAX_PATH;

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = pCmdInfo->hwnd;
			ofn.lpstrFilter =
			    _
			    ("EPANET binary result files (*.epabin)\0*.epabin\0All files (*.*)\0*.*\0");
			ofn.lpstrFile = szBinaryFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags =
			    OFN_EXPLORER | OFN_OVERWRITEPROMPT |
			    OFN_HIDEREADONLY;
			ofn.lpstrDefExt = "epabin";
			if(!GetSaveFileName(&ofn))
				return E_INVALIDARG;
			ofn.lpstrFilter =
			    _
			    ("Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0");
			ofn.lpstrFile = szOutFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags =
			    OFN_EXPLORER | OFN_OVERWRITEPROMPT |
			    OFN_HIDEREADONLY;
			ofn.lpstrDefExt = "txt";
			if(!GetSaveFileName(&ofn))
				return E_INVALIDARG;
			wsprintf(szCommand, "\"%sepanet2.exe\"",
				 m_szDllPath);
			wsprintf(szParams, "\"%s\" \"%s\" \"%s\"",
				 m_szFile, szOutFileName,
				 szBinaryFileName);
			char szLang[MAX_PATH] = "";
			DWORD nEnvironmentRead = 0;
			nEnvironmentRead =
			    GetEnvironmentVariable("LANG", szLang,
						   MAX_PATH);
			if ((nEnvironmentRead > 0)
			    && (nEnvironmentRead < MAX_PATH))
				SetEnvironmentVariable("LANG", "de");
			RunProcess(szCommand, szParams, TRUE);
			if ((nEnvironmentRead > 0)
			    && (nEnvironmentRead < MAX_PATH))
				SetEnvironmentVariable("LANG", szLang);
			return S_OK;
		}
		break;

	case MENU_CSV_RESULT:
		{
			TCHAR szNodeFile[MAX_PATH];
			TCHAR szLinkFile[MAX_PATH];
			WORD lwPathChars = MAX_PATH;
			
			/**
			 * Get node file location.
			 */
			szNodeFile[0] = '\0'; 
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = pCmdInfo->hwnd;
			ofn.lpstrFilter =
			    _("Node CSV files (*.csv)\0*.csv\0All files (*.*)\0*.*\0");
			ofn.lpstrFile = szNodeFile;
			ofn.nMaxFile = sizeof(szNodeFile)/sizeof(*szNodeFile);
			ofn.Flags =
			    OFN_EXPLORER | OFN_OVERWRITEPROMPT |
			    OFN_HIDEREADONLY;
			ofn.lpstrDefExt = "csv";
			ofn.lpstrTitle = _("Save Node CSV file");
			if(!GetSaveFileName(&ofn))
				return E_INVALIDARG;

			/**
			 * Get link file location.
			 */
			szLinkFile[0] = '\0';
			ZeroMemory(&ofn, sizeof(ofn2));
			ofn2.lStructSize = sizeof(OPENFILENAME);
			ofn2.hwndOwner = pCmdInfo->hwnd;
			ofn2.lpstrFilter =
			    _("Link CSV files (*.csv)\0*.csv\0All files (*.*)\0*.*\0");
			ofn2.nMaxFile =  sizeof(szLinkFile)/sizeof(*szLinkFile);
			ofn2.Flags =
			    OFN_EXPLORER | OFN_OVERWRITEPROMPT |
			    OFN_HIDEREADONLY;
			ofn2.lpstrFile = szLinkFile;
			ofn2.lpstrDefExt = "csv";
			ofn2.lpstrTitle = _("Save Link CSV file");
			if(!GetSaveFileName(&ofn2))
				return E_INVALIDARG;

			CreateCsvFile(szNodeFile, szLinkFile);
			
			return S_OK;
		}
		break;
	case MENU_EPANET:
		{

			TCHAR szParams[2 * MAX_PATH + 8];

			WORD lwPathChars = MAX_PATH;

			/**
			 * @todo Proper error handling.
			 */
			wsprintf(szParams, "\"%s\" \"%s\"",
				 m_szEpanet2w, m_szFile);

			WinExec(szParams, SW_SHOW);

		}

		return S_OK;

		break;

	default:

		return E_INVALIDARG;

		break;

	}

}


DWORD
    CInpToolsShlExt::RunProcess(char *szCommand, char *szArgs, bool hidden)
{

	STARTUPINFO si;

	PROCESS_INFORMATION pi;

	char szCommandLine[4096];

	char *szEnvCOMSPEC = NULL;

	char *szDefaultCMD = "CMD.EXE";

	ULONG uReturnCode;


	ZeroMemory(&si, sizeof(STARTUPINFO));

	si.cb = sizeof(STARTUPINFO);

	si.wShowWindow = SW_HIDE;

	si.dwFlags = STARTF_USESHOWWINDOW;


	szCommandLine[0] = 0;


	strcat(szCommandLine, szCommand);

	strcat(szCommandLine, " ");

	strcat(szCommandLine, szArgs);

	if (!CreateProcess
	    (NULL, szCommandLine, NULL, NULL, FALSE,
	     CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {

		return GetLastError();

	}


	WaitForSingleObject(pi.hProcess, INFINITE);

	if (!GetExitCodeProcess(pi.hProcess, &uReturnCode))

		uReturnCode = 0;


	CloseHandle(pi.hThread);

	CloseHandle(pi.hProcess);


	return uReturnCode;

}

DWORD
 CInpToolsShlExt::RunEpanet2csv(char *szBinaryFile, char *szNodesFile, char *szLinkFile)
{
	TCHAR szEpanet2csv[MAX_PATH];
	TCHAR szParams[3 * MAX_PATH + 10];

	wsprintf(szEpanet2csv, "\"%sepanet2csv.exe\"",
			m_szDllPath);
	wsprintf(szParams, "\"%s\" \"%s\" \"%s\"",
			szBinaryFile, szNodesFile, szLinkFile);
	return RunProcess(szEpanet2csv, szParams, TRUE);
}

DWORD
 CInpToolsShlExt::CreateCsvFile(char *szNodeFile, char *szLinkFile)
{
	DWORD dwRetVal;
	DWORD dwBufSize=MAX_PATH;
	TCHAR lpPathBuffer[MAX_PATH];
	TCHAR szReportFileName[MAX_PATH];
	TCHAR szBinaryFileName[MAX_PATH];
	TCHAR szParams[3 * MAX_PATH + 10];
	UINT uRetVal;

    /**
     * Find temp folder
	 */
    dwRetVal = GetTempPath(dwBufSize,    
                           lpPathBuffer); 
    if (dwRetVal > dwBufSize || (dwRetVal == 0))
    {
       MessageBox(m_pCmdInfo->hwnd,
		   _("Temp folder could not be obtained."),
		   _("Inptools Shell Extension"),
		   MB_ICONERROR);
        return E_FAIL;
    }

    /**
	 * Get temporary report file.
	 */
    uRetVal = GetTempFileName(lpPathBuffer,
                              TEXT("NEW"),  
                              0,            
                              szReportFileName);  
    if (uRetVal == 0)
    {
		MessageBox(m_pCmdInfo->hwnd,
		   _("Temporary file name could not be obtained."),
		   _("Inptools Shell Extension"),
		   MB_ICONERROR);
        return E_FAIL;
    }

	/**
	 * Get temporary binary result file.
	 */
    uRetVal = GetTempFileName(lpPathBuffer,
                              TEXT("NEW"),  
                              0,            
                              szBinaryFileName);  
    if (uRetVal == 0)
    {
		MessageBox(m_pCmdInfo->hwnd,
		   _("Temporary file name could not be obtained."),
		   _("Inptools Shell Extension"),
		   MB_ICONERROR);
        return E_FAIL;
    }

	wsprintf(szParams, "\"%s\" \"%s\" \"%s\"",
				 m_szFile, szReportFileName,
				 szBinaryFileName);
	RunProcess(m_szEpanet2, szParams, TRUE);
	return RunEpanet2csv(szBinaryFileName, szNodeFile, szLinkFile);
}

DWORD
 CInpToolsShlExt::OpenHelp(TCHAR *language)
{
	TCHAR szHelpPath[MAX_PATH+32];
	TCHAR szHelpFile[MAX_PATH+32];

	strncpy(szHelpPath, m_szDllPath, MAX_PATH);
	lstrcat(szHelpPath, "..\\doc\\");
	lstrcat(szHelpPath, language);
	strncpy(szHelpFile, szHelpPath, MAX_PATH);
	lstrcat(szHelpFile, "\\inptools.chm");
	DWORD dwAttr = GetFileAttributes(szHelpFile);
	if((dwAttr != 0xffffffff)&&(! (dwAttr & FILE_ATTRIBUTE_DIRECTORY)))
	{
		ShellExecuteA (0, "open", "inptools.chm", NULL, szHelpPath, SW_SHOWNORMAL);
	}
	else
		return E_FAIL;
	return S_OK;
}