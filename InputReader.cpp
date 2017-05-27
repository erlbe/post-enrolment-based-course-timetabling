#include "InputReader.h"
//---------------------------------------------------------------------------------------
//                             READ INPUT FUNCTIONS
//---------------------------------------------------------------------------------------

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

bool** makeEventConflictMatrix()
{
	int i, r, c;
	// This describes which events can be placed in which time slots together. A
	// zero says there is no conflict, a 1 says there is a conflict (and the 
	// corresponding events may not be placed in the same time slot)
	bool** conflict = new bool*[numEvents];
	for (i = 0; i < numEvents; i++)conflict[i] = new bool[numEvents];
	//We fill the matrix
	for (r = 0; r < numEvents; r++)	for (c = 0; c < numEvents; c++)	conflict[r][c] = false;
	//Now, for each cell in the matrix, check to see if there is a student who wishes to take the 2 corresponding events. if so we set the cell to 1
	for (r = 0; r < numEvents; r++) {
		for (c = 0; c < numEvents; c++) {
			bool clash = false;
			i = 0;
			while ((i < numStudents) && (!clash)) {
				if ((attends[i][r] == true) && (attends[i][c] == true)) {
					conflict[r][c] = true;
					clash = true;
				}
				else i++;
			}
		}
	}
	return conflict;
}

int* makeEventSizeMatrix() {
	int r, c, total;
	int* eventSize = new int[numEvents];
	for (c = 0; c < numEvents; c++) {
		total = 0;
		for (r = 0; r < numStudents; r++) if (attends[r][c] == 1) total++;
		eventSize[c] = total;
	}
	return eventSize;
}
