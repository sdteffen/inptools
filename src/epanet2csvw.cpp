/**
 * epanet2csvw.cpp Create CSV result files from EPANET INP file
 * 
 * (c) 2008-2011 Steffen Macke <sdteffen@sdteffen.de>
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

#define _(String) (String)

using namespace std;

DWORD run_process (char *szCommand, char *szArgs);
string & str_replace_null (const string & search, string & subject);
DWORD RunEpanet2csv (char *szBinaryFile, char *szNodesFile, char *szLinkFile);
DWORD CreateCsvFile (char *szNodeFile, char *szLinkFile);

/**
 * USAGE: epanet2csvw epanet2d epanet2csv inputfile filter1 filter2
 */
int
main (int argc, char *argv[])
{
  string params;
  OPENFILENAME ofn;
  char szOutFileName[MAX_PATH] = "";
  DWORD return_value = 0;
  DWORD dwBufSize = MAX_PATH;
  char *szArgs;
  TCHAR szReportFileName[MAX_PATH];
  TCHAR szBinaryFileName[MAX_PATH];
  TCHAR lpPathBuffer[MAX_PATH];
  int param_count;
  string pattern;
  UINT uRetVal;

  if (6 != argc)
    {
      MessageBox (NULL,
		  "USAGE: epanet2csvw epanet2d epanet2csv inputfile filter1 filter2",
		  "epanet2csvw", MB_ICONERROR);
      return 1;
    }

 /**
  * Find temp folder
  */
  return_value = GetTempPath (dwBufSize, lpPathBuffer);
  if (return_value > dwBufSize || (return_value == 0))
    {
      MessageBox (NULL,
		  _("Temp folder could not be obtained."),
		  _("Inptools Shell Extension"), MB_ICONERROR);
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
		  _("Inptools Shell Extension"), MB_ICONERROR);
      return 1;
    }

    /**
     * Get temporary binary result file.
     */
  uRetVal = GetTempFileName (lpPathBuffer, TEXT ("NEW"), 0, szBinaryFileName);
  if (uRetVal == 0)
    {
      MessageBox (NULL,
		  _("Temporary file name could not be obtained."),
		  _("Inptools Shell Extension"), MB_ICONERROR);
      return 1;
    }

  params = "\"";
  params.append (argv[3]);
  params.append ("\" \"");
  params.append (szReportFileName);
  params.append ("\" ");
  params.append (szBinaryFileName);
  params.append ("\" ");
  szArgs = (char *) params.c_str ();
  run_process (argv[1], szArgs);

  /**
   * Run epanet2csv
   */
  params = "\"";
  params.append (szBinaryFileName);
  params.append ("\" ");
  for (param_count = 4; param_count < argc; param_count++)
    {
      ZeroMemory (&ofn, sizeof (ofn));
      ofn.lStructSize = sizeof (ofn);
      ofn.hwndOwner = NULL;

      pattern = argv[param_count];

      ofn.lpstrFilter = str_replace_null ("\\n", pattern).c_str ();
      ofn.lpstrFile = szOutFileName;

      ofn.nMaxFile = MAX_PATH;

      ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

	  /**
           * @todo Extract from pattern
           */
      ofn.lpstrDefExt = "txt";

      if (!GetSaveFileName (&ofn))
	return 1;

      params.append ("\"");
      params.append (szOutFileName);
      params.append ("\" ");
    }
  szArgs = (char *) params.c_str ();
  return_value = run_process (argv[2], szArgs);

  return return_value;
}

/**
 * Replace \n with \0
 */
string & str_replace_null (const string & search, string & subject)
{
  string result = "";
  int subject_length = subject.length ();
  int i = 0;
  int j = 0;

  while (i != string::npos && i < subject_length)
    {
      j = subject.find (search, i);
      if (string::npos == j)
	result.append (subject.substr (i));
      else
	{
	  result.append (subject.substr (i, j - i));
	  result.push_back ('\0');
	  j += search.length ();
	}
      i = j;
    }

  subject = result;
  return subject;
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

  ZeroMemory (&si, sizeof (STARTUPINFO));

  si.cb = sizeof (STARTUPINFO);

  si.wShowWindow = SW_HIDE;

  si.dwFlags = STARTF_USESHOWWINDOW;


  szCommandLine[0] = 0;


  strcat (szCommandLine, szCommand);

  strcat (szCommandLine, " ");

  strcat (szCommandLine, szArgs);

  if (!CreateProcess
      (NULL, szCommandLine, NULL, NULL, FALSE,
       CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {

      return GetLastError ();

    }


  WaitForSingleObject (pi.hProcess, INFINITE);

  if (!GetExitCodeProcess (pi.hProcess, &uReturnCode))

    uReturnCode = 0;


  CloseHandle (pi.hThread);

  CloseHandle (pi.hProcess);


  return uReturnCode;

}
