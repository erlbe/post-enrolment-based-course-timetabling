#include "ITC2007Solution.h"

//	Arrays and variables used throughout the program
int numEvents, numRooms, numFeatures, numStudents, NUMBEROFPLACES;
int *roomSize, *eventSize;
int **before, **currentEventPlace;
bool **attends, **roomFeatures, **eventFeatures, **eventAvail, **roomAvail, **event_conflict, **eventRoom;

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

	// Seed the random function
	srand(time(NULL));

	// The solution will be a 2D vector with the timetable for each room
	TwoDIntVector theSolution;
	theSolution = createRandomSolution();
	printMatrix(theSolution, numRooms, NUMBEROFSLOTS);
	int bestEvaluation = evaluateSolution(theSolution);
	cout << "The evaluation of the random solution is: " << bestEvaluation << endl;

	while (bestEvaluation > 20) {
		TwoDIntVector* neighbours = generateNeighbours(theSolution);
		for (int i = 0; i < 3; i++)
		{
			TwoDIntVector currentNeighbour = neighbours[i];
			int neighbourEvaluation = evaluateSolution(currentNeighbour);
			if (neighbourEvaluation <= bestEvaluation) {
				bestEvaluation = neighbourEvaluation;
				theSolution = currentNeighbour;
				makeCurrentEventPlaceMatrix(theSolution);
				cout << "The evaluation of the current solution is: " << bestEvaluation << endl;
			}
		}
	}
	cout << "The evaluation of the current solution is: " << bestEvaluation << endl;
	printMatrix(theSolution, numRooms, NUMBEROFSLOTS);
	// Create the file to be checked by the official solution validator
	outputSlnAnswerFile(theSolution, slnFileName);
    return 0;
}


//---------------------------------------------------------------------------------------
//                             ALGORITHM FUNCTIONS
//---------------------------------------------------------------------------------------

TwoDIntVector createRandomSolution() {
	// PARTIALCOL2012 eller 2008 sier at random er vel så bra som noe greedy.

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
	makeCurrentEventPlaceMatrix(theSolution);
	return theSolution;
}

TwoDIntVector* generateNeighbours(TwoDIntVector solution) {
	TwoDIntVector neighbour1 = generateTranslateEventToFreePosition(solution);
	TwoDIntVector neighbour2 = generateSwapTwoEvents(solution);
	TwoDIntVector neighbour3 = generateMatchingNeighbour(solution);

	TwoDIntVector* neighbours = new TwoDIntVector[3];
	neighbours[0] = neighbour1;
	neighbours[1] = neighbour2;
	neighbours[2] = neighbour3;
	return neighbours;
}

// TrE: translate a random event to a free position of the timetable
TwoDIntVector generateTranslateEventToFreePosition(TwoDIntVector solution) {
	int event = rand() % numEvents;
	int room = rand() % numRooms;
	int timeslot = rand() % NUMBEROFSLOTS;
	bool foundEmptyPosition = false;

	// Iterate through the solution looking for free rooms from a random timeslot in a random room
	while (!foundEmptyPosition) {
		timeslot++;
		if (timeslot >= NUMBEROFSLOTS) {
			timeslot = 0;
			room++;
			if (room >= numRooms) {
				room = 0;
			}
		}
		if (solution[room][timeslot] == -1) {
			foundEmptyPosition = true;
		}
	}
	// Copy the solution
	TwoDIntVector neighbour = solution;

	// Make the move
	int* oldPlace = getEventPlace(event);
	int oldRoom = oldPlace[0];
	int oldTimeSlot = oldPlace[1];

	neighbour[oldRoom][oldTimeSlot] = -1;
	neighbour[room][timeslot] = event;
	return neighbour;
}

// SwE: swaps two events by interchanging their position in the timetable
TwoDIntVector generateSwapTwoEvents(TwoDIntVector solution) {
	bool distinctEvents = false;
	int event1, event2;
	// Generate two random events
	while (!distinctEvents) {
		event1 = rand() % numEvents;
		event2 = rand() % numEvents;
		if (event1 != event2) {
			distinctEvents = true;
		}
	}
	// Get their rooms and timeslots
	int* placeEvent1 = getEventPlace(event1);
	int* placeEvent2 = getEventPlace(event2);
	
	TwoDIntVector neighbour = solution;

	// Swap their positions
	neighbour[placeEvent1[0]][placeEvent1[1]] = event2;
	neighbour[placeEvent2[0]][placeEvent2[1]] = event1;
	return neighbour;
}

