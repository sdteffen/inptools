/**
 * inp2shp.c convert EPANET INP files to Shapefiles
 * 
 * (c) 2002, 2005 DORSCH Consult
 * (c) 2006 DC Water and Environment
 * (c) 2008, 2009, 2012, 2014 Steffen Macke <sdteffen@sdteffen.de>
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

#include <toolkit.h>
#include <shapefil.h>
#include <stdlib.h>
#include <string.h>
#include "inp2shp.h"

/**
 * These sections have to be parsed in addition
 * to the parsing performed by the toolkit. 
 */
char *SectTxtI[] = { "[COORDINATES]", "[VERTICES]", "[END]",
	"[PIPES]", NULL
};

/**
 * INP file resource pointer. 
 */
FILE *InFile;

/**
 * Junction shapefile handle.
 */
SHPHandle hJunctionSHP = NULL;
/**
 * Junction table handle.
 */
DBFHandle hJunctionDBF = NULL;

/**
 * Pipe shapefile handle.
 */
SHPHandle hPipeSHP = NULL;
DBFHandle hPipeDBF = NULL;

/* tank shapefile */
SHPHandle hTankSHP = NULL;
DBFHandle hTankDBF = NULL;

/* reservoir shapefile */
SHPHandle hReservoirSHP = NULL;
DBFHandle hReservoirDBF = NULL;

/* valve shapefile */
SHPHandle hValveSHP = NULL;
DBFHandle hValveDBF = NULL;

/* pump shapefile */
SHPHandle hPumpSHP = NULL;
DBFHandle hPumpDBF = NULL;

/**
 * Tokens in INP file 
 */
char *Tok[MAXTOKS];

/**
 * Number of junctions in the generated shapefile.
 * Counted while writing to the shapefile.
 */
int num_junctions = 0;
/**
 * Number of pipes in the generated shapefile,
 * maybe higher than the number of EPANET pipes.
 */
int num_pipes = 0;
int num_tanks = 0;
int num_reservoirs = 0;
int num_pumps = 0;
int num_valves = 0;

char vertex_line_name[16] = "";
/**
 * An index of a link in the list of links.
 */
int vertex_line_index = 0;
int num_vertices = -1;

/** 
 * Caching the vertices of a line while
 * parsing the vertices section
 * \todo Dynamically allocate according to model size.
 */
double vertex_x[MAXNUMNODES];
double vertex_y[MAXNUMNODES];

/**
 * Cache of the node vertices.
 * \todo Dynamically allocate according to model size.
 */
double node_x[MAXNUMNODES];
double node_y[MAXNUMNODES];

/**
 * The name of the junction shapefile.
 */
char *junctions_name = "";
/**
 * The name of the pipes shapefile.
 */
char *pipes_name = "";
/**
 * The name of the tanks shapefile.
 */
char *tanks_name = "";
/**
 * The name of the reservoir shapefile.
 */
char *reservoirs_name = "";
/**
 * The name of the pump shapefile.
 */
char *pumps_name = "";
/**
 * The name of the valve shapefile.
 */
char *valves_name = "";

/**
 * Convert an EPANET INP file to a series of shape files.
 * @param argc is the number of arguments provided from the command line 
 * (including the command name).
 * @param argv is an array of strings that contains the argc arguments.
 * @return An error code if something goes wrong or 0 if there was no error
 * during the conversion process.
 * \todo Allow generation of empty report file.
 */
int main(int argc, char **argv)
{
	int error;
	char line[MAXLINE];
	char wline[MAXLINE];
	int Ntokens;
	int newsect, sect;
	int i;

	sect = -1;

	strcpy(vertex_line_name, "");
	num_vertices = 0;

	initialize();

	/* parameter check */
	if ((argc != 9) ||
	    ((!str_is_shp(argv[3]) || (!str_is_shp(argv[4]) ||
				       (!str_is_shp(argv[5])
					|| (!str_is_shp(argv[6])
					    || (!str_is_shp(argv[7])
						||
						(!str_is_shp
						 (argv[8]))))))))) {
		printf
		    ("inp2shp 0.3.0\nCopyright (c) 2002-2006 DC Water and Environment, (c) 2009-2014 Steffen Macke\n");
		printf
		    ("usage: inp2shp inpfile reportfile junction_shapefile\
 pipe_shapefile pump_shapefile reservoir_shapefile tank_shapefile\
 valve_shapefile\n");
		exit(1);
	}

	for (i = 3; i < 9; i++) {
		remove_shp(argv[i]);
	}

  /**
   * Open the files 
   */
	error = ENopen(argv[1], argv[2], "");
	if (error != 0) {
		fprintf(stderr,
			"FATAL ERROR: EPANET returned error %d when opening '%s'.\n",
			error, argv[1]);
		ENclose();
		exit(error);
	}
	junctions_name = argv[3];
	pipes_name = argv[4];
	pumps_name = argv[5];
	reservoirs_name = argv[6];
	tanks_name = argv[7];
	valves_name = argv[8];

	create_junction_shapefile(junctions_name);
	create_pipe_shapefile(pipes_name);
	create_pump_shapefile(pumps_name);
	create_reservoir_shapefile(reservoirs_name);
	create_tank_shapefile(tanks_name);
	create_valve_shapefile(valves_name);
	InFile = fopen(argv[1], "rt");

	while (fgets(line, MAXLINE, InFile) != NULL) {
		strcpy(wline, line);
		Ntokens = en_gettokens(wline);
		/* Skip blank lines and comments */
		if (Ntokens == 0)
			continue;
		if (*Tok[0] == ';')
			continue;

		/* Check if max. length exceeded */
		if (strlen(line) >= MAXLINE) {
			printf("WARNING: Line too long.\n");
		}
		/* Check if line begins with a new section heading */
		if (*Tok[0] == '[') {
			/* [VERTICES] */
			if ((sect == 1) && (num_vertices > 0))
				write_pipe_shape();
			/* [COORDINATES] */
			if (sect == 0)
				handle_virtual_line_nodes();
			sect = (-1);
			newsect = en_findmatch(Tok[0], SectTxtI);
			if (newsect >= 0) {
				sect = newsect;
				if (sect == 2)
					break;
				continue;
			} else
				continue;
		}

		switch (sect) {
			/* [COORDINATES] */
		case 0:
			if (Ntokens == 3)
				write_node();
			break;
			/* [VERTICES] */
		case 1:
			if (Ntokens == 3)
				write_vertex();
			break;
			/* [PIPES] */
		case 3:
			if (Ntokens > 3) {
				write_null_pipe();
			} else {
				fprintf(stderr,
					"WARNING: Less than 4 tokens for a pipe.");
			}
			break;
		}
	}

	write_remaining_pipe_shapes();
	write_virtual_lines();

	exit_inp2shp(0);
	return 0;
}

/**
 * Initialize global variables.
 */
void initialize()
{
	int i;
  /**
   * \todo This can be optimized
   */
	for (i = 0; i < 5000; i++) {
		vertex_x[i] = 0;
		vertex_y[i] = 0;
		node_x[i] = 0;
		node_y[i] = 0;
	}
	for (i = 0; i < MAXTOKS; i++) {
		Tok[i] = NULL;
	}
}

/**
 * Exit the program with the given error code.
 * Closes open files if necessary.
 * @param error is the error code to be returned.
 */
