#ifndef UTILITIES_H
#define UTILITIES_H

#include "graph.h"

Time calculate_time_diff(char* time1, char* time2);
void debug_memory(void* ptr, const char* msg);
void add_bus_id(AdjStop* adj_stop, int bus_id);
Time parseTime(const char *timeStr);

#endif

