#include "ITC2007Solution.h"

//	Arrays and variables used throughout the program
int numEvents, numRooms, numFeatures, numStudents, NUMBEROFPLACES;
int *roomSize, *eventSize, *numSuitableRooms, *numSuitableEvents, *totalNumConflict, *totalNumAvailableSlots;
int **before, **currentEventPlace;
bool **attends, **roomFeatures, **eventFeatures, **eventAvail, **roomAvail, **event_conflict, **eventRoom;
IntVector unplacedEvents, tabuList;

int main(int argc, char**argv)
{
	char fileName[40], slnFileName[40];
	int i;
	clock_t timeLimit = 15;
	ifstream inStream;

	// Read the input and arguments
	for (i = 1; i < argc; i++)
	{
		if (strcmp("-t", argv[i]) == 0) {
			timeLimit = atoi(argv[++i]);
		}
		else {
			strcpy(fileName, argv[i]);
			strcat(fileName, ".tim");
			strcpy(slnFileName, argv[i]);
			inStream.open(fileName);
			readInputFile(inStream);
			inStream.close();
		}
		
	}

	// Seed the random function
	//srand(time(NULL));
	srand(time(NULL));

	clock_t clockStart = clock();
	clock_t clockFinish = clockStart + int(timeLimit*CLOCKS_PER_SEC);
	TwoDIntVector finalSolution = localSearch(clockFinish);
	double duration = (double)(clock() - clockStart) / CLOCKS_PER_SEC;

	cout << "The final solution looks like this:" << endl;
	printMatrix(finalSolution, numRooms, NUMBEROFSLOTS);
	cout << "The evaluation of the final solution is: " << evaluateSolution(finalSolution) << endl;

	if (duration < timeLimit) {
		cout << "Program ended. " << duration << " seconds to of CPU time to complete\n";
	}
	else {
		cout << "Program ended. " << timeLimit << " seconds to of CPU time to complete\n";
	}

	// Create the file to be checked by the official solution validator
	outputSlnAnswerFile(finalSolution, slnFileName);
    return 0;
}


//---------------------------------------------------------------------------------------
//                             ALGORITHM FUNCTIONS
//---------------------------------------------------------------------------------------

TwoDIntVector localSearch(clock_t clockFinish) {
	// The solution will be a 2D vector with the timetable for each room
	TwoDIntVector theSolution;
	// theSolution = createRandomSolution();
	theSolution = generateFirstSolution();
	int bestEvaluation = evaluateSolution(theSolution);
	cout << "The evaluation of the initial candidate solution is: " << bestEvaluation << endl;
	while (clockFinish > clock() && bestEvaluation > 0) {
		TwoDIntVector* neighbours = generateNeighbours(theSolution);
		for (int i = 0; i < 5 + numRooms; i++)
		{
			TwoDIntVector currentNeighbour = neighbours[i];
			int neighbourEvaluation = evaluateSolution(currentNeighbour);
			if (neighbourEvaluation < bestEvaluation) {
				bestEvaluation = neighbourEvaluation;
				theSolution = currentNeighbour;
				makeCurrentEventPlaceMatrix(theSolution);
				//cout << "The evaluation of the current solution is: " << bestEvaluation << endl;

				// Add the room to the tabu-list
				if (i >= 5) {
					tabuList.push_back(i - 5);
					// If tabulist too big, remove one
					if (tabuList.size() > MAXTABULISTSIZE) {
						tabuList.erase(tabuList.begin());
					}
				}
			}
		}
	}
	return theSolution;
}

// THIS METHOD IS NOT USED. TOO BAD...
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