void exit_inp2shp(int error)
{
	if (hPipeSHP) {
		SHPClose(hPipeSHP);
	}
	if (hPipeDBF)
		DBFClose(hPipeDBF);
	if (hJunctionDBF)
		DBFClose(hJunctionDBF);
	if (hJunctionSHP) {
		SHPClose(hJunctionSHP);
	}
	if (hTankSHP) {
		SHPClose(hTankSHP);
	}
	if (hTankDBF)
		DBFClose(hTankDBF);
	if (hReservoirSHP) {
		SHPClose(hReservoirSHP);
	}
	if (hReservoirDBF)
		DBFClose(hReservoirDBF);
	if (hPumpSHP) {
		SHPClose(hPumpSHP);
	}
	if (hPumpDBF)
		DBFClose(hPumpDBF);
	if (hValveSHP) {
		SHPClose(hValveSHP);
	}
	if (hValveDBF)
		DBFClose(hValveDBF);

	if (num_junctions == 0)
		remove_shp(junctions_name);
	if (num_pipes == 0)
		remove_shp(pipes_name);
	if (num_pumps == 0)
		remove_shp(pumps_name);
	if (num_tanks == 0)
		remove_shp(tanks_name);
	if (num_reservoirs == 0)
		remove_shp(reservoirs_name);
	if (num_valves == 0)
		remove_shp(valves_name);

	if (error == 0) {
		print_statistics();
	}

	ENclose();

	exit(error);
}

/**
 * Write those pipes which have no additional vertices and are no
 * virtual lines.
 */
void write_remaining_pipe_shapes()
{
	int i;
	SHPObject *shape;
	double x[2], y[2];
	char *pipe_id;
	int pipe_index;
	int from_node, to_node;
	int error;

	for (i = 0; i < num_pipes; i++) {
    /**
     * \todo performance increase with SHPGetInfo 
     */
		shape = SHPReadObject(hPipeSHP, i);
		if (NULL == shape) {
			fprintf(stderr,
				"FATAL ERROR: Read pipe shape returned NULL in write_remaining_pipe_shapes().\n");
			exit_inp2shp(1);
		}
		x[0] = 0;
		y[0] = 0;
		x[1] = 0;
		y[1] = 0;
		if (shape->nSHPType == SHPT_NULL) {
			SHPDestroyObject(shape);
			pipe_id =
			    (char *) DBFReadStringAttribute(hPipeDBF, i,
							    0);
			error = ENgetlinkindex(pipe_id, &pipe_index);
			if (0 != error) {
				fprintf(stderr,
					"FATAL ERROR: ENgetlinkindex(\"%s\") returned error %d in write_remaining_pipe_shapes().\n",
					pipe_id, error);
				exit_inp2shp(1);
			}
			error =
			    ENgetlinknodes(pipe_index, &from_node,
					   &to_node);
			if (0 != error) {
				fprintf(stderr,
					"FATAL ERROR: ENgetlinknodes(%d) returned error %d in write_remaining_pipe_shapes().\n",
					pipe_index, error);
				exit_inp2shp(1);
			}
			x[0] = node_x[from_node];
			y[0] = node_y[from_node];
			x[1] = node_x[to_node];
			y[1] = node_y[to_node];
			shape =
			    SHPCreateSimpleObject(SHPT_ARC, 2, x, y, NULL);
			if (-1 == SHPWriteObject(hPipeSHP, i, shape)) {
				SHPDestroyObject(shape);
				fprintf(stderr,
					"FATAL ERROR: SHPWriteObject failed in write_remaining_pipe_shapes().\n");
				exit_inp2shp(1);
			}
			SHPDestroyObject(shape);
		} else {
			SHPDestroyObject(shape);
		}
	}
	return;
}

/**
 * Turn virtual lines (valves, pumps) into points. 
 */
void handle_virtual_line_nodes()
{
	int i;
	int from_node, to_node;
	int num_lines;
	int linktype;
	int error;

	error = ENgetcount(EN_LINKCOUNT, &num_lines);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetcount(EN_LINKCOUNT) returned error %d in handle_virtual_line_nodes().\n",
			error);
		exit_inp2shp(1);
	}
	for (i = 0; i < num_lines; i++) {
		error = ENgetlinktype(i + 1, &linktype);
		if (0 != error) {
			fprintf(stderr,
				"FATAL ERROR: ENgetlinktype(%d) returned error %d in handle_virtual_line_nodes().\n",
				i + 1, error);
			exit_inp2shp(1);
		}
		switch (linktype) {
		case EN_PIPE:
		case EN_CVPIPE:
			break;
		default:
			error =
			    ENgetlinknodes(i + 1, &from_node, &to_node);
			if (0 != error) {
				fprintf(stderr,
					"FATAL ERROR: ENgetlinknodes(%d) returned error %d in handle_virtual_line_nodes().\n",
					i + 1, error);
				exit_inp2shp(1);
			}
			if ((to_node >= MAXNUMNODES)
			    || (from_node >= MAXNUMNODES)) {
				fprintf(stderr,
					"FATAL ERROR: Maximum number (%d) of nodes exceeded.\n",
					MAXNUMNODES);
				exit_inp2shp(1);
			}
			break;
		}
	}
	return;
}

/**
 * Write pumps and valves
 */
int write_virtual_lines()
{
  /**
   * \todo Remove superfluous nodes. 
   */
	int i;
	int num_lines;
	int linktype;
	int error;

	error = ENgetcount(EN_LINKCOUNT, &num_lines);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetcount(EN_LINKCOUNT) returned error %d in write_virtual_lines().\n",
			error);
		exit_inp2shp(1);
	}
	for (i = 0; i < num_lines; i++) {
		error = ENgetlinktype(i + 1, &linktype);
		if (0 != error) {
			fprintf(stderr,
				"FATAL ERROR: ENgetlinktype(%d) returned error %d in write_virtual_lines().\n",
				i + 1, error);
			exit_inp2shp(1);
		}
		switch (linktype) {
		case EN_PIPE:
		case EN_CVPIPE:
      /**
       * \todo Create valve for EN_CVPIPE.
       */
			break;
		case EN_PUMP:
			write_pump(i + 1);
			write_virtual_line_parts(i +1);
			break;
		default:
			write_valve(i + 1);
			write_virtual_line_parts(i +1);
			break;
		}
	}
	return 0;
}

/**
 * Write two dummy pipes replacing the virtual line.
 * @param index int is the index of virtual line in the list of links.
 * @return 0 of there is no error.
 */
