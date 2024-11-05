#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include "graph.h"

void load_stops(const char* filename, Graph* graph);
void load_stop_times(const char* filename, Graph* graph);
Route* readRoutesFromFile(const char* filename, int *routeCount);
void load_fares(const char* filename, Graph* graph);

#endif

