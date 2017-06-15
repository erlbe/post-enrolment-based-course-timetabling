#include "InputReader.h"

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

	// Create confilct_event matrix with events that have one or more students that take another event 
	event_conflict = makeEventConflictMatrix();

	// Create totalNumConflict array which tells us how many conflicts each event has. Both with other events, and unavailable timeslots.
	totalNumConflict = makeTotalNumConflictArray();

	totalNumAvailableSlots = makeTotalNumAvailableSlotsArray();

	// Create eventSize matrix to decide the number of students attending each event. Used to make sure the rooms are big enough.
	eventSize = makeEventSizeMatrix();

	// Create eventRoom matrix which says if the room is suitable for a given event.
	eventRoom = makeEventRoomMatrix();

	// Create numSuitableRooms which says how many suitable rooms each event has
	numSuitableRooms = makeNumSuitableRoomsArray();

	// Create numSuitableEvents which says how many suitable events each room has
	numSuitableEvents = makeNumSuitableEventsArray();
}
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

//-------------------------------------------------------------------------------------
int* makeTotalNumConflictArray()
{
	//we construct a 1D array that tells us how many other events
	//each event conflicts with
	int* totalNumConflict = new int[numEvents];
	for (int r = 0; r<numEvents; r++) {
		int total = 0;
		for (int c = 0; c<numEvents; c++) if (event_conflict[r][c])total++;

		totalNumConflict[r] = total - 1;
	}
	return totalNumConflict;
}

int* makeTotalNumAvailableSlotsArray()
{
	int* totalNumAvailable = new int[numEvents];
	for (int r = 0; r<numEvents; r++) {
		int total = 0;
		for (int c = 0; c<NUMBEROFSLOTS; c++) if (eventAvail[r][c])total++;

		totalNumAvailable[r] = total - 1;
	}
	return totalNumAvailable;
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
bool** makeEventRoomMatrix()
{
	int i, r, c;
	bool** eventRoom = new bool*[numEvents];
	for (i = 0; i < numEvents; i++) eventRoom[i] = new bool[numRooms];
	//We step through each cell in turn in the matrix and we check to see
	//that if event r demands it, room c supplies it.
	for (r = 0; r < numEvents; r++) {
		for (c = 0; c < numRooms; c++) {
			int count = 0;
			int suitable = 1;
			while ((count < numFeatures) && (suitable == 1)) {
				if (eventFeatures[r][count] == 0) {
					//event r doesnt require the feature - we dont care
					count++;
				}
				else {
					//The event	must have a requirement
					if (roomFeatures[c][count] == 1) {
						//But its OK because room c supplies it
						count++;
					}
					else {
						//The room doesnt satisfy the requirement
						suitable = 0;
					}
				}
			}
			if (suitable == 1)	eventRoom[r][c] = true;
			else eventRoom[r][c] = false;
		}
	}

	//Now we modify the event room matrix using event_size array
	for (r = 0; r < numEvents; r++) {
		for (c = 0; c < numRooms; c++) {
			if (eventSize[r] > roomSize[c]) eventRoom[r][c] = false;
		}
	}

	/* From the PARTIALCOL alg.
	//For each event, calculate a list of the rooms it can go in.
	for (r = 0; r<numEvents; r++) {
		IntVector newIV;
		feasRooms.push_back(newIV);
		for (c = 0; c<numRooms; c++) {
			if (eventRoom[r][c]) feasRooms[r].push_back(c);
		}
	}
	*/
	return eventRoom;
}

int* makeNumSuitableRoomsArray() {
	//we construct a 1D array that tells us how many rooms each event can go into
	//in the empty timetable
	int *totalNumRooms = new int[numEvents];
	for (int r = 0; r<numEvents; r++) {
		int total = 0;
		for (int c = 0; c<numRooms; c++) if (eventRoom[r][c])total++;
		totalNumRooms[r] = total;
	}
	return(totalNumRooms);
}

int* makeNumSuitableEventsArray() {
	//we construct a 1D array that tells us how many events can go into each room
	//in the empty timetable
	int *totalNumEvents = new int[numRooms];
	for (int r = 0; r<numRooms; r++) {
		int total = 0;
		for (int c = 0; c<numEvents; c++) if (eventRoom[c][r])total++;
		totalNumEvents[r] = total;
	}
	return(totalNumEvents);
}