int write_virtual_line_parts(int index)
{
	SHPObject *shape;
	double x[2], y[2];
	float d;
	int to_node, from_node;
	char string[16];
	int type;
	int error;

#ifdef DEBUG
	fprintf(stderr, "write_virtual_line_parts()\n");
#endif

	error = ENgetlinkid(index, string);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkid(%d) returned error %d in write_virtual_line_parts().\n",
			index, error);
		exit_inp2shp(1);
	}
	error = ENgetlinknodes(index, &from_node, &to_node);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinknodes(%d) returned error %d in write_virtual_line_parts().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteStringAttribute(hPipeDBF, num_pipes, PI_DC_ID, string);
	DBFWriteStringAttribute(hPipeDBF, num_pipes+1, PI_DC_ID, string);
	DBFWriteStringAttribute(hPipeDBF, num_pipes+1, PI_NODE1, string);
	DBFWriteStringAttribute(hPipeDBF, num_pipes, PI_NODE2, string);
	error = ENgetlinkvalue(index, EN_DIAMETER, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_DIAMETER) returned error %d in write_virtual_line_parts().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteIntegerAttribute(hPipeDBF, num_pipes, PI_DIAMETER, (int) d);
	DBFWriteIntegerAttribute(hPipeDBF, num_pipes+1, PI_DIAMETER, (int) d);
	error = ENgetlinkvalue(index, EN_LENGTH, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_LENGTH) returned error %d in write_virtual_line_parts().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteIntegerAttribute(hPipeDBF, num_pipes, PI_LENGTH, (int) d/2);
	DBFWriteIntegerAttribute(hPipeDBF, num_pipes+1, PI_LENGTH, (int) d/2);
	error = ENgetlinknodes(index, &from_node, &to_node);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinknodes(%d) returned error %d in write_virtual_line_parts().\n",
			index, error);
		exit_inp2shp(1);
	}

	find_from_node(from_node, string);
	DBFWriteStringAttribute(hPipeDBF, num_pipes, PI_NODE1, string);
	error = ENgetnodeid(to_node, string);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodeid(%d) returned error %d in write_virtual_line_parts().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteStringAttribute(hPipeDBF, num_pipes+1, PI_NODE2, string);

	error = ENgetlinkvalue(index, EN_ROUGHNESS, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_ROUGHNESS) returned error %d in write_virtual_line_parts().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteDoubleAttribute(hPipeDBF, num_pipes, PI_ROUGHNESS, (double) d);
	DBFWriteDoubleAttribute(hPipeDBF, num_pipes+1, PI_ROUGHNESS, (double) d);

	error = ENgetlinkvalue(index, EN_MINORLOSS, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_MINORLOSS) returned error %d in write_virtual_line_parts().\n",
			index, error);
		exit_inp2shp(1);
	}

	DBFWriteDoubleAttribute(hPipeDBF, num_pipes, PI_MINORLOSS, (double) d);
	DBFWriteDoubleAttribute(hPipeDBF, num_pipes+1, PI_MINORLOSS, (double) d);

	error = ENgetlinkvalue(index, EN_INITSTATUS, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_INITSTATUS) returned error %d in write_virtual_line_parts().\n",
			index, error);
		exit_inp2shp(1);
	}
	if (d == 1) {
		DBFWriteStringAttribute(hPipeDBF, num_pipes, PI_STATUS,
					"OPEN");
		DBFWriteStringAttribute(hPipeDBF, num_pipes+1, PI_STATUS,
							"OPEN");
	} else {
		DBFWriteStringAttribute(hPipeDBF, num_pipes, PI_STATUS,
					"CLOSED");

		DBFWriteStringAttribute(hPipeDBF, num_pipes+1, PI_STATUS,
					"CLOSED");
	}
	/**
	 * Write first shape.
	 */
	x[0] = node_x[from_node];
	y[0] = node_y[from_node];
	x[1] = (node_x[to_node] + node_x[from_node])/2;
	y[1] = (node_y[to_node] + node_y[from_node])/2;
	shape =
	    SHPCreateSimpleObject(SHPT_ARC, 2, x, y, NULL);
	if (-1 == SHPWriteObject(hPipeSHP, -1, shape)) {
		SHPDestroyObject(shape);
		fprintf(stderr,
			"FATAL ERROR: SHPWriteObject failed in write_virtual_line_parts().\n");
		exit_inp2shp(1);
	}
	SHPDestroyObject(shape);
	/**
	 * Write second shape.
	 */
	x[0] = x[1];
	y[0] = y[1];
	x[1] = node_x[to_node];
	y[1] = node_y[to_node];
	shape =
	    SHPCreateSimpleObject(SHPT_ARC, 2, x, y, NULL);
	if (-1 == SHPWriteObject(hPipeSHP, -1, shape)) {
		SHPDestroyObject(shape);
		fprintf(stderr,
			"FATAL ERROR: SHPWriteObject failed in write_virtual_line_parts().\n");
		exit_inp2shp(1);
	}
	SHPDestroyObject(shape);

	num_pipes += 2;
	return 0;	
}


/**
 * Write a pump
 * @param index int is the index of the pump in the list of pipes.
 * @return 0 if there is no error.
 */
int write_pump(int index)
{
	SHPObject *shape;
	double x, y;
	int from_node, to_node;
	char string[16];
	float d;
	float power;
	int type;
	int error;

	error = ENgetlinkid(index, string);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkid(%d) returned error %d in write_pump().\n",
			index, error);
		exit_inp2shp(1);
	}
	error = ENgetlinknodes(index, &from_node, &to_node);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinknodes(%d) returned error %d in write_pump().\n",
			index, error);
		exit_inp2shp(1);
	}
	x = (node_x[from_node]+node_x[to_node])/2;
	y = (node_y[from_node]+node_y[to_node])/2;
	shape = SHPCreateSimpleObject(SHPT_POINT, 1, &x, &y, NULL);
	error = SHPWriteObject(hPumpSHP, -1, shape);
	SHPDestroyObject(shape);
	if (-1 == error) {
		fprintf(stderr,
			"FATAL ERROR: SHPWriteObject failed in write_pump().\n");
		exit_inp2shp(1);
	}
	DBFWriteStringAttribute(hPumpDBF, num_pumps, 0, string);
	error = ENgetnodevalue(from_node, EN_ELEVATION, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodevalue(%d, EN_ELEVATION) returned error %d in write_pump().\n",
			from_node, error);
		exit_inp2shp(1);
	}
	DBFWriteDoubleAttribute(hPumpDBF, num_pumps, 5, (double) d);
	error = ENgetlinkvalue(index, EN_INITSETTING, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_INITSETTING) returned error %d in write_pump().\n",
			index, error);
		exit_inp2shp(1);
	}
	error = ENgetpumptype(index, &type);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetpumptype(%d) returned error %d in write_pump().\n",
			index, error);
		exit_inp2shp(1);
	}
	strcpy(Tok[0], "");
	switch (type) {
	case EN_CONST_HP:
		error = ENgetlinkvalue(index, EN_MINORLOSS, &power);
		if (0 != error) {
			fprintf(stderr,
				"FATAL ERROR: ENgetlinkvalue(%d, EN_MINORLOSS) returned error %d in write_pump().\n",
				index, error);
			exit_inp2shp(1);
		}
		sprintf(Tok[0], "POWER %f SPEED %f", power, d);
		break;
	case EN_CUSTOM:
		error = ENgetheadcurve(index, string);
		if (0 != error) {
			fprintf(stderr,
				"FATAL ERROR: ENgetheadcurve(%d) returned error %d in write_pump().\n",
				index, error);
			exit_inp2shp(1);
		}
		sprintf(Tok[0], "HEAD %s SPEED %f", string, d);
		break;
	}

	DBFWriteStringAttribute(hPumpDBF, num_pumps, 11, Tok[0]);

	num_pumps++;

	return 0;
}

/**
 * Write a valve record to the shapefile.
 * @param index int is the index of the valve in the list of links.
 * @return 0 if there is no error.
 */
