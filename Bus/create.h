#pragma once
#include "struct.h"

// Functions for creating various structures
Stop* createStop(int stopId, float lat, float lon, const char *stopName);
Graph* createGraph(int numStops);
void addStopToAdjList(Graph* graph, int src, int stopId, const char* stopName, float lat, float lon);
Route* createRoute(int routeId, int numStops);
MinHeapNode* createMinHeapNode(int stopId, Time time);
MinHeap* createMinHeap(int capacity);

