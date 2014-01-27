/**
 * inp2shpw.cpp Create shape files from EPANET INP file
 * 
 * (c) 2014 Steffen Macke <sdteffen@sdteffen.de>
 * 
 * http://epanet.de/inptools
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

#include <string>
#include <windows.h>
#include <process.h>
#include <shlobj.h>
#include <objbase.h>
#include <string.h>

#define _(String) (String)

using namespace std;

int file_exists(TCHAR * file);
DWORD run_process (char *szCommand, char *szArgs);
string & str_replace_null (const string & search, string & subject);

HANDLE g_hStdOutRd = NULL;
HANDLE g_hStdOutWr = NULL;
string title = _("inp2shpw");

/**
 * USAGE: inp2shpw inp2shp inputfile
 */
int
main (int argc, char *argv[])
{
  string params;
  OPENFILENAME ofn;
  DWORD return_value = 0;
  DWORD dwBufSize = MAX_PATH;
  TCHAR szReportFileName[MAX_PATH];
  TCHAR lpPathBuffer[MAX_PATH];
  int param_count, partCount;
  string pattern;
  UINT uRetVal;

  LPITEMIDLIST pidl     = NULL;
  BROWSEINFO   bi       = { 0 };
  BOOL         bResult  = FALSE;
  TCHAR szDir[MAX_PATH];
  string shapefilepath, question;

  /**
   * The shapefiles that will be generated.
   */
  string shapefiles[] = { "junctions", "pipes", "pumps", "reservoirs", "tanks",
  	"valves"
  };
  const int SHAPEFILECOUNT = 6;

  /**
   * The different files that belong to a shapefile
   */
  string shapeparts[] = {"shp", "dbf", "shx"};
  const int SHAPEPARTCOUNT = 3;

  if (3 != argc)
    {
      MessageBox (NULL,
		  _("USAGE: inp2shpw inp2shp inputfile"),
		  title.c_str(), MB_ICONERROR);
      return 1;
    }

  for (param_count = 1; param_count < 3; param_count++)
  {
	  if (!file_exists(argv[param_count]))
	  {
		  question = "File not found: \"";
		  question.append(argv[param_count]);
		  question.append("\"");
		  MessageBox (NULL,
				  question.c_str(), title.c_str(), MB_ICONERROR);
		  return 1;
	  }
  }

 /**
  * Find temp folder
  */
  return_value = GetTempPath (dwBufSize, lpPathBuffer);
  if (return_value > dwBufSize || (return_value == 0))
    {
      MessageBox (NULL,
		  _("Temp folder could not be obtained."),
		  title.c_str(), MB_ICONERROR);
      return 1;
    }

    /**
     * Get temporary report file.
     */
  uRetVal = GetTempFileName (lpPathBuffer, TEXT ("NEW"), 0, szReportFileName);
  if (uRetVal == 0)
    {
      MessageBox (NULL,
		  _("Temporary file name could not be obtained."),
		  title.c_str(), MB_ICONERROR);
      return 1;
    }

    /**
     * Get the folder.
     */
    if(S_OK != CoInitialize(NULL))
    {
    	MessageBox (NULL, _("CoInitialize() failed."),
    			title.c_str(), MB_ICONERROR);
    	return 1;
    }
  	bi.hwndOwner      = NULL;
  	bi.pszDisplayName = szDir;
  	bi.pidlRoot       = NULL;
  	bi.lpszTitle      = _("Please select a folder to store the shape files.");
  	bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
  	bi.ulFlags = 0;
  	bi.lpfn = NULL;
  	bi.lParam = 0;
  	bi.iImage = -1;
  	/**
  	 * TODO: Replace with IFileDialog
  	 */
  	if ((pidl = SHBrowseForFolder(&bi)) != NULL)
  	{
  		bResult = SHGetPathFromIDList(pidl, szDir);
  		CoTaskMemFree(pidl);
  	}
  	CoUninitialize();
  	if (FALSE == bResult)
  		return 0;

  /**
   * Build the inp2shp command line.
   */
  params = "\"";
  params.append (argv[2]);
  params.append ("\" \"");
  params.append (szReportFileName);
  params.append ("\"");
  /**
   * Loop over all shapefiles
   */
  for (param_count = 0; param_count < SHAPEFILECOUNT; param_count++)
  {
	  for (partCount = 0; partCount < SHAPEPARTCOUNT; partCount++)
	  {
		  shapefilepath = "";
		  shapefilepath.append(szDir);
		  shapefilepath.append("\\");
		  shapefilepath.append(shapefiles[param_count]);
		  shapefilepath.append(".");
		  shapefilepath.append(shapeparts[partCount]);
		  if (file_exists((TCHAR *)(shapefilepath.c_str()))) {
			  question = "Do you want to overwrite \"";
			  question.append(shapefilepath);
			  question.append("\"?");
			  if (IDYES == MessageBox(NULL, question.c_str(), title.c_str(), MB_ICONQUESTION | MB_YESNO))
				  break;
			  shapefilepath.erase(shapefilepath.end()-3,shapefilepath.end());
			  shapefilepath.append("shp");
			  strncpy(lpPathBuffer, shapefilepath.c_str(), shapefilepath.length());
			  ZeroMemory (&ofn, sizeof (ofn));
			   ofn.lStructSize = sizeof (ofn);
			   ofn.hwndOwner = NULL;
			   ofn.lpstrFilter = "Shapefiles (*.shp)\0*.shp\0\0";
			   ofn.lpstrFile = lpPathBuffer;
			   ofn.nMaxFile = MAX_PATH;
			   ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
			   ofn.lpstrDefExt = "shp";

			   if (!GetSaveFileName (&ofn))
				   return 1;
			   shapefilepath = lpPathBuffer;
		  }
	  }
	  shapefilepath.erase(shapefilepath.end()-3,shapefilepath.end());
	  shapefilepath.append("shp");

	  params.append(" \"");
	  params.append(shapefilepath);
	  params.append("\"");
  }
  /**
   * Run inp2shp
   */
  return run_process (argv[1], (char *)params.c_str());
}

