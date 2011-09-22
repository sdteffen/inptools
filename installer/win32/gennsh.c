/** 
 * Inptools - work with EPANET INP files
 * Copyright (C) 2011 Steffen Macke <sdteffen@sdteffen.de>
 *
 * gennsh.c
 *
 * gennsh is a program that allows to generate NSIS locale files
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <libintl.h>
#include <glib.h>

#define _(String) gettext(String)

int main(int argc, char *argv[])
{

  bindtextdomain("inptools", "../../../build/win32/locale");
  bind_textdomain_codeset("inptools", "UTF-8");
  textdomain("inptools");

  /* Shell context menu entry */
  printf("!define INPTOOLS_OPEN_WITH_EPANET \"%s\"\n", g_strescape(_("Open with EPANET"), ""));

  return 0;
}
