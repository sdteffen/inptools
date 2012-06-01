/**
 * shp2inp.c convert a set of shapefiles into an EPANET INP file
 * 
 * Copyright (c) 2008,2012 Steffen Macke <sdteffen@sdteffen.de>
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

#include <shapefil.h>
#include <stdlib.h>
#include <string.h>
#include "shp2inp.h"

/**
 * INP file resource pointer. 
 */
FILE *OutFile;

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
 * Number of junctions
 */
int num_junctions = 0;
/**
 * Number of pipes
 */
int num_pipes = 0;
int num_tanks = 0;
int num_reservoirs = 0;
int num_pumps = 0;
int num_valves = 0;

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
 * Convert a set of shape files into an EPANET INP file.
 * @param argc is the number of arguments provided from the command line 
 * (including the command name).
 * @param argv is an array of strings that contains the argc arguments.
 * @return An error code if something goes wrong or 0 if there was no error
 * during the conversion process.
 */
int main(int argc, char **argv)
{
	int error;
	int i, j;
	int nRecords;
	SHPObject *oShape;

	/* parameter check */
	if ((argc != 8) ||
	    ((!str_is_shp(argv[1]) || (!str_is_shp(argv[2]) ||
				       (!str_is_shp(argv[3])
					|| (!str_is_shp(argv[4])
					    || (!str_is_shp(argv[5])
						||
						(!str_is_shp
						 (argv[6]))))))))) {
		printf
		    ("shp2inp 0.0.1 (c) 2008 Steffen Macke <sdteffen@sdteffen.de>\n");
		printf("usage: shp2inp junction_shapefile\
 pipe_shapefile pump_shapefile reservoir_shapefile tank_shapefile\
 valve_shapefile inpfile\n");
		exit(1);
	}


  /**
   * Open the files 
   */
	OutFile = fopen(argv[7], "wt");
	if (NULL == OutFile) {
		fprintf(stderr, "FATAL ERROR: Could not open '%s'.\n",
			argv[7]);
		exit(1);
	}
	junctions_name = argv[1];
	pipes_name = argv[2];
	pumps_name = argv[3];
	reservoirs_name = argv[4];
	tanks_name = argv[5];
	valves_name = argv[6];

  /**
   * Write junctions
   */
	fprintf(OutFile, "[JUNCTIONS]\n");
	hJunctionDBF = DBFOpen(junctions_name, "rb");
	if (!hJunctionDBF) {
		fprintf(stderr, "FATAL ERROR: Could not open '%s'.\n",
			junctions_name);
		exit_shp2inp(1);
	}
	hJunctionSHP = SHPOpen(junctions_name, "rb");
	if (!hJunctionSHP) {
		fprintf(stderr, "FATAL ERROR: Could not open '%s'.\n",
			junctions_name);
		exit_shp2inp(1);
	}
	nRecords = DBFGetRecordCount(hJunctionDBF);
	for (i = 0; i < nRecords; i++) {
		fprintf(OutFile, "%s %f %f %s\n",
			DBFReadStringAttribute(hJunctionDBF, i,
					       DBFGetFieldIndex
					       (hJunctionDBF, "dc_id")),
			DBFReadDoubleAttribute(hJunctionDBF, i,
					       DBFGetFieldIndex
					       (hJunctionDBF,
						"elevation")),
			DBFReadDoubleAttribute(hJunctionDBF, i,
					       DBFGetFieldIndex
					       (hJunctionDBF, "demand")),
			DBFReadStringAttribute(hJunctionDBF, i,
					       DBFGetFieldIndex
					       (hJunctionDBF, "pattern")));
	}
	fprintf(OutFile, "[COORDINATES]\n");
	for (i = 0; i < nRecords; i++) {
		oShape = SHPReadObject(hJunctionSHP, i);
		fprintf(OutFile, "%s %f %f\n",
			DBFReadStringAttribute(hJunctionDBF, i,
					       DBFGetFieldIndex
					       (hJunctionDBF, "dc_id")),
			oShape->dfXMin, oShape->dfYMin);
		SHPDestroyObject(oShape);
	}

  /**
   * Write pipes
   */
	fprintf(OutFile, "[PIPES]\n");
	hPipeDBF = DBFOpen(pipes_name, "rb");
	if (!hPipeDBF) {
		fprintf(stderr, "FATAL ERROR: Could not open '%s'.\n",
			pipes_name);
		exit_shp2inp(1);
	}
	hPipeSHP = SHPOpen(pipes_name, "rb");
	if (!hPipeSHP) {
		fprintf(stderr, "FATAL ERROR: Could not open '%s'.\n",
			pipes_name);
		exit_shp2inp(1);
	}
	nRecords = DBFGetRecordCount(hPipeDBF);
	for (i = 0; i < nRecords; i++) {
	/**
         * \TODO this is not complete!
         */
		fprintf(OutFile, "%s %s %s %f %d %f %f OPEN\n",
			DBFReadStringAttribute(hPipeDBF, i,
					       DBFGetFieldIndex(hPipeDBF,
								"dc_id")),
			DBFReadStringAttribute(hPipeDBF, i, 6
					       /*DBFGetFieldIndex(hPipeDBF, "NODE1") */
			), DBFReadStringAttribute(hPipeDBF, i, 7
						  /*DBFGetFieldIndex(hPipeDBF, "NODE2") */
			),
			DBFReadDoubleAttribute(hPipeDBF, i,
					       DBFGetFieldIndex(hPipeDBF,
								"length")),
			DBFReadIntegerAttribute(hPipeDBF, i,
						DBFGetFieldIndex(hPipeDBF,
								 "diameter")),
			DBFReadDoubleAttribute(hPipeDBF, i,
					       DBFGetFieldIndex(hPipeDBF,
								"roughness")),
			DBFReadDoubleAttribute(hPipeDBF, i,
					       DBFGetFieldIndex(hPipeDBF,
								"minorloss")));
	}
	fprintf(OutFile, "[VERTICES]\n");
	for (i = 0; i < nRecords; i++) {
		oShape = SHPReadObject(hPipeSHP, i);
		for (j = 0; j < oShape->nVertices; j++) {
			fprintf(OutFile, "%s %f %f\n",
				DBFReadStringAttribute(hPipeDBF, i,
						       DBFGetFieldIndex
						       (hPipeDBF,
							"dc_id")),
				oShape->padfX[j], oShape->padfY[j]);
		}
		SHPDestroyObject(oShape);
	}
	exit_shp2inp(0);

	return 0;
}

/**
 * Exit the program with the given error code.
 * Closes open files if necessary.
 * @param error is the error code to be returned.
 */
void exit_shp2inp(int error)
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

	if (OutFile)
		fclose(OutFile);

	if (error == 0) {
		print_statistics();
	}

	exit(error);
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
		exit_shp2inp(1);
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
		exit_shp2inp(1);
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
		exit_shp2inp(1);
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
		exit_shp2inp(1);
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
		exit_shp2inp(1);
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