int write_valve(int index)
{
	SHPObject *shape;
	double x, y;
	int from_node, to_node;
	char string[16];
	float d;
	int type;
	int error;

	error = ENgetlinkid(index, string);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkid(%d) returned error %d in write_valve().\n",
			index, error);
		exit_inp2shp(1);
	}
	error = ENgetlinknodes(index, &from_node, &to_node);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinknodes(%d) returned error %d in write_valve().\n",
			index, error);
		exit_inp2shp(1);
	}
	x = (node_x[from_node]+node_x[to_node])/2;
	y = (node_y[from_node]+node_y[to_node])/2;
	shape = SHPCreateSimpleObject(SHPT_POINT, 1, &x, &y, NULL);
	error = SHPWriteObject(hValveSHP, -1, shape);
	SHPDestroyObject(shape);
	if (-1 == error) {
		fprintf(stderr,
			"FATAL ERROR: SHPWriteObject failed in write_valve().\n");
		exit_inp2shp(1);
	}
	DBFWriteStringAttribute(hValveDBF, num_valves, 0, string);
	error = ENgetnodevalue(from_node, EN_ELEVATION, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodevalue(%d, EN_ELEVATION) returned error %d in write_valve().\n",
			from_node, error);
		exit_inp2shp(1);
	}
	DBFWriteDoubleAttribute(hValveDBF, num_valves, 5, (double) d);
	error = ENgetlinkvalue(index, EN_DIAMETER, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_DIAMETER) returned error %d in write_valve().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteDoubleAttribute(hValveDBF, num_valves, 11, (double) d);
	error = ENgetlinktype(index, &type);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinktype(%d) returned error %d in write_valve().\n",
			index, error);
		exit_inp2shp(1);
	}
	switch (type) {
	case EN_CVPIPE:
		DBFWriteStringAttribute(hValveDBF, num_valves, 12, "CV");
		break;
	case EN_PRV:
		DBFWriteStringAttribute(hValveDBF, num_valves, 12, "PRV");
		break;
	case EN_PSV:
		DBFWriteStringAttribute(hValveDBF, num_valves, 12, "PSV");
		break;
	case EN_PBV:
		DBFWriteStringAttribute(hValveDBF, num_valves, 12, "PBV");
		break;
	case EN_TCV:
		DBFWriteStringAttribute(hValveDBF, num_valves, 12, "TCV");
		break;
	case EN_GPV:
		DBFWriteStringAttribute(hValveDBF, num_valves, 12, "GPV");
		break;
	}
	error = ENgetlinkvalue(index, EN_INITSETTING, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_INITSETTING) returned error %d in write_valve().\n",
			index, error);
		exit_inp2shp(1);
	}
	sprintf(Tok[0], "%f", d);
	DBFWriteStringAttribute(hValveDBF, num_valves, 13, Tok[0]);
	error = ENgetlinkvalue(index, EN_MINORLOSS, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_MINORLOSS) returned error %d in write_valve().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteDoubleAttribute(hValveDBF, num_valves, 14, (double) d);

	num_valves++;
	return 1;
}

/**
 * Call the node writing functions for the current node.
 * Virtual line nodes are dropped.
 * @return 0 if there's an error. 1 if there's no error.
 * \todo Handle return values of called functions.
 */
int write_node()
{
	int index;
	int nodetype;
	int linkindex = 1;
	int linktype;
	int linkcount;
	int fromnode;
	int tonode;
	int error;

	error = ENgetnodeindex(Tok[0], &index);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodeindex(%s) returned error %d in write_node().\n",
			Tok[0], error);
		exit_inp2shp(1);
	}
#ifdef DEBUG
	fprintf(stderr, "write_node(): Node %d\n", index);
#endif

	error = ENgetcount(EN_LINKCOUNT, &linkcount);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkcount(%s) returned error %d in write_node().\n",
			Tok[0], error);
		exit_inp2shp(1);
	}
	if (0 == index) {
		fprintf(stderr,
			"ERROR: Node \"%s\" referenced in [COORDINATES] was not found.\n",
			Tok[0]);
		return 0;
	}
	if (index >= MAXNUMNODES) {
		fprintf(stderr,
			"FATAL ERROR: Maximum number (%d) of nodes exceeded.\n",
			MAXNUMNODES);
		exit_inp2shp(1);
	}
	node_x[index] = atof(Tok[1]);
	node_y[index] = atof(Tok[2]);



	error = ENgetnodetype(index, &nodetype);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodetype(%d) returned error %d in write_node().\n",
			index, error);
		exit_inp2shp(1);
	}
	switch (nodetype) {
	case EN_JUNCTION:
		write_junction(index);
		break;
	case EN_TANK:
		write_tank(index);
		break;
	case EN_RESERVOIR:
		write_reservoir(index);
		break;
	default:
		fprintf(stderr, "FATAL ERROR: Unknown node type \"%d\".\n",
			nodetype);
		exit_inp2shp(1);
		break;
	}
	return 1;
}

/**
 * Write a vertex of a pipe.
 */
int write_vertex()
{
	int linktype;
  /**
   * An index to a link in the list of links.
   */
	int index;
	int error;

	if (strcmp(vertex_line_name, Tok[0]) != 0) {
		error = ENgetlinkindex(Tok[0], &index);
		if (0 != error) {
			fprintf(stderr,
				"FATAL ERROR: ENgetlinkindex(%s) returned error %d in write_vertex().\n",
				Tok[0], error);
			exit_inp2shp(1);
		}
		if (index == 0) {
			printf("[COORDINATES]: link '%s' was not found\n",
			       Tok[0]);
			return 0;
		}
		if (strcmp(vertex_line_name, "") != 0) {
			error =
			    ENgetlinktype(vertex_line_index, &linktype);
			if (0 != error) {
				fprintf(stderr,
					"FATAL ERROR: ENgetlinktype(%d) returned error %d in write_vertex().\n",
					vertex_line_index, error);
				exit_inp2shp(1);
			}
			switch (linktype) {
			case EN_CVPIPE:
			case EN_PIPE:
				write_pipe_shape();
				break;
			}
		}
		num_vertices = 1;
		vertex_line_index = index;
		strcpy(vertex_line_name, Tok[0]);
	} else {
		num_vertices++;
	}
	if (num_vertices >= MAXNUMNODES) {
		fprintf(stderr,
			"FATAL ERROR: Maximum number (%d) of vertices exceeded.\n",
			MAXNUMNODES);
		exit_inp2shp(1);
	}
	vertex_x[num_vertices] = atof(Tok[1]);
	vertex_y[num_vertices] = atof(Tok[2]);

	return 0;
}

/**
 * Write a junction shape and the related DBF records.
 * @return 1 if no error occurs, 0 if there was an error.
 * \todo Use return shapelib return values and handle errors.
 */
int write_junction(int index)
{
	double x, y;
	char string[MAXLINE];
	SHPObject *shape;
	float f;
	int returnvalue;
	int error;

	x = atof(Tok[1]);
	y = atof(Tok[2]);

	error = ENgetnodeid(index, string);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodeid(%d) returned error %d in write_junction().\n",
			index, error);
		exit_inp2shp(1);
	}

	shape = SHPCreateSimpleObject(SHPT_POINT, 1, &x, &y, NULL);
	returnvalue = SHPWriteObject(hJunctionSHP, -1, shape);
#ifdef DEBUG
	fprintf(stderr, "SHPWriteObject() returned %d\n", returnvalue);
