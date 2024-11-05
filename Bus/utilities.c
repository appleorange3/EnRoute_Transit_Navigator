#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.h"

Time calculate_time_diff(char* time1, char* time2) {
    int h1, m1, s1, h2, m2, s2;
    sscanf(time1, "%d:%d:%d", &h1, &m1, &s1);
    sscanf(time2, "%d:%d:%d", &h2, &m2, &s2);

    int total_seconds1 = h1 * 3600 + m1 * 60 + s1;
    int total_seconds2 = h2 * 3600 + m2 * 60 + s2;
    int diff = abs(total_seconds2 - total_seconds1);

    Time time_diff;
    time_diff.hr = diff / 3600;
    time_diff.min = (diff % 3600) / 60;
    time_diff.sec = diff % 60;

    return time_diff;
  }

void debug_memory(void* ptr, const char* msg) {
    if (!ptr) {
        fprintf(stderr, "Memory Error: %s\n", msg);
        exit(EXIT_FAILURE);
    }
}

void add_bus_id(AdjStop* adj_stop, int bus_id) {
    AdjStop *temp=adj_stop;
    int bus_id_checker=1;
    for(int i=0;i<adj_stop->bus_count;i++){
    if((bus_id)==temp->bus_ids[i]){
    bus_id_checker=0;
    break;
    }
    }
if(bus_id_checker){
    adj_stop->bus_ids = realloc(adj_stop->bus_ids, (adj_stop->bus_count + 1) * sizeof(int));
    debug_memory(adj_stop->bus_ids, "Failed to reallocate memory for bus_ids");

    adj_stop->bus_ids[adj_stop->bus_count] = bus_id;
    adj_stop->bus_count++;

}
}

Time parseTime(const char *timeStr) {
    Time t;
    sscanf(timeStr, "%d:%d:%d", &t.hr, &t.min, &t.sec);
    return t;
}