DWORD
run_process (char *szCommand, char *szArgs)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  char szCommandLine[4096];
  char *szEnvCOMSPEC = NULL;
  char szDefaultCMD[] = "CMD.EXE";
  ULONG uReturnCode;
  SECURITY_ATTRIBUTES saAttr;
  const int BUFSIZE = 4096;
  CHAR chBuf[BUFSIZE];
  DWORD dwRead;

  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  if (!CreatePipe(&g_hStdOutRd, &g_hStdOutWr, &saAttr, 0))
  {
	  MessageBox(NULL,
			  _("CreatePipe() failed."), title.c_str(), MB_ICONERROR);
      exit(1);
  }

  if (!SetHandleInformation(g_hStdOutRd, HANDLE_FLAG_INHERIT, 0))
  {
	  MessageBox(NULL,
			  _("SetHandleInformation() failed."),
			  title.c_str(), MB_ICONERROR);
	  exit(1);
  }

  ZeroMemory (&si, sizeof (STARTUPINFO));

  si.cb = sizeof (STARTUPINFO);

  si.wShowWindow = SW_SHOW;

  si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
  si.hStdOutput = g_hStdOutWr;
  si.hStdError = g_hStdOutWr;


  szCommandLine[0] = 0;


  strcat (szCommandLine, szCommand);

  strcat (szCommandLine, " ");

  strcat (szCommandLine, szArgs);

  if (!CreateProcess
      (NULL, szCommandLine, NULL, NULL, TRUE,
       0, NULL, NULL, &si, &pi))
    {

      return GetLastError ();

    }


  WaitForSingleObject (pi.hProcess, INFINITE);

  if (!GetExitCodeProcess (pi.hProcess, &uReturnCode))

    uReturnCode = 0;

  CloseHandle (pi.hThread);

  CloseHandle (pi.hProcess);

  if(ReadFile(g_hStdOutRd, chBuf, BUFSIZE, &dwRead, NULL))
	  MessageBox (NULL,
			  chBuf, title.c_str(), MB_ICONINFORMATION);

  return uReturnCode;

}

/**
 * Check if a given file exists.
 */
int file_exists(TCHAR * file)
{
   WIN32_FIND_DATA FindFileData;
   HANDLE handle = FindFirstFile(file, &FindFileData) ;
   int found = handle != INVALID_HANDLE_VALUE;
   if(found)
       FindClose(handle);
   return found;
}
