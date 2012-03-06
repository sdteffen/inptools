/**
 * inptools-about.c Display Inptools about dialog
 * 
 * (c) 2011,2012 Steffen Macke <sdteffen@sdteffen.de>
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

#include <windows.h>

int
main (int argc, char *argv[])
{
  MessageBox (NULL,
	      "Inptools 1.0.1\nCopyright (c) 2008 - 2012 Steffen Macke\nPortions Copyright (c) 1999, 2000 Frank Wamerdam\n\nhttp://epanet.de/inptools",
	      "Inptools", MB_OK);
  return 0;
}
