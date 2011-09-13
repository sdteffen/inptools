/**
 * inpproj.c Projection tool for EPANET INP files
 * 
 * Copyright (c) 2008 Steffen Macke <sdteffen@sdteffen.de>
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

#include "toolkit.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "inpproj.h"
#include <proj_api.h>

/**
 * These sections have to be projected. 
 */
char *SectTxtI[] = { "[COORDINATES]", "[VERTICES]", NULL
};

/**
 * Input file resource pointer. 
 */
FILE *InFile;

/**
 * Output file resource pointer.
 */
FILE *OutFile;

/**
 * Tokens in INP file 
 */
char *Tok[MAXTOKS];

/**
 * Project an EPANET INP file.
 * @param argc is the number of arguments provided from the command line 
 * (including the command name).
 * @param argv is an array of strings that contains the argc arguments.
 * @return An error code if something goes wrong or 0 if there was no error
 * during the conversion process.
 */
int main(int argc, char **argv)
{
	char line[MAXLINE];
	char wline[MAXLINE];
	int Ntokens;
	int newsect, sect;
	char *args_gk[] = { "+proj=tmerc", "+lat_0=0", "+lon_0=9", "+k=1",
		"+x_0=3500000",
		"+y_0=0", "+ellps=bessel", "+datum=potsdam", "+units=m",
		"+no_defs"
	};
	char *args_wgs84[] =
	    { "+proj=longlat", "+ellps=WGS84", "+datum=WGS84",
		"+no_defs"
	};
	projPJ pj_gk;
	projPJ pj_wgs84;
	double x;
	double y;
	double z;

	if (!(pj_gk = pj_init(10, args_gk))) {
		printf("failed to init GK projection\n");
		exit(1);
	}

	if (!(pj_wgs84 = pj_init(14, args_wgs84))) {
		printf("failed to init WGS84 projection\n");
		exit(1);
	}

	sect = -1;

  /**
   * Check parameters 
   */
	if (argc != 3) {
		printf
		    ("inpproj 0.0.1 (c) 2008 Steffen Macke <sdteffen@sdteffen.de>\n");
		printf("usage: inpproj input.inp output.inp\n");
		exit(1);
	}

  /**
   * Open the files 
   */
	InFile = fopen(argv[1], "rt");
	OutFile = fopen(argv[2], "wt");

	while (fgets(line, MAXLINE, InFile) != NULL) {
		strcpy(wline, line);
		Ntokens = gettokens(wline);
      /**
       * Skip blank lines and comments 
       */
		if (Ntokens == 0) {
			fprintf(OutFile, "%s", line);
			continue;
		}
		if (*Tok[0] == ';') {
			fprintf(OutFile, "%s", line);
			continue;
		}

		/* Check if max. length exceeded */
		if (strlen(line) >= MAXLINE) {
			printf("WARNING: Line too long.\n");
		}
		/* Check if line begins with a new section heading */
		if (*Tok[0] == '[') {
			fprintf(OutFile, "%s", line);
			sect = (-1);
			newsect = findmatch(Tok[0], SectTxtI);
			if (newsect >= 0) {
				sect = newsect;
				continue;
			} else
				continue;
		}

		switch (sect) {
			/* [COORDINATES] */
		case 0:
	/**
         * [VERTICES]
         */
		case 1:
			if (Ntokens == 3) {
				x = strtod(Tok[1], NULL);
				y = strtod(Tok[2], NULL);
				z = 0.0;
				pj_transform(pj_gk, pj_wgs84, 1, 0, &x, &y,
					     &z);
				fprintf(OutFile, " %s\t%f\t%f\n", Tok[0],
					x / DEG_TO_RAD, y / DEG_TO_RAD);
			}
			break;
		default:
			fprintf(OutFile, "%s", line);
			break;
		}
	}

	exit_inpproj(0);
	return 0;
}

/**
 * Exit the program with the given error code.
 * Closes open files if necessary.
 * @param error is the error code to be returned.
 */
void exit_inpproj(int error)
{
	if (OutFile)
		fclose(OutFile);
	if (InFile)
		fclose(InFile);
	exit(error);
}

int findmatch(char *line, char *keyword[])
/*
**--------------------------------------------------------------
**  Input:   *line      = line from input file
**           *keyword[] = list of NULL terminated keywords
**  Output:  returns index of matching keyword or
**           -1 if no match found
**  Purpose: determines which keyword appears on input line
**--------------------------------------------------------------
*/
{
	int i = 0;
	while (keyword[i] != NULL) {
		if (match(line, keyword[i]))
			return (i);
		i++;
	}
	return (-1);
}				/* end of findmatch */

int match(char *str, char *substr)
/*
**--------------------------------------------------------------
**  Input:   *str    = string being searched
**           *substr = substring being searched for
**  Output:  returns 1 if substr found in str, 0 if not
**  Purpose: sees if substr matches any part of str
**
**      (Not case sensitive)
**--------------------------------------------------------------
*/
{
	int i, j;

/*** Updated 9/7/00 ***/
/* Fail if substring is empty */
	if (!substr[0])
		return (0);

/* Skip leading blanks of str. */
	for (i = 0; str[i]; i++)
		if (str[i] != ' ')
			break;

/* Check if substr matches remainder of str. */
	for (i = i, j = 0; substr[j]; i++, j++)
		if (!str[i] || UCHAR(str[i]) != UCHAR(substr[j]))
			return (0);
	return (1);
}				/* end of match */


/*** Updated 10/25/00 ***/
/* The gettokens function has been totally re-written. */
int gettokens(char *s)
/*
**--------------------------------------------------------------
**  Input:   *s = string to be tokenized
**  Output:  returns number of tokens in s
**  Purpose: scans string for tokens, saving pointers to them
**           in module global variable Tok[]
**
** Tokens can be separated by the characters listed in SEPSTR
** (spaces, tabs, newline, carriage return) which is defined
** in TYPES.H. Text between quotes is treated as a single token.
**--------------------------------------------------------------
*/
{
	int len, m, n;
	char *c;

/* Begin with no tokens */
	for (n = 0; n < MAXTOKS; n++)
		Tok[n] = NULL;
	n = 0;

/* Truncate s at start of comment */
	c = strchr(s, ';');
	if (c)
		*c = '\0';
	len = strlen(s);

/* Scan s for tokens until nothing left */
	while (len > 0 && n < MAXTOKS) {
		m = strcspn(s, SEPSTR);	/* Find token length */
		len -= m + 1;	/* Update length of s */
		if (m == 0)
			s++;	/* No token found */

		else {
			if (*s == '"') {	/* Token begins with quote */
				s++;	/* Start token after quote */
				m = strcspn(s, "\"\n\r");	/* Find end quote (or EOL) */
			}
			s[m] = '\0';	/* Null-terminate the token */
			Tok[n] = s;	/* Save pointer to token */
			n++;	/* Update token count */
			s += m + 1;	/* Begin next token */
		}
	}
	return (n);
}				/* End of gettokens */
