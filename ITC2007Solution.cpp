// ITC2007Solution.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "string.h"
#include <iostream>  
#include <fstream>
#include <vector>
#include <random>

using namespace std;

//ALGORITHM CONSTANTS
const int NUMBEROFSLOTS = 45;
const int NUMDAYS = 5;
const int SLOTSPERDAY = 9;
const int LASTSLOTINDEX = SLOTSPERDAY - 1;

// Definitions to use in the algorithm
typedef std::vector<int> IntVector;
typedef std::vector<IntVector> TwoDIntVector;

//Forward declaration
void readInputFile(ifstream& inStream);
int* makeRoomSizeArray(ifstream& i_stream);
bool** makeAttendsMatrix(ifstream& inStream);
bool** makeRoomFeaturesMatrix(ifstream& inStream);
bool** makeEventFeaturesMatrix(ifstream& inStream);
bool** makeEventAvail(ifstream& inStream);
bool** makeRoomAvail();
int** makeBeforeMatrix(ifstream& inStream);
bool** makeEventConflictMatrix();
int* makeEventSizeMatrix();
void outputSlnAnswerFile(TwoDIntVector&, char*);
int evaluateSolution(TwoDIntVector);

TwoDIntVector createRandomSolution();

void printArray(int* array, int size);
void printMatrix(int** matrix, int height, int width);
void printMatrix(bool** matrix, int height, int width);
void printMatrix(TwoDIntVector matrix, int rows, int cols);


//	Arrays and variables used throughout the program
int numEvents, numRooms, numFeatures, numStudents, NUMBEROFPLACES;
int *roomSize, *eventSize;
int **before;
bool **attends, **roomFeatures, **eventFeatures, **eventAvail, **roomAvail, **event_conflict;



