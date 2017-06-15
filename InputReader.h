#ifndef INPUT_READER
#define INPUT_READER

#include "ITC2007Solution.h"

void readInputFile(ifstream& inStream);
int* makeRoomSizeArray(ifstream& i_stream);
bool** makeAttendsMatrix(ifstream& inStream);
bool** makeRoomFeaturesMatrix(ifstream& inStream);
bool** makeEventFeaturesMatrix(ifstream& inStream);
bool** makeEventAvail(ifstream& inStream);
bool** makeRoomAvail();
int** makeBeforeMatrix(ifstream& inStream);
bool** makeEventConflictMatrix();
int* makeTotalNumConflictArray();
int* makeEventSizeMatrix();
bool** makeEventRoomMatrix();
int* makeNumSuitableRoomsArray();
int* makeNumSuitableEventsArray();
int* makeTotalNumAvailableSlotsArray();

#endif