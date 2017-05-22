// ITC2007Solution.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "string.h"
#include <iostream>  
#include <fstream>
using namespace std;

//ALGORITHM CONSTANTS
const int NUMBEROFSLOTS = 45;
const int NUMDAYS = 5;
const int SLOTSPERDAY = 9;
const int LASTSLOTINDEX = SLOTSPERDAY - 1;

//Forward declaration
void readInputFile(ifstream& inStream);
int* makeRoomSizeArray(ifstream& i_stream);
bool** makeAttendsMatrix(ifstream& inStream);
bool** makeRoomFeaturesMatrix(ifstream& inStream);
bool** makeEventFeaturesMatrix(ifstream& inStream);
bool** makeEventAvail(ifstream& inStream);
bool** makeRoomAvail();
int** makeBeforeMatrix(ifstream& inStream);

void printArray(int* array, int size);
void printMatrix(int** matrix, int height, int width);
void printMatrix(bool** matrix, int height, int width);


//	Arrays and variables used throughout the program
int numEvents, numRooms, numFeatures, numStudents, NUMBEROFPLACES;
int *roomSize;
int **before;
bool **attends, **roomFeatures, **eventFeatures, **eventAvail, **roomAvail;



int main(int argc, char**argv)
{
	char fileName[40];
	int i;
	ifstream inStream;

	for (i = 1; i < argc; i++)
	{
		strcpy(fileName, argv[i]);
		strcat(fileName, ".tim");
		inStream.open(fileName);
		readInputFile(inStream);
		inStream.close();

	}

    return 0;
}

void readInputFile(ifstream& inStream)
{
	//Read in the first line of the tim file
	inStream >> numEvents >> numRooms >> numFeatures >> numStudents;
	
	// Read the stuff from the files to do with rooms
	roomSize = makeRoomSizeArray(inStream);
	attends = makeAttendsMatrix(inStream);
	roomFeatures = makeRoomFeaturesMatrix(inStream);
	eventFeatures = makeEventFeaturesMatrix(inStream);
	eventAvail = makeEventAvail(inStream);

	NUMBEROFPLACES = numRooms*NUMBEROFSLOTS;

	// Make all true
	roomAvail = makeRoomAvail();

	// Finally, read in the precidence constraints and make before matrix;
	before = makeBeforeMatrix(inStream);

	printMatrix(roomFeatures, numRooms, numEvents);
}

int* makeRoomSizeArray(ifstream& i_stream) {
	int i;
	int* r_size;
	r_size = new int[numRooms];
	for (i = 0; i < numRooms; i++) i_stream >> r_size[i];
	return (r_size);
}
bool** makeAttendsMatrix(ifstream& i_stream) {
	int i, r, c;
	bool** att_matrix;
	att_matrix = new bool*[numStudents];
	for (i = 0; i<numStudents; i++)	att_matrix[i] = new bool[numEvents];
	for (r = 0; r < numStudents; r++) {
		for (c = 0; c < numEvents; c++) {
			i_stream >> i;
			if (i == 1)att_matrix[r][c] = true;
			else att_matrix[r][c] = false;
		}
	}
	return (att_matrix);
};

bool** makeRoomFeaturesMatrix(ifstream& i_stream) {
	int r, c, i;
	bool** r_features;
	r_features = new bool*[numRooms];
	for (i = 0; i<numRooms; i++)r_features[i] = new bool[numFeatures];
	for (r = 0; r < numRooms; r++) {
		for (c = 0; c < numFeatures; c++) {
			i_stream >> i;
			if (i == 1)r_features[r][c] = true;
			else r_features[r][c] = false;
		}
	}
	return (r_features);
}

bool** makeEventFeaturesMatrix(ifstream& i_stream) {
	int r, c, i;
	bool** e_features;
	e_features = new bool*[numEvents];
	for (i = 0; i <numEvents; i++) e_features[i] = new bool[numFeatures];
	for (r = 0; r < numEvents; r++) {
		for (c = 0; c < numFeatures; c++) {
			i_stream >> i;
			if (i == 1)e_features[r][c] = true;
			else e_features[r][c] = false;
		}
	}
	return (e_features);
}

bool** makeEventAvail(ifstream& i_stream) {
	int r, c, i;
	bool** evAvail;
	evAvail = new bool*[numEvents];
	for (i = 0; i <numEvents; i++) evAvail[i] = new bool[NUMBEROFSLOTS];
	for (r = 0; r < numEvents; r++) 
	{
		for (c = 0; c < NUMBEROFSLOTS; c++) 
		{
			i_stream >> i;
			if (i == 1) evAvail[r][c] = true;
			else evAvail[r][c] = false;
		}
	}
	return (evAvail);
}

// Make all rooms available
bool** makeRoomAvail()
{
	int r, c, i;
	bool** rmAvail;
	rmAvail = new bool*[numRooms];
	for (i = 0; i<numRooms; i++)rmAvail[i] = new bool[NUMBEROFSLOTS];
	for (r = 0; r<numRooms; r++)for (c = 0; c<NUMBEROFSLOTS; c++)rmAvail[r][c] = true;
	return (rmAvail);
}

int** makeBeforeMatrix(ifstream &i_stream)
{
	int r, c, i;
	int** before;
	before = new int*[numEvents];
	for (i = 0; i <numEvents; i++)before[i] = new int[numEvents];
	for (r = 0; r < numEvents; r++) {
		for (c = 0; c < numEvents; c++) {
			i_stream >> before[r][c];
		}
	}
	return (before);
}



// Help function to visualize arrays
void printArray(int* array, int size) {
	for (int i = 0; i < size; i++)
	{
		cout << array[i] << endl;
	}
}

// Help function to visualize integer matrices
void printMatrix(int** matrix, int rows, int cols) {
	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < cols; y++) {
			cout << matrix[x][y] << " ";
		}
		cout << endl;
	}
}

// Help function to visualize boolean matrices
void printMatrix(bool** matrix, int rows, int cols) {
	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < cols; y++) {
			cout << matrix[x][y] << " ";
		}
		cout << endl;
	}
}