TwoDIntVector generateFirstSolution() {
	//Make an empty the timetable
	TwoDIntVector theSolution;
	vector<vector<int> > temp(numRooms, vector<int>(NUMBEROFSLOTS, -1));
	theSolution.swap(temp);

	// Make copy of the numSuitableRooms which we can use in the generation of the first solution
	int* numRoomsForEvent = new int[numEvents];
	for (int r = 0; r<numEvents; r++) numRoomsForEvent[r] = numSuitableRooms[r];

	// Make copy of the numSuitableEvents which we can use in the generation of the first solution
	// FIXME: Needed?
	int* numEventsForRoom = new int[numRooms];
	for (int r = 0; r < numRooms; r++) numEventsForRoom[r] = numSuitableEvents[r];

	// Create a matrix with a vector of available timeslots in each room. At the beginning all are available.
	TwoDIntVector availableTimeslots;
	vector<vector<int> > tempMatrix(numRooms, vector<int>(NUMBEROFSLOTS, -1));
	availableTimeslots.swap(tempMatrix);
	for (int room = 0; room < numRooms; room++) {
		for (int timeslot = 0; timeslot < NUMBEROFSLOTS; timeslot++){
			availableTimeslots[room][timeslot] = timeslot;
		}
	}

	int numEventsToBePlaced = numEvents;

	while (numEventsToBePlaced > 0) {
		int event = selectEvent(numRoomsForEvent);
		int room = selectRoom(event, numEventsForRoom, availableTimeslots);
		if (room != -1) {
			// Select a random timeslot from one of the available ones for this room.
			int index = rand() % availableTimeslots[room].size();
			int timeslot = availableTimeslots[room][index];

			// Remove the timeslot from the array of available timeslots for this room
			availableTimeslots[room].erase(availableTimeslots[room].begin() + index);

			theSolution[room][timeslot] = event;
		}
		else {
			cout << event << "Was left unplaced because of finding no fitting room" << endl;
			unplacedEvents.push_back(event);
		}
		numEventsToBePlaced--;
	}
	
	makeCurrentEventPlaceMatrix(theSolution);
	return theSolution;
}

int selectEvent(int* numRoomsForEvent) {
	// Select one of the events that has fewest rooms that it can go into. If there are multiple, select one at random.

	int minimum = INT_MAX;
	int i;
	IntVector min_vals;
	for (i = 0; i<numEvents; i++) {
		if (numRoomsForEvent[i] != -1 && numRoomsForEvent[i] != 0) {
			if (numRoomsForEvent[i] < minimum) {
				min_vals.clear();
				min_vals.push_back(i);
				minimum = numRoomsForEvent[i];
			}
			else if (numRoomsForEvent[i] == minimum) {
				min_vals.push_back(i);
			}
			else; //do nothing
		}
	}
	int r = rand() % min_vals.size();
	int event = min_vals[r];

	// Update the numRoomsForEvent array
	numRoomsForEvent[event] = -1;

	return event;

}

int selectRoom(int event, int* numEventsForRoom, TwoDIntVector availableTimeslots) {
	int minimumq1 = INT_MAX;
	int c, q1;
	IntVector min_vals;
	IntVector suitedRooms;
	for (c = 0; c<numRooms; c++) {
		if (eventRoom[event][c]) {
			suitedRooms.push_back(c);
			//work out how many other events could go in this room (minus 1 because we are considering other events
			q1 = numEventsForRoom[c] - 1;
			if (q1<0) {
				cout << "Error in select_place " << endl;
				exit(1);
			}

			//IF THIS VALUE IS THE LOWEST SO FAR, OR EQUAL TO THE LOWEST, WE STICK IT IN A LIST.
			if (q1 < minimumq1) {
				min_vals.clear();
				min_vals.push_back(c);
				minimumq1 = q1;
			}
			else if (q1 == minimumq1) {
				min_vals.push_back(c);
			}
			else; //do nothing

		}//end of if
	}//end of for loop
	 //We now have at least one event in a list called min_vals. If there are more than
	 //one in the list then we need to choose one. We do this randomly
	while (suitedRooms.size() > 0) {
		int r, room;
		if (min_vals.size() > 0) {
			r = rand() % min_vals.size();
			room = min_vals[r];
		}
		else {
			r = rand() % suitedRooms.size();
			room = suitedRooms[r];
		}
		int numberOfAvailableSlotsInRoom = availableTimeslots[room].size();
		if (numberOfAvailableSlotsInRoom > 0) {
			return room;
		}
		else {
			// No available slots in this room. Remove it from suitables and min_vals and try another.
			if (min_vals.size() > 0) {
				min_vals.erase(std::remove(min_vals.begin(), min_vals.end(), room), min_vals.end());
			}
			suitedRooms.erase(std::remove(suitedRooms.begin(), suitedRooms.end(), room), suitedRooms.end());
		}
	}
	// If we reach this point none of the suited rooms are available. This means the event is left unplaced
	return -1;
	
}