int main(int argc, char**argv)
{
	char fileName[40], slnFileName[40];
	int i;
	ifstream inStream;

	for (i = 1; i < argc; i++)
	{
		strcpy(fileName, argv[i]);
		strcat(fileName, ".tim");
		strcpy(slnFileName, argv[i]);
		inStream.open(fileName);
		readInputFile(inStream);
		inStream.close();
	}
	// The solution will be a 2D vector with the timetable for each room
	TwoDIntVector theSolution;
	theSolution = createRandomSolution();
	printMatrix(theSolution, numRooms, NUMBEROFSLOTS);
	cout << "The evaluation of the random solution is: " << evaluateSolution(theSolution);

	// Create the file to be checked by the official solution validator
	outputSlnAnswerFile(theSolution, slnFileName);
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

	// Create confilct_event matrix with events that have one or more students that take another event 
	event_conflict = makeEventConflictMatrix();

	// Create eventSize matrix to decide the number of students attending each event. Used to make sure the rooms are big enough.
	eventSize = makeEventSizeMatrix();
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

//---------------------------------------------------------------------------------------
//                             ALGORITHM FUNCTIONS
//---------------------------------------------------------------------------------------

TwoDIntVector createRandomSolution() {
	//Make an empty the timetable
	TwoDIntVector theSolution;
	vector<vector<int> > temp(numRooms, vector<int>(NUMBEROFSLOTS, -1));
	theSolution.swap(temp);
	printMatrix(theSolution, numRooms, NUMBEROFSLOTS);

	// Place each event randomly
	for (int event = 0; event < numEvents; event++)
	{
		bool* currentEventFeatures = eventFeatures[event];

		int room=-1;
		int timeslot=-1;

		bool suitableRoom = false;
		bool suitableTimeslot = false;

		// Find a suitable room (has required features) and timeslot (isn't occupied)
		while (!suitableRoom || !suitableTimeslot) {
			// Room is suitable until proven otherwise
			suitableRoom = true;
			// Timeslot is suitable until proven otherwise
			suitableTimeslot = true;

			// Choose random room
			room = rand() % numRooms;
			bool* currentRoomFeatures = roomFeatures[room];			

			// Assign a random timeslot
			timeslot = rand() % NUMBEROFSLOTS;

			// Check if timeslot is available
			if (theSolution[room][timeslot] != -1) {
				suitableTimeslot = false;
			}
		}
		theSolution[room][timeslot] = event;
	}

	return theSolution;
}

int evaluateSolution(TwoDIntVector solution) {
	int totalCost = 0;

	// For each timeslot in each room

	for (int room = 0; room < numRooms; room++)
	{	
		// The features of this room
		bool* currentRoomFeatures = roomFeatures[room];
		for (int timeslot = 0; timeslot < NUMBEROFSLOTS; timeslot++)
		{
			int event = solution[room][timeslot];
			// If the timeslot has an event scheduled
			if (event != -1) {
				// HARD CONSTRAINT 1) no student attends more than one event at the same time;
				// Check if the event has conflict with another event in the same timeslot in another room
				for (int otherRoom = 0; otherRoom < numRooms; otherRoom++)
				{
					// Make sure we don't look at the same room
					if (room != otherRoom) {
						int otherEvent = solution[otherRoom][timeslot];
						// Check the event_conflict matrix
						if (event_conflict[event][otherEvent]) {
							totalCost++;
						}
					}
				}

				// HARD CONSTRAINT 2) the room is big enough for all the attending students and satisfies all the features required by the event;
				//Check if room satisfies the event feature

				// The features of the event
				bool* currentEventFeatures = eventFeatures[event];

				for (int feature = 0; feature < numFeatures; feature++) {
					// If a room has a required feature and the room doesn't have that feature it is unsuited
					if (currentEventFeatures[feature] && !currentRoomFeatures[feature]) {
						totalCost++;
					}
					// If a room is too small for the size (number of people attending) of an event
					if (roomSize[room] < eventSize[event]) {
						totalCost++;
					}
				}
				// HARD CONSTRAINT 3) only one event is put into each room in any timeslot;
				// This one is impossible to violate given how the solution is represented

				// HARD CONSTRAINT 4) events are only assigned to timeslots that are pre-defined as available for those events;
				if (!eventAvail[event][timeslot]) {
					totalCost++;
				}

				// HARD CONSTRAINT 5) where specified,  events are scheduled to occur in the correct order in the week;
				int* beforeEvent = before[event];
				int startTimeslot, stopTimeslot, shouldBeBefore;
				// Check if the timeslot we are looking at is in the first half of the week (So we only have to check if there are conflicts before or after - whichever is shortest)
				if (timeslot <= (float)NUMBEROFSLOTS / (float)2) {
					startTimeslot = 0;
					stopTimeslot = timeslot;
					shouldBeBefore = -1;
				}
				else {
					startTimeslot = timeslot;
					stopTimeslot = NUMBEROFSLOTS;
					shouldBeBefore = 1;
				}

				// Go through all the events that happens before or after the event we are looking at
				for (int otherRoom = 0; otherRoom < numRooms; otherRoom++)
				{
					for (int otherTimeslot = startTimeslot; otherTimeslot < stopTimeslot; otherTimeslot++) {
						int otherEvent = solution[otherRoom][otherTimeslot];
						if (beforeEvent[otherEvent] == shouldBeBefore) {
							totalCost++;
						}
					}
				}
			}
		}
	}
	return totalCost;
}

//---------------------------------------------------------------------------------------
//                             HELP FUNCTIONS
//---------------------------------------------------------------------------------------

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

// Help function to visualize integer double integers
void printMatrix(TwoDIntVector matrix, int rows, int cols) {
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

void outputSlnAnswerFile(TwoDIntVector &theSolution, char *slnfilename)
{
	//Outputs the best solution acheived in the desired format for the checker program
	char outfileName[50];
	strcpy(outfileName, slnfilename);
	strcat(outfileName, ".sln\0");
	ofstream ansStream;
	ansStream.open(outfileName);
	IntVector event_rooms(numEvents, -1), event_slots(numEvents, -1);
	int r, c, i;
	for (r = 0; r<numRooms; r++) {
		for (c = 0; c<NUMBEROFSLOTS; c++) {
			if (theSolution[r][c] != -1) {
				event_rooms[theSolution[r][c]] = r;
				event_slots[theSolution[r][c]] = c;
			}
		}
	}
	for (i = 0; i<numEvents; i++)ansStream << event_slots[i] << ' ' << event_rooms[i] << endl;
	ansStream.close();
}