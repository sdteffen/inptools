/**
 * inptools-file-dialog.c Display a file dialog and execute a command
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

DWORD run_process(char *szCommand, char *szArgs);
string& str_replace_null(const string &search, string &subject);

/**
 * USAGE: inptools-file-dialog filter command inputfile
 */
int main(int argc, char *argv[])
{
	TCHAR szParams[2*MAX_PATH +4];
	OPENFILENAME ofn;
	char szOutFileName[MAX_PATH] = "";
	DWORD return_value = 0;
	char szLang[MAX_PATH] = "";
	DWORD nEnvironmentRead = 0;

	std::string pattern;	

	if(4 != argc)
	{
		MessageBox(NULL,
			"USAGE: inptools-file-dialog filter command inputfile", 
			"inptools-file-dialog", MB_ICONERROR);
		return 1;
	}

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;

	pattern = argv[1];

	ofn.lpstrFilter = str_replace_null("\\n", pattern).c_str();
	ofn.lpstrFile = szOutFileName;

	ofn.nMaxFile = MAX_PATH;

	ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

	ofn.lpstrDefExt = "txt";

	if(!GetSaveFileName(&ofn))
		return 1;

	wsprintf(szParams, "\"%s\" \"%s\"", argv[3], szOutFileName);

	nEnvironmentRead =
	    GetEnvironmentVariable("LANG", szLang, MAX_PATH);

	if ((nEnvironmentRead > 0) && (nEnvironmentRead < MAX_PATH))
		SetEnvironmentVariable("LANG", "de");

	return_value = run_process(argv[2], szParams);

	if ((nEnvironmentRead > 0)
			    && (nEnvironmentRead < MAX_PATH))

	SetEnvironmentVariable("LANG", szLang);

	return return_value;
}

/**
 * Replace \n with \0
 */
string& str_replace_null(const string &search, string &subject)
{
	string result = "";
	int subject_length = subject.length();
	int i = 0;
	int j = 0;
	
	while(i != string::npos && i < subject_length)
	{
		j = subject.find(search, i);
		if(string::npos == j)
			result.append(subject.substr(i));
		else
		{
			result.append(subject.substr(i, j-i));
			result.push_back('\0');
			j += search.length();
		}
		i = j;
	}
    
    subject = result;
    return subject;
}

DWORD run_process(char *szCommand, char *szArgs)
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