TwoDIntVector* generateNeighbours(TwoDIntVector solution) {
	TwoDIntVector neighbour1 = generateTranslateEventToFreePosition(solution, true);
	TwoDIntVector neighbour2 = generateTranslateEventToFreePosition(solution, false);
	TwoDIntVector neighbour3 = generateSwapTwoEvents(solution, true);
	TwoDIntVector neighbour4 = generateSwapTwoEvents(solution, false);
	TwoDIntVector neighbour5 = generateMatchingNeighbour(solution);

	TwoDIntVector* neighbours = new TwoDIntVector[5+numRooms];
	neighbours[0] = neighbour1;
	neighbours[1] = neighbour2;
	neighbours[2] = neighbour3;
	neighbours[3] = neighbour4;
	neighbours[4] = neighbour5;
	for (int i = 0; i < numRooms; i++)
	{
		neighbours[i+5] = generateTimeslotMatchingNeighbour(solution, i);
	}
	return neighbours;
}

// TrE: translate a random event to a free position of the timetable
// Random if we want to swap random rooms.
TwoDIntVector generateTranslateEventToFreePosition(TwoDIntVector solution, bool random) {
	int event = rand() % numEvents;
	int room;
	if (random) {
		room = rand() % numRooms;
	} else {
		int* place = getEventPlace(event);
		room = place[0];
	}
	
	int timeslot = rand() % NUMBEROFSLOTS;
	int firstTimeslot = timeslot;
	bool foundEmptyPosition = false;

	// Iterate through the solution looking for free rooms from a random timeslot in a random room
	while (!foundEmptyPosition) {
		timeslot++;
		if (timeslot >= NUMBEROFSLOTS) {
			timeslot = 0;
			if (random) {
				room++;
				if (room >= numRooms) {
					room = 0;
				} 
			}
		}
		if (!random && timeslot == firstTimeslot) {
			// We must have looped, and not found any free timeslots in this room. 
			return solution;
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
// Random if we want to swap rooms as well.
TwoDIntVector generateSwapTwoEvents(TwoDIntVector solution, bool random) {
	bool distinctEvents = false;
	int event1, event2;
	// Generate two random events
	while (!distinctEvents) {
		event1 = rand() % numEvents;
		if (random) {
			event2 = rand() % numEvents;
		}
		else {
			int room = getEventPlace(event1)[0];
			bool foundEvent = false;
			while (!foundEvent) {
				int timeslot = rand() % NUMBEROFSLOTS;
				event2 = solution[room][timeslot];
				if (event2 != -1) {
					foundEvent = true;
				}
			}
			
		}
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

// TMa: Timeslot Matching. Try to re-arrange the timeslots in a given room better.
// TODO: Handle before/after also.
TwoDIntVector generateTimeslotMatchingNeighbour(TwoDIntVector solution, int room) {
	IntVector eventsToBeReScheduled;
	// Copy the solution to be tinkered with.
	TwoDIntVector neighbourSolution = solution;

	// Go through the timeslots in the room and clear the solution, and append all the events to the list of events to be scheduled.
	for (int timeslot = 0; timeslot < NUMBEROFSLOTS; timeslot++){
		int event = neighbourSolution[room][timeslot];
		if (event != -1) {
			// Add event to the ones to be scheduled.
			eventsToBeReScheduled.push_back(event);
			// Make timeslot available
			neighbourSolution[room][timeslot] = -1;
		}
	}
	while (eventsToBeReScheduled.size() > 0) {
		// Choose the event with fewest available timeslots
		IntVector events = chooseEventsWithFewestAvailable(eventsToBeReScheduled);

		// Choose the event with most conflicts
		int event = chooseEventWithMostConflicts(events);
		// Delete the event from the list of events to be scheduled
		eventsToBeReScheduled.erase(std::remove(eventsToBeReScheduled.begin(), eventsToBeReScheduled.end(), event), eventsToBeReScheduled.end());

		// Get the suited timeslots
		IntVector suitedTimeslots;
		for (int timeslot = 0; timeslot < NUMBEROFSLOTS; timeslot++) {
			if (eventAvail[event][timeslot]) {
				suitedTimeslots.push_back(timeslot);
			}
		}
		bool foundTimeslot = false;
		IntVector suitedTimeSlotsCopy = suitedTimeslots;
		while (suitedTimeSlotsCopy.size() > 0 && !foundTimeslot) {
			// Select a timeslot
			int r = rand() % suitedTimeSlotsCopy.size();
			int timeslot = suitedTimeSlotsCopy[r];

			// Check if there is a conflict with the other events in this timeslot in the other rooms.
			bool conflict = false;
			for (int room = 0; room < numRooms; room++) {
				int otherEvent = neighbourSolution[room][timeslot];
				if (otherEvent != -1) {
					if (event_conflict[event][otherEvent]) {
						cout << "Conflict between event " << event << " and " << otherEvent << endl;
						conflict = true;
					}
				}
			}
			if (!conflict && neighbourSolution[room][timeslot] == -1) {
				// If no conflict, place the event in the timeslot.
				neighbourSolution[room][timeslot] = event;
				foundTimeslot = true;
			}
			else {
				// If conflict or taken, remove the timeslot from the ones we consider
				suitedTimeSlotsCopy.erase(std::remove(suitedTimeSlotsCopy.begin(), suitedTimeSlotsCopy.end(), timeslot), suitedTimeSlotsCopy.end());
			}
		}
		// If all of the suited timeslots are occupied by other events this event is in conflict with
		if (suitedTimeSlotsCopy.size() == 0) {
			// Choose one of the suitable timeslots at random, and hope the conflict will be resolved later.
			while (suitedTimeslots.size() > 0 && !foundTimeslot) {
				int r = rand() % suitedTimeslots.size();
				int timeslot = suitedTimeslots[r];
				if (neighbourSolution[room][timeslot] == -1) {
					neighbourSolution[room][timeslot] = event;
					foundTimeslot = true;
				}
				else {
					suitedTimeslots.erase(std::remove(suitedTimeslots.begin(), suitedTimeslots.end(), timeslot), suitedTimeslots.end());
				}
			}
			// If all of the suited timeslots are taken, just place the event in a random one that isn't taken.
			if (suitedTimeslots.size() == 0) {
				while (!foundTimeslot) {
					int timeslot = rand() % NUMBEROFSLOTS;
					if (neighbourSolution[room][timeslot] == -1) {
						neighbourSolution[room][timeslot] = event;
						foundTimeslot = true;
					}
				}
			}
			if (!foundTimeslot) {
				cout << "Didn't find timeslot for event " << event << endl;
		}
		}
	}
	return neighbourSolution;
}
IntVector chooseEventsWithFewestAvailable(IntVector events) {
	int minimum = NUMBEROFSLOTS + 1;
	IntVector min_vals;
	for (int i = 0; i < events.size(); i++)	{
		int event = events[i];
		if (totalNumAvailableSlots[event] < minimum) {
			min_vals.clear();
			min_vals.push_back(event);
			minimum = totalNumAvailableSlots[event];
		}
		else if (totalNumAvailableSlots[event] == minimum){
			min_vals.push_back(event);
		}
	}
	return min_vals;
}

int chooseEventWithMostConflicts(IntVector events) {
	int maxValue = -1;
	IntVector maxList;
	// Select the event(s) with most conflicts
	for (int i = 0; i < events.size(); i++) {
		int event = events[i];
		if (totalNumConflict[event] > maxValue) {
			maxList.clear();
			maxList.push_back(event);
			maxValue = totalNumConflict[event];
		}
		else if (totalNumConflict[event] == maxValue) {
			maxList.push_back(event);
		}
	}
	int r = rand() % maxList.size();
	int event = maxList[r];
	return event;
}


int evaluateSolution(TwoDIntVector solution) {
	int totalCost = 0;
	int constraint1 = 0, constraint2 = 0, constraint3 = 0, constraint4 = 0, constraint5 = 0;

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
							constraint1++;
						}
					}
				}
				

				// HARD CONSTRAINT 2) the room is big enough for all the attending students and satisfies all the features required by the event;
				//Check if room satisfies the event feature
				if (eventRoom[event][room] == false) {
					// If a room has a required feature and the room doesn't have that feature it is unsuited
					// If a room is too small for the size (number of people attending) of an event
					// The eventRoom matrix has stored both of this information. 
					totalCost++;
					constraint2++;
				}
				// HARD CONSTRAINT 3) only one event is put into each room in any timeslot;
				// This one is impossible to violate given how the solution is represented

				// HARD CONSTRAINT 4) events are only assigned to timeslots that are pre-defined as available for those events;
				if (!eventAvail[event][timeslot]) {
					totalCost++;
					constraint4++;
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
							constraint5++;
						}
					}
				}
				
				
			}
		}
	}
	/*
	cout << "Hard constraint 1 has the following cost: " << constraint1 << endl;
	cout << "Hard constraint 2 has the following cost: " << constraint2 << endl;
	cout << "Hard constraint 3 has the following cost: " << constraint3 << endl;
	cout << "Hard constraint 4 has the following cost: " << constraint4 << endl;
	cout << "Hard constraint 5 has the following cost: " << constraint5 << endl << endl;
	*/
	

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