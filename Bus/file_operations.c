#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_operations.h"
#include "utilities.h"

void load_stops(const char* filename, Graph* graph) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open stops file");
        exit(EXIT_FAILURE);
    }

    char line[110];
    fgets(line, sizeof(line), file); // Read header line

    int i = 0;
    int line_number = 1; // Start counting from the first data line

    while (fgets(line, sizeof(line), file) && i < graph->num_stops) {
        line_number++; // Increment line number for each line read
        int stop_id;
        float stop_lat, stop_lon;
        char stop_name[50];

        // Parsing the line
        if (sscanf(line, "%*[^,],%d,%f,%f,%49[^,]", &stop_id, &stop_lat, &stop_lon, stop_name) < 4) {
            fprintf(stderr, "Error parsing stop data on line %d: %s\n", line_number, line);
            continue; // Skip to the next line
        }

        // Store the stop information in the graph
        graph->stops[i].stop_id = stop_id;
        graph->stops[i].stop_lat = stop_lat;
        graph->stops[i].stop_lon = stop_lon;
        strcpy(graph->stops[i].stop_name, stop_name);

        graph->stop_id_map[stop_id] = i; 
        i++;
    }
    fclose(file);
}

void load_stop_times(const char* filename, Graph* graph) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open stop times file");
        exit(EXIT_FAILURE);
    }

    char line[100], prev_time[10], curr_time[10],bus_id[10];
    int prev_stop_id = -1, curr_stop_id, curr_seq_no;

    fgets(line, sizeof(line), file); // Ignore header

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        int parsedItems = sscanf(line, "%[^_]_%*[^,],%[^,],%*[^,],%d,%d", bus_id, curr_time, &curr_stop_id, &curr_seq_no);

        if (parsedItems < 4) {
            fprintf(stderr, "Error parsing stop time data: %s\n", line);
            continue;
        }
if (curr_seq_no){
        Time travel_time = calculate_time_diff(prev_time, curr_time);
                add_edge(graph, prev_stop_id, curr_stop_id, travel_time, -1,atoi(bus_id)); }

        

        prev_stop_id = curr_stop_id;
        strcpy(prev_time, curr_time);
    }

    fclose(file);
}
Route* readRoutesFromFile(const char *filename, int *routeCount) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    Route *routes = NULL;
    *routeCount = 0;
    int currentRouteId = -1;
    int routeId, stopId, stopSeq;
    char timeStr[9], line[128];
	
	fgets(line, sizeof(line), file);
	
    while (fscanf(file, "%d,%8[^,],%d,%d", &routeId, timeStr, &stopId, &stopSeq) == 4) {
        Time time = parseTime(timeStr);
        
        if (routeId != currentRouteId) {
            currentRouteId = routeId;
            (*routeCount)++;
            routes = realloc(routes, sizeof(Route) * (*routeCount));
            routes[*routeCount - 1].routeId = routeId;
            routes[*routeCount - 1].stops = NULL;
            routes[*routeCount - 1].times = NULL;
            routes[*routeCount - 1].stopCount = 0;
        }

        addStopToRoute(&routes[*routeCount - 1], stopId, time);
    }
   
    fclose(file);
    return routes;
}


void load_fares(const char* filename, Graph* graph) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open fares file");
        exit(EXIT_FAILURE);
    }

    char line[100];
    fgets(line, sizeof(line), file); 

    while (fgets(line, sizeof(line), file)) {
        char stop1[10], stop2[10];
        float price;
        
        if (sscanf(line, "%*[^_]_%*[^_]_%[^_]_%[^,],%f", stop1, stop2, &price) < 3) {
            fprintf(stderr, "Error parsing fare data: %s\n", line);
            continue;
        }

        int from_stop = graph->stop_id_map[atoi(stop1)];     
        int to_stop = graph->stop_id_map[atoi(stop2)];
        int to_id=atoi(stop2);

        if (from_stop < 0 || from_stop >= graph->num_stops || to_stop < 0 || to_stop >= graph->num_stops) {
            fprintf(stderr, "Warning: Invalid stop ID in fares file (%d -> %d)\n", from_stop, to_stop);
            continue;
        }

        AdjStop* adj = graph->adjLists[from_stop].head;
                
        int index = 0;
        while (adj) {
        
            if (adj->stop_id == to_id) {
                if (index < graph->adjLists[from_stop].size) {
                    graph->adjLists[from_stop].fares[index] = price;
                } else {
                    fprintf(stderr, "Warning: Index out of bounds for fares array\n");
                }
                break;
            }
            adj = adj->next;
            index++;
        }
    }

    fclose(file);
}

