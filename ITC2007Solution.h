#ifndef ITC2007SOLUTION_H
#define ITC2007SOLUTION_H

#include "stdafx.h"
#include "string.h"
#include <iostream>  
#include <fstream>
#include <vector>
#include <random>
#include <ctime>
#pragma once

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
void outputSlnAnswerFile(TwoDIntVector&, char*);
int evaluateSolution(TwoDIntVector);
TwoDIntVector* generateNeighbours(TwoDIntVector);
void makeCurrentEventPlaceMatrix(TwoDIntVector);
int* getEventPlace(int event);

TwoDIntVector createRandomSolution();

void printArray(int* array, int size);
void printMatrix(int** matrix, int height, int width);
void printMatrix(bool** matrix, int height, int width);
void printMatrix(TwoDIntVector matrix, int rows, int cols);


//	Arrays and variables used throughout the program
extern int numEvents, numRooms, numFeatures, numStudents, NUMBEROFPLACES;
extern int *roomSize, *eventSize;
extern int **before;
extern bool **attends, **roomFeatures, **eventFeatures, **eventAvail, **roomAvail, **event_conflict;

#include "InputReader.h"

#endif