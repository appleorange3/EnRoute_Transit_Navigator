#pragma once
#include "struct.h"
#include "create.h"

// Functions for reading from files
void appendStop(Stop **head, int stopId, float lat, float lon, const char *stopName);
Stop* readStopsFromFile(const char* filename);
void printStops(Stop* head);
Time parseTime(const char *timeStr);
void addStopToRoute(Route *route, int stopId, Time time);
Route* readRoutesFromFile(const char* filename, int *routeCount);
void printRoutes(Route *routes, int routeCount);

