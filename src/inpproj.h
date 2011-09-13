/**
 * inpproj.h convert EPANET INP files to Shapefiles
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

#define MAXLINE 255
#define   SEPSTR    " \t\n\r"	/* Token separator characters */
#define   MAXTOKS   40		/* Max. items per line of input */
#define UCHAR(x) (((x) >= 'a' && (x) <= 'z') ? ((x)&~32) : (x))
/* #define DEBUG 1 */
#define MAXNUMNODES 5000

void exit_inpproj(int error);