#endif
	SHPDestroyObject(shape);
	if (-1 == returnvalue) {
		fprintf(stderr,
			"FATAL ERROR: SHPWriteObject() failed in write_junction().\n");
		exit_inp2shp(1);
	}
	returnvalue =
	    DBFWriteStringAttribute(hJunctionDBF, num_junctions, 0,
				    string);
#ifdef DEBUG
	fprintf(stderr,
		"DBFWriteStringAttribute(%d, 0, \"%s\") returned %d\n",
		num_junctions, string, returnvalue);
#endif
	error = ENgetnodevalue(index, EN_ELEVATION, &f);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodevalue(%d, EN_ELEVATION) returned error %d in write_junction().\n",
			index, error);
		exit_inp2shp(1);
	}
	returnvalue =
	    DBFWriteDoubleAttribute(hJunctionDBF, num_junctions, 5,
				    (double) f);
#ifdef DEBUG
	fprintf(stderr, "DBFWriteDoubleAttribute(%d, 5, %f) returned %d\n",
		num_junctions, f, returnvalue);
#endif
	error = ENgetnodevalue(index, EN_BASEDEMAND, &f);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodevalue(%d, EN_BASEDEMAND) returned error %d in write_junction().\n",
			index, error);
		exit_inp2shp(1);
	}
	returnvalue =
	    DBFWriteDoubleAttribute(hJunctionDBF, num_junctions, 9,
				    (double) f);
#ifdef DEBUG
	fprintf(stderr, "DBFWriteDoubleAttribute(%d, 9, %f) returned %d\n",
		num_junctions, f, returnvalue);
#endif
	error = ENgetnodevalue(index, EN_PATTERN, &f);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodevalue(%d, EN_PATTERN) returned error %d in write_junction().\n",
			index, error);
		exit_inp2shp(1);
	}
  /**
   * Only write the pattern if there is one.
   */
	if (0 != (int) f) {
		error = ENgetpatternid((int) f, string);
		if (0 != error) {
			fprintf(stderr,
				"FATAL ERROR: ENgetpatternid(%d) returned error %d in write_junction().\n",
				(int) f, error);
			exit_inp2shp(1);
		}
		returnvalue =
		    DBFWriteStringAttribute(hJunctionDBF, num_junctions,
					    10, string);
#ifdef DEBUG
		fprintf(stderr,
			"DBFWriteStringAttribute(%d, 10, \"%s\") returned %d\n",
			num_junctions, string, returnvalue);
#endif
	}
	num_junctions++;
	return 0;
}

/**
 * Write a tank shape and the related DBF records.
 * @return 0 if no error occurs, 1 if there was an error.
 * \todo Use return shapelib return values and handle errors.
 */
int write_tank(int index)
{
	double x, y;
	char string[MAXLINE];
	SHPObject *shape;
	float f;
	int error;

	x = atof(Tok[1]);
	y = atof(Tok[2]);
	error = ENgetnodeid(index, string);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodeid(%d) returned error %d in write_tank().\n",
			index, error);
		exit_inp2shp(1);
	}

	shape = SHPCreateSimpleObject(SHPT_POINT, 1, &x, &y, NULL);
	SHPWriteObject(hTankSHP, -1, shape);
	SHPDestroyObject(shape);
	DBFWriteStringAttribute(hTankDBF, num_tanks, 0, string);
	error = ENgetnodevalue(index, EN_ELEVATION, &f);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodevalue(%d, EN_ELEVATION) returned error %d in write_tank().\n",
			index, error);
		exit_inp2shp(1);
	}

	DBFWriteDoubleAttribute(hTankDBF, num_tanks, 5, (double) f);
	error = ENgetnodevalue(index, EN_TANKLEVEL, &f);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodevalue(%d, EN_TANKLEVEL) returned error %d in write_tank().\n",
			index, error);
		exit_inp2shp(1);
	}

	DBFWriteDoubleAttribute(hTankDBF, num_tanks, 9, (double) f);
	error = ENgetnodevalue(index, EN_PATTERN, &f);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodevalue(%d, EN_PATTERN) returned error %d in write_tank().\n",
			index, error);
		exit_inp2shp(1);
	}
  /**
   * \todo: Complete Tank attribute handling.
   */
  /**
   * Write dummy minimum level to overcome "NUMBER NULL" errors
   */
	DBFWriteDoubleAttribute(hTankDBF, num_tanks, 10, 0.0);
  /**
   * Write dummy maximum level to overcome "NUMBER NULL" errors
   */
	DBFWriteDoubleAttribute(hTankDBF, num_tanks, 11, 0.0);
  /**
   * Write dummy diameter to overcome "NUMBER NULL" errors
   */
	DBFWriteDoubleAttribute(hTankDBF, num_tanks, 12, 0.0);
  /**
   * Write dummy minimum volume to overcome "NUMBER NULL" errors
   */
	DBFWriteDoubleAttribute(hTankDBF, num_tanks, 13, 0.0);
  /**
   * Write empty volume curve to overcome "NULL" errors  
   */
	DBFWriteStringAttribute(hTankDBF, num_tanks, 14, "");

	num_tanks++;
	return 0;
}

/**
 * Write a reservoir shape and the related DBF records.
 * @return 0 if no error occurs, 1 if there was an error.
 * \todo Use return shapelib return values and handle errors.
 * \todo Complete attribute import.
 */
int write_reservoir(int index)
{
	double x, y;
	char string[MAXLINE];
	SHPObject *shape;
	int error;
	float f;

	x = atof(Tok[1]);
	y = atof(Tok[2]);

	error = ENgetnodeid(index, string);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodeid(%d) returned error %d in write_reservoir().\n",
			index, error);
		exit_inp2shp(1);
	}

	shape = SHPCreateSimpleObject(SHPT_POINT, 1, &x, &y, NULL);
	SHPWriteObject(hReservoirSHP, -1, shape);
	SHPDestroyObject(shape);
	DBFWriteStringAttribute(hReservoirDBF, num_reservoirs, 0, string);
	error = ENgetnodevalue(index, EN_ELEVATION, &f);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodevalue(%d, EN_ELEVATION) returned error %d in write_reservoir().\n",
			index, error);
		exit_inp2shp(1);
	}
  /**
   * Write head to head and elevation fields.
   */
	error = DBFWriteDoubleAttribute(hReservoirDBF, num_reservoirs, 5,
					(double) f);
	error = DBFWriteDoubleAttribute(hReservoirDBF, num_reservoirs, 9,
					(double) f);

	error = ENgetnodevalue(index, EN_PATTERN, &f);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodevalue(%d, EN_PATTERN) returned error %d in write_reservoir().\n",
			index, error);
		exit_inp2shp(1);
	}
  /**
   * Only write the pattern if there is one.
   */
	if (0 != (int) f) {
		error = ENgetpatternid((int) f, string);
		if (0 != error) {
			fprintf(stderr,
				"FATAL ERROR: ENgetpatternid(%d) returned error %d in write_reservoir().\n",
				(int) f, error);
			exit_inp2shp(1);
		}
		error =
		    DBFWriteStringAttribute(hReservoirDBF, num_reservoirs,
					    10, string);
#ifdef DEBUG
		fprintf(stderr,
			"DBFWriteStringAttribute(%d, 10, \"%s\") returned %d in write_reservoir()\n",
			num_junctions, string, error);
#endif
	}
	num_reservoirs++;
	return 0;
}

