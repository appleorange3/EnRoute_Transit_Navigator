#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.h"

// Function to calculate the time difference between two times
Time calculate_time_diff(const char* start_time, const char* end_time) {
    Time start = parseTime(start_time);
    Time end = parseTime(end_time);

    Time diff;
    // Calculate difference in seconds
    int start_seconds = start.hr * 3600 + start.min * 60 + start.sec;
    int end_seconds = end.hr * 3600 + end.min * 60 + end.sec;
    int diff_seconds = end_seconds - start_seconds;

    // Convert the difference back to hours, minutes, and seconds
    diff.hr = diff_seconds / 3600;
    diff.min = (diff_seconds % 3600) / 60;
    diff.sec = diff_seconds % 60;

    return diff;
}

// Function to parse a time string (HH:MM:SS) into a Time structure
Time parseTime(const char* time_str) {
    Time time = {0};
    sscanf(time_str, "%d:%d:%d", &time.hr, &time.min, &time.sec);
    return time;
}
