#ifndef ITC2007SOLUTION_H
#define ITC2007SOLUTION_H

//#include "stdafx.h"
#include "string.h"
#include <iostream>  
#include <fstream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <time.h>
#pragma once

using namespace std;

//ALGORITHM CONSTANTS
const int NUMBEROFSLOTS = 45;
const int NUMDAYS = 5;
const int SLOTSPERDAY = 9;
const int LASTSLOTINDEX = SLOTSPERDAY - 1;
const int MAXTABULISTSIZE = 3;

// Definitions to use in the algorithm
typedef std::vector<int> IntVector;
typedef std::vector<IntVector> TwoDIntVector;

//Forward declaration
void outputSlnAnswerFile(TwoDIntVector&, char*);
int evaluateSolution(TwoDIntVector);
TwoDIntVector* generateNeighbours(TwoDIntVector);
TwoDIntVector generateTranslateEventToFreePosition(TwoDIntVector, bool);
TwoDIntVector generateSwapTwoEvents(TwoDIntVector, bool);
TwoDIntVector generateMatchingNeighbour(TwoDIntVector);
TwoDIntVector generateTimeslotMatchingNeighbour(TwoDIntVector solution, int room);
IntVector chooseEventsWithFewestAvailable(IntVector events);
int chooseEventWithMostConflicts(IntVector events);
int selectEventWithFewestSuitableRooms(int timeslot, int* numRoomsThatCanHaveEvs, int*);
int selectRoomWithFewestSuitableEvents(int selectedEvent, int* numEvsThatCanGoInRoom, int*);
void makeCurrentEventPlaceMatrix(TwoDIntVector);
int* getEventPlace(int event);

TwoDIntVector localSearch(clock_t);
TwoDIntVector createRandomSolution();
TwoDIntVector generateFirstSolution();
int selectEvent(int* numRoomsForEvent);
int selectRoom(int event, int* numEventsForRoom, TwoDIntVector availableTimeslots);

void printArray(int* array, int size);
void printArray(bool* array, int size);
void printMatrix(int** matrix, int height, int width);
void printMatrix(bool** matrix, int height, int width);
void printMatrix(TwoDIntVector matrix, int rows, int cols);


//	Arrays and variables used throughout the program
extern int numEvents, numRooms, numFeatures, numStudents, NUMBEROFPLACES;
extern int *roomSize, *eventSize, *numSuitableRooms, *numSuitableEvents, *totalNumConflict, *totalNumAvailableSlots;
extern int **before, **currentEventPlace;
extern bool **attends, **roomFeatures, **eventFeatures, **eventAvail, **roomAvail, **event_conflict, **eventRoom;

#include "InputReader.h"

#endif