/**
 * Find the from ID for a given pipe.
 * Drop virtual line IDs.
 * @param linkindex is the index of the link in question.
 * @param nodeid is a pointer to a string that should be filled with the from
 * node id.
 * @return 1 if everything is ok, 0 in case there's an error.
 * \todo Use 0 as "ok" return value (EPANET standard).
 */
int find_from_node(int nodeindex, char *nodeid)
{
	int error;

	error = ENgetnodeid(nodeindex, nodeid);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodeid(%d) returned error %d in find_from_node().\n",
			nodeindex, error);
		exit_inp2shp(1);
	}
	return 1;
}

/**
 * Write a NULL shape for all pipes.
 * \todo Handle return values.
 * @return 0 if there is no error.
 */
int write_null_pipe()
{
	SHPObject *shape;
	int index;
	double x[2], y[2];
	float d;
	int to_node, from_node;
	char string[16];
	int type;
	int error;

#ifdef DEBUG
	fprintf(stderr, "write_null_pipe()\n");
#endif
	error = ENgetlinkindex(Tok[0], &index);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkindex(%s) returned error %d in write_null_pipe().\n",
			Tok[0], error);
		exit_inp2shp(1);
	}
	DBFWriteStringAttribute(hPipeDBF, num_pipes, 0, Tok[0]);
	error = ENgetlinkvalue(index, EN_DIAMETER, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_DIAMETER) returned error %d in write_null_pipe().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteIntegerAttribute(hPipeDBF, num_pipes, 5, (int) d);
	error = ENgetlinkvalue(index, EN_LENGTH, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_LENGTH) returned error %d in write_null_pipe().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteIntegerAttribute(hPipeDBF, num_pipes, 14, (int) d);
	error = ENgetlinknodes(index, &from_node, &to_node);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinknodes(%d) returned error %d in write_null_pipe().\n",
			index, error);
		exit_inp2shp(1);
	}

	find_from_node(from_node, string);
	DBFWriteStringAttribute(hPipeDBF, num_pipes, PI_NODE1, string);
	error = ENgetnodeid(to_node, string);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetnodeid(%d) returned error %d in write_null_pipe().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteStringAttribute(hPipeDBF, num_pipes, 7, string);
	error = ENgetlinkvalue(index, EN_ROUGHNESS, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_ROUGHNESS) returned error %d in write_null_pipe().\n",
			index, error);
		exit_inp2shp(1);
	}
	DBFWriteDoubleAttribute(hPipeDBF, num_pipes, 8, (double) d);
	error = ENgetlinkvalue(index, EN_MINORLOSS, &d);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinkvalue(%d, EN_MINORLOSS) returned error %d in write_null_pipe().\n",
			index, error);
		exit_inp2shp(1);
	}

	DBFWriteDoubleAttribute(hPipeDBF, num_pipes, 9, (double) d);
	error = ENgetlinktype(index, &type);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinktype(%d) returned error %d in write_null_pipe().\n",
			index, error);
		exit_inp2shp(1);
	}

	if (type == EN_CVPIPE) {
		DBFWriteStringAttribute(hPipeDBF, num_pipes, 10, "CV");
	} else {
		error = ENgetlinkvalue(index, EN_INITSTATUS, &d);
		if (0 != error) {
			fprintf(stderr,
				"FATAL ERROR: ENgetlinkvalue(%d, EN_INITSTATUS) returned error %d in write_null_pipe().\n",
				index, error);
			exit_inp2shp(1);
		}

		if (d == 1) {
			DBFWriteStringAttribute(hPipeDBF, num_pipes, 10,
						"OPEN");
		} else {
			DBFWriteStringAttribute(hPipeDBF, num_pipes, 10,
						"CLOSED");
		}
	}
	x[0] = 0;
	y[0] = 0;
	x[1] = 1;
	y[1] = 1;
	shape = SHPCreateSimpleObject(SHPT_NULL, 2, x, y, NULL);
	SHPWriteObject(hPipeSHP, -1, shape);
	SHPDestroyObject(shape);
	num_pipes++;
	return 0;
}

/**
 * Write out pipe shapes where additional vertices are available.
 * @return 1 if no error was encountered. 0 if an error was encountered.
 * \todo Drop virtual lines. 
 * \todo Use 0 as "ok" return value (EPANET standard).
 */
int write_pipe_shape()
{
	char *string;
	SHPObject *shape;
	int to_node, from_node;
	int index = vertex_line_index;
	int error;

	error = ENgetlinknodes(index, &from_node, &to_node);
	if (0 != error) {
		fprintf(stderr,
			"FATAL ERROR: ENgetlinknodes(%d) returned error %d in write_pipe_shape().\n",
			index, error);
		exit_inp2shp(1);
	}
#ifdef DEBUG
	fprintf(stderr, "write_pipe_shape(%d, %d)\n", to_node, from_node);
#endif
	vertex_x[0] = node_x[from_node];
	vertex_y[0] = node_y[from_node];
	vertex_x[num_vertices + 1] = node_x[to_node];
	vertex_y[num_vertices + 1] = node_y[to_node];
  /**
   * Find pipe record in Shapefile.
   */
	do {
		string =
		    (char *) DBFReadStringAttribute(hPipeDBF, index, 0);
		index--;
	}
	while (((string == NULL)
		|| (strcmp(string, vertex_line_name) != 0))
	       && (index >= -1));
	if (index != -2) {
		shape = SHPCreateSimpleObject(SHPT_ARC, num_vertices + 2,
					      vertex_x, vertex_y, NULL);
		SHPWriteObject(hPipeSHP, index + 1, shape);
		SHPDestroyObject(shape);
	} else {
		fprintf(stderr,
			"ERROR: write_pipe_shape(): Could not find pipe record \"%s\" %d.\n",
			string, vertex_line_index);
	}

	return 1;
}

/**
 * Create a point shapefile with the necessary field structure.
 * @return 0 if there is no error.
 */
int create_junction_shapefile()
{
	hJunctionSHP = SHPCreate(junctions_name, SHPT_POINT);
	hJunctionDBF = DBFCreate(junctions_name);
	if (hJunctionSHP == NULL || hJunctionDBF == NULL) {
		fprintf(stderr,
			"FATAL ERROR: Unable to create file '%s'.\n",
			junctions_name);
		exit_inp2shp(1);
	}
	DBFAddField(hJunctionDBF, "dc_id", FTString, 16, 0);
	DBFAddField(hJunctionDBF, "installati", FTString, 16, 0);
	DBFAddField(hJunctionDBF, "abandon_da", FTString, 16, 0);
	DBFAddField(hJunctionDBF, "dcsubtype", FTInteger, 16, 0);
	DBFAddField(hJunctionDBF, "bitcodezon", FTInteger, 20, 0);
	DBFAddField(hJunctionDBF, "elevation", FTDouble, 16, 3);
	DBFAddField(hJunctionDBF, "result_dem", FTDouble, 16, 8);
	DBFAddField(hJunctionDBF, "result_hea", FTDouble, 16, 8);
	DBFAddField(hJunctionDBF, "result_pre", FTDouble, 16, 8);
	DBFAddField(hJunctionDBF, "demand", FTDouble, 16, 8);
	DBFAddField(hJunctionDBF, "pattern", FTString, 16, 0);
	num_junctions = 0;
	return 0;
}

/**
 * Create a point shapefile with the necessary field structure.
 * @return 0 if there is no error.
 */