// Ma (Matching): reassigns the events within a given timeslot to minimise the number
// of events assigned to an unsuitable room; to allow violations, a maximum
// matching is solved. Events left unassigned in the matching are put into arbitrary rooms.
TwoDIntVector generateMatchingNeighbour(TwoDIntVector solution) {
	int timeslot = rand() % NUMBEROFSLOTS;
	
	//Make an array signifying how many of the events can go in each room
	int* numEvsThatCanGoInRoom = new int[numRooms];

	// And an array signifying how many of the rooms each event can go into
	int* numRoomsThatCanHaveEvs = new int[numRooms];

	for (int room = 0; room<numRooms; room++) {
		int numEventsCount = 0;
		int numRoomCount = 0;

		int eventEvent = solution[room][timeslot];
		for (int i = 0; i < numRooms; i++)
		{
			int roomEvent = solution[i][timeslot];
			if (roomEvent != -1) {
				if (eventRoom[roomEvent][room]) {
					numEventsCount++;
				}
			}
			if (eventEvent != -1) {
				if (eventRoom[eventEvent][i]) {
					numRoomCount++;
				}
			}
		}
		numEvsThatCanGoInRoom[room] = numEventsCount;
		numRoomsThatCanHaveEvs[room] = numRoomCount;
	}

	TwoDIntVector partialSolution = solution;
	int* unplacedEventsInTimeslot = new int[numRooms];
	int* unDesignatedRoomsInTimeslot = new int[numRooms];
	int numberOfUnplacedEventsInTimeslot = 0;
	// Make all rooms in the certain timeslot clear. We are going to give the events in this timeslot new rooms
	for (int room = 0; room < numRooms; room++)
	{
		int event = solution[room][timeslot];
		unplacedEventsInTimeslot[room] = event;
		unDesignatedRoomsInTimeslot[room] = room;
		partialSolution[room][timeslot] = -1;
		if (event != -1) {
			numberOfUnplacedEventsInTimeslot++;
		}
	}

	while (numberOfUnplacedEventsInTimeslot > 0) {
		//We start with the event that has fewest rooms that it it can go into.
		int selectedEvent = selectEventWithFewestSuitableRooms(timeslot, numRoomsThatCanHaveEvs, unplacedEventsInTimeslot);

		// Chose the suited room which has the fewest other events that can go there.
		int selectedRoom = selectRoomWithFewestSuitableEvents(selectedEvent, numEvsThatCanGoInRoom, unDesignatedRoomsInTimeslot);

		// Place the selected event in the selected room
		partialSolution[selectedRoom][timeslot] = selectedEvent;
		// Decrease number of unplaced events
		numberOfUnplacedEventsInTimeslot--;

		int oldEventRoom = getEventPlace(selectedEvent)[0];
		// Update the array with unplaced events
		unplacedEventsInTimeslot[oldEventRoom] = -1;
		unDesignatedRoomsInTimeslot[selectedRoom] = -1;
	}
	
	return partialSolution;
}

int selectEventWithFewestSuitableRooms(int timeslot, int* numRoomsThatCanHaveEvs, int* unplacedEventsInTimeslot) {
	
	int minValue = numRooms + 1;
	IntVector minList;
	for (int i = 0; i < numRooms; i++)
	{
		int event = unplacedEventsInTimeslot[i];
		if (event != -1) {
			int numberOfRooms = numRoomsThatCanHaveEvs[i];
			// If it has the fewest rooms who works
			if (numberOfRooms < minValue) {
				minList.clear();
				minList.push_back(event);
				minValue = numberOfRooms;
			}
			// If it has as many as the current fewest rooms
			else if (numberOfRooms == minValue) {
				minList.push_back(event);
			}
		}
	}
	int r = rand() % minList.size();
	return minList[r];
}

int selectRoomWithFewestSuitableEvents(int selectedEvent, int* numEvsThatCanGoInRoom, int* unDesignatedRoomsInTimeslot) {
	// Get a list of the rooms that suit this events needs
	bool* suitedRooms = eventRoom[selectedEvent];

	int minValue = numRooms + 1;
	IntVector minList;
	for (int room = 0; room < numRooms; room++)
	{
		// If this room is suited and that it hasn't already been assigned
		if (suitedRooms[room] && unDesignatedRoomsInTimeslot[room] != -1) {
			int numberOfEvents = numEvsThatCanGoInRoom[room];
			// If it has the fewest events who works
			if (numberOfEvents < minValue) {
				minList.clear();
				minList.push_back(room);
				minValue = numberOfEvents;
			}
			// If it has as many as the current fewest events
			else if (numberOfEvents == minValue) {
				minList.push_back(room);
			}
		}
	}
	// If there are no suitable, untaken rooms. Put event in a random room. I.e the first available one.
	if (minList.size() == 0) {
		for (int room = 0; room < numRooms; room++)
		{
			if (unDesignatedRoomsInTimeslot[room] != -1) {
				return room;
			}
		}
	}
	else {
		int r = rand() % minList.size();
			return minList[r];
	}

	
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
				/*
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
				*/
				

				// HARD CONSTRAINT 2) the room is big enough for all the attending students and satisfies all the features required by the event;
				//Check if room satisfies the event feature
				if (eventRoom[event][room] == false) {
					// If a room has a required feature and the room doesn't have that feature it is unsuited
					// If a room is too small for the size (number of people attending) of an event
					// The eventRoom matrix has stored both of this information. 
					totalCost++;
				}
				/*
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
				*/
				
			}
		}
	}
	return totalCost;
}

//---------------------------------------------------------------------------------------
//                             HELP FUNCTIONS
//---------------------------------------------------------------------------------------

// Makes a matrix with the room and timeslot of each event.
// Should be ran everytime changes are made to the solution
void makeCurrentEventPlaceMatrix(TwoDIntVector solution) {
	int r, c, i;
	currentEventPlace = new int*[numEvents];
	for (i = 0; i <numEvents; i++)currentEventPlace[i] = new int[2];
	for (r = 0; r < numRooms; r++) {
		for (c = 0; c < NUMBEROFSLOTS; c++) {
			int event = solution[r][c];
			if (event != -1) {
				currentEventPlace[event][0] = r;
				currentEventPlace[event][1] = c;
			}
		}
	}
}

// Returns room[0] and timeslot[1]
int* getEventPlace(int event) {
	return currentEventPlace[event];
}

// Help function to visualize arrays
void printArray(int* array, int size) {
	for (int i = 0; i < size; i++)
	{
		cout << array[i] << endl;
	}
}
// Help function to visualize arrays
void printArray(bool* array, int size) {
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