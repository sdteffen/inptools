/**
 * epanet2csv.c convert binary epanet results file to csv format
 *
 * Copyright (C) 2001, 2009 Steffen Macke <sdteffen@sdteffen.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

int writeNodesFile(FILE * fileIn, char *strNodeFilename);
int writeLinksFile(FILE * fileIn, char *strLinkFilename);

int numNodes;
int numLinks;
int numTanks;
int numPumps;
int numPeriods;
int numReportStart;
int numTimeStep;
int numDuration;

int main(int argc, char **argv)
{
	FILE *fileIn;

	if (argc == 4) {
		if ((fileIn = fopen(argv[1], "r+b")) == NULL) {
			printf("Error opening input file.\n");
			exit(0);
		}
		fseek(fileIn, 8, SEEK_SET);
		fread(&numNodes, 4, 1, fileIn);
		printf("Number of Nodes: %d\n", numNodes);

		fseek(fileIn, 16, SEEK_SET);
		fread(&numLinks, 4, 1, fileIn);
		printf("Number of Links: %d\n", numLinks);

		fseek(fileIn, 20, SEEK_SET);
		fread(&numPumps, 4, 1, fileIn);
		printf("Number of Pumps: %d\n", numPumps);

		fseek(fileIn, 12, SEEK_SET);
		fread(&numTanks, 4, 1, fileIn);
		printf("Number of Tanks: %d\n", numTanks);

		fseek(fileIn, 48, SEEK_SET);
		fread(&numReportStart, 4, 1, fileIn);
		printf("Report Start: %d\n", numReportStart);

		fseek(fileIn, 52, SEEK_SET);
		fread(&numTimeStep, 4, 1, fileIn);
		printf("Report Time Step: %d\n", numTimeStep);

		fseek(fileIn, 56, SEEK_SET);
		fread(&numDuration, 4, 1, fileIn);
		printf("Simulation Duration: %d\n", numDuration);

		numPeriods = (numDuration / numTimeStep) + 1;
		printf("Number of Periods: %d\n", numPeriods);

		writeNodesFile(fileIn, argv[2]);
		writeLinksFile(fileIn, argv[3]);

		/* close input file */
		fclose(fileIn);
	} else {
		printf("epanet2csv 1.0.0 (c) 2001, 2009 Steffen Macke\n");
		printf
		    ("usage: epanet2csv binary_result_file nodes_file.csv links_file.csv\n");
	}
}

/* writes id, demand, head, pressure,
   time to node output file
   returns 0 */
int writeNodesFile(FILE * fileIn, char *strNodeFilename)
{
	FILE *fileOut;
	int offsetResults;
	int offsetNodeIDs;
	char strNodeID[33];
	float demand;
	float head;
	float pressure;
	float quality;
	int i;
	int j;

	if ((fileOut = fopen(strNodeFilename, "w")) == NULL) {
		printf("Error: can't open nodes output file.\n");
	} else {
		printf("writing nodes text file ...\n");

		offsetNodeIDs = 884;
		offsetResults =
		    offsetNodeIDs + (36 * numNodes) + (52 * numLinks) +
		    (8 * numTanks) + (28 * numPumps) + 4;
		fprintf(fileOut,
			"node_id,result_demand,result_head,result_pressure,result_quality,timestep\n");
		for (i = 0; i < numPeriods; i++) {
			for (j = 0; j < numNodes; j++) {
				fseek(fileIn, offsetNodeIDs + (j * 32),
				      SEEK_SET);
				fread(strNodeID, 32, 1, fileIn);
				fseek(fileIn, offsetResults + (j * 4),
				      SEEK_SET);
				fread(&demand, 4, 1, fileIn);
				fseek(fileIn,
				      offsetResults + ((numNodes + j) * 4),
				      SEEK_SET);
				fread(&head, 4, 1, fileIn);
				fseek(fileIn,
				      offsetResults +
				      ((2 * numNodes + j) * 4), SEEK_SET);
				fread(&pressure, 4, 1, fileIn);
				fseek(fileIn,
				      offsetResults +
				      ((3 * numNodes + j) * 4), SEEK_SET);
				fread(&quality, 4, 1, fileIn);
				fprintf(fileOut, "%s,%f,%f,%f,%f,%d\n",
					strNodeID, demand, head, pressure,
					quality, i + 1);
			}
			offsetResults += (16 * numNodes + 32 * numLinks);
		}

		fclose(fileOut);
		printf("... done.\n");
	}
	return 0;
}

/* writes id, flow, velocity, headloss
   time  to link output file
   returns 0 */
int writeLinksFile(FILE * fileIn, char *strLinkFilename)
{
	FILE *fileOut;
	int offsetResults;
	int offsetLinkIDs;
	char strLinkID[17];
	float flow;
	float velocity;
	float headloss;
	float quality;
	float status;
	float setting;
	float reaction;
	float friction;
	int i;
	int j;

	if ((fileOut = fopen(strLinkFilename, "w")) == NULL) {
		printf("Error: can't open links output file.\n");
	} else {
		printf("writing links text file ...\n");
		offsetLinkIDs = 884 + (32 * numNodes);
		offsetResults =
		    884 + (36 * numNodes) + (52 * numLinks) +
		    (8 * numTanks) + (28 * numPumps) + 4 + (16 * numNodes);

		fprintf(fileOut,
			"link_id,result_flow,result_velocity,result_headloss,result_quality,result_status,result_reaction,result_friction,timestep\n");
		for (i = 0; i < numPeriods; i++) {
			for (j = 0; j < numLinks; j++) {

				fseek(fileIn, offsetLinkIDs + (j * 32),
				      SEEK_SET);
				fread(strLinkID, 16, 1, fileIn);
				fseek(fileIn, offsetResults + (j * 4),
				      SEEK_SET);
				fread(&flow, 4, 1, fileIn);
				fseek(fileIn,
				      offsetResults + ((numLinks + j) * 4),
				      SEEK_SET);
				fread(&velocity, 4, 1, fileIn);
				fseek(fileIn,
				      offsetResults +
				      ((2 * numLinks + j) * 4), SEEK_SET);
				fread(&headloss, 4, 1, fileIn);
				fseek(fileIn,
				      offsetResults +
				      ((3 * numLinks + j) * 4), SEEK_SET);
				fread(&quality, 4, 1, fileIn);
				fseek(fileIn,
				      offsetResults +
				      ((4 * numLinks + j) * 4), SEEK_SET);
				fread(&status, 4, 1, fileIn);
				fseek(fileIn,
				      offsetResults +
				      ((5 * numLinks + j) * 4), SEEK_SET);
				fread(&setting, 4, 1, fileIn);
				fseek(fileIn,
				      offsetResults +
				      ((6 * numLinks + j) * 4), SEEK_SET);
				fread(&reaction, 4, 1, fileIn);
				fseek(fileIn,
				      offsetResults +
				      ((7 * numLinks + j) * 4), SEEK_SET);
				fread(&friction, 4, 1, fileIn);
				fprintf(fileOut,
					"%s,%f,%f,%f,%f,%f,%f,%f,%f,%d\n",
					strLinkID, flow, velocity,
					headloss, quality, status, setting,
					reaction, friction, i + 1);
			}
			offsetResults += (16 * numNodes + 32 * numLinks);
		}
		fclose(fileOut);
		printf("... done\n");
	}
	return 0;
}
