/* inp2shp.h convert EPANET INP files to Shapefiles
  
  (c) 2002, 2005 DORSCH Consult 
 (c) 2009 Steffen Macke
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
  
*/

#define MAXLINE 255
#define   SEPSTR    " \t\n\r"	/* Token separator characters */
#define   MAXTOKS   40		/* Max. items per line of input */
#define UCHAR(x) (((x) >= 'a' && (x) <= 'z') ? ((x)&~32) : (x))
/* #define DEBUG 1 */
#define MAXNUMNODES 5000

void remove_shp(char *shapefile);
int str_is_shp(char *str);
void initialize();
void exit_inp2shp(int error);
void write_remaining_pipe_shapes();
void handle_virtual_line_nodes();
void print_statistics();
int write_virtual_lines();
int write_pump(int index);
int write_valve(int index);
int write_node();
int write_vertex();
int write_junction(int index);
int write_tank(int index);
int write_reservoir(int index);
int write_null_pipe();
int write_pipe_shape();
int create_junction_shapefile();
int create_tank_shapefile(char *filename);
int create_reservoir_shapefile(char *filename);
int create_pump_shapefile(char *filename);
int create_valve_shapefile(char *filename);
int create_pipe_shapefile(char *filename);
int match(char *str, char *substr);

int  en_findmatch(char *line, char *keyword[]);
int  en_gettokens(char *s);
int  en_match(char *str, char *substr);