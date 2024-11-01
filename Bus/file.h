#pragma once
#include "struct.h"
#include "create.h"

// Functions for reading from files
Stop* readStopsFromFile(const char* filename);
void printStops(Stop* head);
Route* readRoutesFromFile(const char* filename, int *routeCount);

