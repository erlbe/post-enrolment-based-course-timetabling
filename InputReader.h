#ifndef INPUT_READER
#define INPUT_READER

#include "ITC2007Solution.h"


int* makeRoomSizeArray(ifstream& i_stream);
bool** makeAttendsMatrix(ifstream& inStream);
bool** makeRoomFeaturesMatrix(ifstream& inStream);
bool** makeEventFeaturesMatrix(ifstream& inStream);
bool** makeEventAvail(ifstream& inStream);
bool** makeRoomAvail();
int** makeBeforeMatrix(ifstream& inStream);
bool** makeEventConflictMatrix();
int* makeEventSizeMatrix();

#endif