int create_tank_shapefile(char *filename)
{
	hTankSHP = SHPCreate(filename, SHPT_POINT);
	hTankDBF = DBFCreate(filename);
	if (hTankSHP == NULL || hTankDBF == NULL) {
		fprintf(stderr,
			"FATAL ERROR: Unable to create file '%s'.\n",
			filename);
		exit_inp2shp(1);
	}
	DBFAddField(hTankDBF, "dc_id", FTString, 16, 0);	/* 0 */
	DBFAddField(hTankDBF, "installati", FTString, 16, 0);
	DBFAddField(hTankDBF, "abandon_da", FTString, 16, 0);
	DBFAddField(hTankDBF, "dcsubtype", FTInteger, 16, 0);
	DBFAddField(hTankDBF, "bitcodezon", FTInteger, 20, 0);
	DBFAddField(hTankDBF, "elevation", FTDouble, 16, 3);	/* 5 */
	DBFAddField(hTankDBF, "result_dem", FTDouble, 16, 8);
	DBFAddField(hTankDBF, "result_hea", FTDouble, 16, 8);
	DBFAddField(hTankDBF, "result_pre", FTDouble, 16, 8);
	DBFAddField(hTankDBF, "initiallev", FTDouble, 16, 8);
	DBFAddField(hTankDBF, "minimumlev", FTDouble, 16, 8);	/* 10 */
	DBFAddField(hTankDBF, "maximumlev", FTDouble, 16, 8);
	DBFAddField(hTankDBF, "diameter", FTDouble, 16, 8);
	DBFAddField(hTankDBF, "minimumvol", FTDouble, 16, 8);
	DBFAddField(hTankDBF, "volumecurv", FTString, 16, 0);
	num_tanks = 0;
	return 0;
}

/**
 * Create a point shapefile with the necessary field structure.
 * @return 0 if there is no error.
 */
int create_reservoir_shapefile(char *filename)
{
	hReservoirSHP = SHPCreate(filename, SHPT_POINT);
	hReservoirDBF = DBFCreate(filename);
	if (hReservoirSHP == NULL || hReservoirDBF == NULL) {
		/* TODO: close other shapefiles */
		fprintf(stderr,
			"FATAL ERROR: Unable to create file '%s'.\n",
			filename);
		exit_inp2shp(1);
	}
	DBFAddField(hReservoirDBF, "dc_id", FTString, 16, 0);	/* 0 */
	DBFAddField(hReservoirDBF, "installati", FTString, 16, 0);
	DBFAddField(hReservoirDBF, "abandon_da", FTString, 16, 0);
	DBFAddField(hReservoirDBF, "dcsubtype", FTInteger, 16, 0);
	DBFAddField(hReservoirDBF, "bitcodezon", FTInteger, 20, 0);
	DBFAddField(hReservoirDBF, "elevation", FTDouble, 16, 3);	/* 5 */
	DBFAddField(hReservoirDBF, "result_dem", FTDouble, 16, 8);
	DBFAddField(hReservoirDBF, "result_hea", FTDouble, 16, 8);
	DBFAddField(hReservoirDBF, "result_pre", FTDouble, 16, 8);
	DBFAddField(hReservoirDBF, "head", FTDouble, 16, 8);
	DBFAddField(hReservoirDBF, "pattern", FTString, 16, 0);	/* 10 */
	num_reservoirs = 0;
	return 0;
}

/**
 * Create a point shapefile with the necessary field structure.
 * @return 0 if there is no error.
 */
int create_pump_shapefile(char *filename)
{
	hPumpSHP = SHPCreate(filename, SHPT_POINT);
	hPumpDBF = DBFCreate(filename);
	if (hPumpSHP == NULL || hPumpDBF == NULL) {
		fprintf(stderr,
			"FATAL ERROR: Unable to create file '%s'.\n",
			filename);
		exit_inp2shp(1);
	}
	DBFAddField(hPumpDBF, "dc_id", FTString, 16, 0);	/* 0 */
	DBFAddField(hPumpDBF, "installati", FTString, 16, 0);
	DBFAddField(hPumpDBF, "abandon_da", FTString, 16, 0);
	DBFAddField(hPumpDBF, "dcsubtype", FTInteger, 16, 0);
	DBFAddField(hPumpDBF, "bitcodezon", FTInteger, 20, 0);
	DBFAddField(hPumpDBF, "elevation", FTDouble, 16, 3);	/* 5 */
	DBFAddField(hPumpDBF, "result_dem", FTDouble, 16, 8);
	DBFAddField(hPumpDBF, "result_hea", FTDouble, 16, 8);
	DBFAddField(hPumpDBF, "result_pre", FTDouble, 16, 8);
	DBFAddField(hPumpDBF, "result_flo", FTDouble, 16, 8);
	DBFAddField(hPumpDBF, "result_vel", FTDouble, 16, 8);	/* 10 */
	DBFAddField(hPumpDBF, "properties", FTString, 200, 0);
	DBFAddField(hPumpDBF, "power_kw", FTInteger, 16, 0);
	num_pumps = 0;
	return 0;
}

/**
 * Create a point shapefile with the necessary field structure.
 * @return 0 if there is no error.
 */
int create_valve_shapefile(char *filename)
{
	hValveSHP = SHPCreate(filename, SHPT_POINT);
	hValveDBF = DBFCreate(filename);
	if (hValveSHP == NULL || hValveDBF == NULL) {
		fprintf(stderr,
			"FATAL ERROR: Unable to create file '%s'.\n",
			filename);
		exit_inp2shp(1);
	}
	DBFAddField(hValveDBF, "dc_id", FTString, 16, 0);	/* 0 */
	DBFAddField(hValveDBF, "installati", FTString, 16, 0);
	DBFAddField(hValveDBF, "abandon_da", FTString, 16, 0);
	DBFAddField(hValveDBF, "dcsubtype", FTInteger, 16, 0);
	DBFAddField(hValveDBF, "bitcodezon", FTInteger, 20, 0);
	DBFAddField(hValveDBF, "elevation", FTDouble, 16, 3);	/* 5 */
	DBFAddField(hValveDBF, "result_dem", FTDouble, 16, 8);
	DBFAddField(hValveDBF, "result_hea", FTDouble, 16, 8);
	DBFAddField(hValveDBF, "result_pre", FTDouble, 16, 8);
	DBFAddField(hValveDBF, "result_flo", FTDouble, 16, 8);
	DBFAddField(hValveDBF, "result_vel", FTDouble, 16, 8);	/* 10 */
	DBFAddField(hValveDBF, "diameter", FTInteger, 20, 0);
	DBFAddField(hValveDBF, "type", FTString, 16, 0);
	DBFAddField(hValveDBF, "setting", FTString, 16, 0);
	DBFAddField(hValveDBF, "minorloss", FTDouble, 16, 8);
	num_valves = 0;
	return 0;
}

/**
 * Create a polyline shapefile with the necessary field structure. 
 * @return 0 if there is no error.
 */
