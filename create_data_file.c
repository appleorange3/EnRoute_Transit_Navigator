#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Time {
    int hr, min, sec;
} Time;

Time difference(Time start, Time end) {
    Time diff;

    // Start with seconds and handle borrowing
    diff.sec = end.sec - start.sec;
    if (diff.sec < 0) {
        diff.sec += 60;
        end.min -= 1;
    }

    // Move to minutes and handle borrowing
    diff.min = end.min - start.min;
    if (diff.min < 0) {
        diff.min += 60;
        end.hr -= 1;
    }

    // Calculate hours
    diff.hr = end.hr - start.hr;
    if (diff.hr < 0) {
        diff.hr += 24;  // Handle cases crossing midnight
    }

    return diff;
}

int main() {
    FILE *inputFile, *outputFile;
    Time time, prev_time, time_diff;

    // Allocate memory for strings
    char route_id[20], prev_id[20] = " "; // Sufficient size for route_id
    char stop_id[10]; // Sufficient size for stop_id
    char stop_seq[4]; // Sufficient size for stop_sequence
    char arrival_time[9]; // Sufficient size for HH:MM:SS
    char line[1024];
    int stop, prev;

    inputFile = fopen("./data/bus/stop_times.txt", "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    outputFile = fopen("./data/bus/new_stop_times.txt", "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        fclose(inputFile);
        return EXIT_FAILURE;
    }

    fgets(line, sizeof(line), inputFile); // Skip the header line
    while (fgets(line, sizeof(line), inputFile)) {
        // Read the data from the line
        sscanf(line, "%[^_]%*[^,],%[^,],%*[^,],%[^,],%s", route_id, arrival_time, stop_id, stop_seq);

        // Parse the arrival time into hr, min, sec
        sscanf(arrival_time, "%d:%d:%d", &time.hr, &time.min, &time.sec);
        sscanf(stop_seq, "%d", &stop);

        if(strcmp(prev_id, route_id) != 0) {
                prev_time.hr = time.hr;
                prev_time.min = time.min;
                prev_time.sec = time.sec;
        }

        time_diff = difference(prev_time, time);

        // Write the desired output to the output file
                if(strcmp(prev_id, route_id) != 0 || (strcmp(prev_id, route_id) == 0 && stop > prev))
                fprintf(outputFile, "%s,%02d:%02d:%02d,%s,%d\n", route_id, time_diff.hr, time_diff.min, time_diff.sec, stop_id, stop);

                if(stop > prev || strcmp(prev_id, route_id) != 0)
                prev = stop;

        strcpy(prev_id, route_id);
        prev_time = time;
    }

    // Close the files
    fclose(inputFile);
    fclose(outputFile);

    printf("Data has been written to output.txt.\n");
    return EXIT_SUCCESS;
}
