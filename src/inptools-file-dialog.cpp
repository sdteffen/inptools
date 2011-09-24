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

string& str_replace_null(const string &search, string &subject);

/**
 * USAGE: inptools-file-dialog filter command inputfile
 */
int main(int argc, char *argv[])
{
	TCHAR szCommand[MAX_PATH];
	TCHAR szParams[2 * MAX_PATH + 8];
	WORD lwPathChars = MAX_PATH;
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";
	char szOutFileName[MAX_PATH] = "";

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
	//ofn.lpstrFilter = _("Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0");
	//MessageBox(NULL, ofn.lpstrFilter, "", MB_ICONERROR);
	//MessageBox(NULL, "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0", "", MB_ICONERROR);
	//MessageBox(NULL, argv[1], "", MB_ICONERROR);
			ofn.lpstrFile = szOutFileName;

			ofn.nMaxFile = MAX_PATH;

			ofn.Flags =
			    OFN_EXPLORER | OFN_OVERWRITEPROMPT |
			    OFN_HIDEREADONLY;

			ofn.lpstrDefExt = "txt";

			if(!GetSaveFileName(&ofn))
				return E_INVALIDARG;

			wsprintf(szParams, "\"%s\" \"%s\"", argv[1],
				 szOutFileName);

			char szLang[MAX_PATH] = "";

			DWORD nEnvironmentRead = 0;

			nEnvironmentRead =
			    GetEnvironmentVariable("LANG", szLang,
						   MAX_PATH);

			if ((nEnvironmentRead > 0)
			    && (nEnvironmentRead < MAX_PATH))

				SetEnvironmentVariable("LANG", "de");

			execl(szCommand, szParams, TRUE);

			if ((nEnvironmentRead > 0)
			    && (nEnvironmentRead < MAX_PATH))

				SetEnvironmentVariable("LANG", szLang);

	return 0;
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