int create_pipe_shapefile(char *filename)
{
	hPipeSHP = SHPCreate(filename, SHPT_ARC);
	hPipeDBF = DBFCreate(filename);
	if (hPipeSHP == NULL || hPipeDBF == NULL) {
		fprintf(stderr,
			"FATAL ERROR: Unable to create file '%s'.\n",
			filename);
		exit_inp2shp(1);
	}
	DBFAddField(hPipeDBF, "dc_id", FTString, 16, 0);	/* 0 */
	DBFAddField(hPipeDBF, "installati", FTString, 16, 0);
	DBFAddField(hPipeDBF, "abandon_da", FTString, 16, 0);
	DBFAddField(hPipeDBF, "dcsubtype", FTInteger, 16, 0);
	DBFAddField(hPipeDBF, "bitcodezon", FTInteger, 20, 0);
	DBFAddField(hPipeDBF, "diameter", FTInteger, 16, 0);	/* 5 */
	DBFAddField(hPipeDBF, "node1", FTString, 16, 0);
	DBFAddField(hPipeDBF, "node2", FTString, 16, 0);
	DBFAddField(hPipeDBF, "roughness", FTDouble, 16, 8);
	DBFAddField(hPipeDBF, "minorloss", FTDouble, 16, 8);
	DBFAddField(hPipeDBF, "status", FTString, 128, 0);	/* 10 */
	DBFAddField(hPipeDBF, "result_flow", FTDouble, 16, 8);
	DBFAddField(hPipeDBF, "result_velo", FTDouble, 16, 8);
	DBFAddField(hPipeDBF, "result_hea", FTDouble, 16, 8);
	DBFAddField(hPipeDBF, "length", FTDouble, 16, 2);
	num_pipes = 0;
	return 0;
}


/**
 * Print statistics about the generated shapefiles.
 * \todo Check WARNING about dropped junctions and pipes.
 * @return Nothing.
 */
void print_statistics()
{
  /**
   * Total number of junctions as reported by the EPANET toolkit.
   */
	int total_junctions = 0;
	int total_pipes = 0;
	int total_tanks = 0;
	int total_reservoirs = 0;
	int total_pumps = 0;
	int total_valves = 0;
	int total_virtual_lines = 0;
	int type;
	int i = 1;

	do {
		ENgetnodetype(i, &type);
		i++;
		switch (type) {
		case EN_JUNCTION:
			total_junctions++;
			break;
		case EN_RESERVOIR:
			total_reservoirs++;
			break;
		case EN_TANK:
			total_tanks++;
			break;
		}
	}
	while (type != -1);
	i = 1;
	do {
		ENgetlinktype(i, &type);
		i++;
		switch (type) {
		case EN_PIPE:
			total_pipes++;
			break;
		case EN_PUMP:
			total_pipes += 2;
			total_pumps++;
			break;
		case EN_CVPIPE:
		case EN_PRV:
		case EN_PSV:
		case EN_PBV:
		case EN_FCV:
		case EN_TCV:
		case EN_GPV:
			total_pipes += 2;
			total_valves++;
			break;
		}
	}
	while (type != -1);
	total_virtual_lines = total_pumps + total_valves;
	if (num_junctions > 0)
		printf("Wrote %d junctions to \"%s\".\n", num_junctions,
		       junctions_name);
	if (num_junctions + (2 * total_virtual_lines) < total_junctions) {
		fprintf(stderr,
			"WARNING: Dropped %d junctions without coordinates.\n",
			total_junctions - (num_junctions +
					   (2 * total_virtual_lines)));
	}
	if (num_pipes > 0)
		printf("Wrote %d pipes to \"%s\".\n", num_pipes,
		       pipes_name);
	if (num_pipes + (2 * total_virtual_lines) < total_pipes) {
		fprintf(stderr,
			"WARNING: Dropped %d pipes without coordinates.\n",
			total_pipes - (num_pipes +
				       (2 * total_virtual_lines)));
	}
	if (num_reservoirs > 0)
		printf("Wrote %d reservoirs to \"%s\".\n", num_reservoirs,
		       reservoirs_name);
	if (num_tanks > 0)
		printf("Wrote %d tanks to \"%s\".\n", num_tanks,
		       tanks_name);
	if (num_pumps > 0)
		printf("Wrote %d pumps to \"%s\".\n", num_pumps,
		       pumps_name);
	if (num_valves > 0)
		printf("Wrote %d valves to \"%s\".\n", num_valves,
		       valves_name);
}

/**
 * Determine whether a string is a shapefile name or not.
 * @param str is String that needs to be tested.
 * @return 1 if the string ends with "*.shp", otherwise 0 is returned.
 */
int str_is_shp(char *str)
{
	int len;

	len = strlen(str);
	if (strncmp(&str[len - 4], ".shp", 4) == 0) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * Remove the three files that make up a shapefile.
 * @param shapefilename is a string containing the name of the shapefile.
 * @return Nothing.
 */
void remove_shp(char *shapefilename)
{
	char filename[255];
	int len;
	int returnvalue;

	returnvalue = remove(shapefilename);
#ifdef DEBUG
	fprintf(stderr, "remove(\"%s\") returned %d.\n", shapefilename,
		returnvalue);
#endif
	len = strlen(shapefilename);
	if ((len < 255) && (len > 3)) {
		strncpy(filename, shapefilename, len - 3);
		strncpy(&filename[len - 3], "dbf", 4);
		returnvalue = remove(filename);
#ifdef DEBUG
		fprintf(stderr, "remove(\"%s\") returned %d.\n", filename,
			returnvalue);
#endif
		strncpy(&filename[len - 3], "shx", 4);
		returnvalue = remove(filename);
#ifdef DEBUG
		fprintf(stderr, "remove(\"%s\") returned %d.\n", filename,
			returnvalue);
#endif
	}
}


/**
  * Function copied from EPANET sources
  */
int  en_findmatch(char *line, char *keyword[])
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
   while (keyword[i] != NULL)
   {
      if (en_match(line,keyword[i])) return(i);
      i++;
   }
   return(-1);
}                        /* end of findmatch */

/**
 * Copied from EPANET sources
 */
int  en_gettokens(char *s)
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
   int  len, m, n;
   char *c;

/* Begin with no tokens */
   for (n=0; n<MAXTOKS; n++) Tok[n] = NULL;
   n = 0;

/* Truncate s at start of comment */
   c = strchr(s,';');
   if (c) *c = '\0';
   len = strlen(s);

/* Scan s for tokens until nothing left */
   while (len > 0 && n < MAXTOKS)
   {
       m = strcspn(s,SEPSTR);          /* Find token length */
       len -= m+1;                     /* Update length of s */
       if (m == 0) s++;                /* No token found */
       else
       {
          if (*s == '"')               /* Token begins with quote */
          {
             s++;                      /* Start token after quote */
             m = strcspn(s,"\"\n\r");  /* Find end quote (or EOL) */
          }                            
          s[m] = '\0';                 /* Null-terminate the token */
          Tok[n] = s;                  /* Save pointer to token */
          n++;                         /* Update token count */
          s += m+1;                    /* Begin next token */
       }
   }
   return(n);
}                        /* End of gettokens */


/**
 * Copied from EPANET sources.
 */
int  en_match(char *str, char *substr)
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
   int i,j;

/*** Updated 9/7/00 ***/
/* Fail if substring is empty */
   if (!substr[0]) return(0);

/* Skip leading blanks of str. */
   for (i=0; str[i]; i++)
     if (str[i] != ' ') break;

/* Check if substr matches remainder of str. */
   for (i=i,j=0; substr[j]; i++,j++)
      if (!str[i] || UCHAR(str[i]) != UCHAR(substr[j]))
         return(0);
   return(1);
}                        /* end of match */
