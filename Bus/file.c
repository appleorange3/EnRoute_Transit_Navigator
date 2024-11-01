#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "create.c"

// Function to append a Stop to the linked list
void appendStop(Stop **head, int stopId, float lat, float lon, const char *stopName) {
    Stop *newStop = createStop(stopId, lat, lon, stopName);

    if (*head == NULL) {
        *head = newStop;
    } else {
        Stop *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newStop;
    }
}

// Function to read stops from the CSV file
Stop* readStopsFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Unable to open file for reading.\n");
        return NULL;
    }

    Stop *head = NULL;  
    char line[256];     
    int stopId;
    float lat, lon;
    char stopName[64];

    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%*[^,],%d,%f,%f,%63[^,],%*d", &stopId, &lat, &lon, stopName) == 4) {
            appendStop(&head, stopId, lat, lon, stopName);  
        }
    }

    fclose(file);
    return head;  
}

// Function to print the stops
void printStops(Stop* head) {
    Stop* current = head;
    while (current != NULL) {
        printf("Stop ID: %d\n", current->stopId);
        printf("Stop Name: %s\n", current->stopName);
        printf("Latitude: %.6f\n", current->lat);
        printf("longitude: %.6f\n\n", current->lon);
        current = current->next;  
    }
} 

// Function to parse time in HH:MM:SS format
Time parseTime(const char *timeStr) {
    Time t;
    sscanf(timeStr, "%d:%d:%d", &t.hour, &t.min, &t.sec);
    return t;
}

// Function to add a stop to a route
void addStopToRoute(Route *route, int stopId, Time time) {
    route->stops = realloc(route->stops, sizeof(int) * (route->stopCount + 1));
    route->times = realloc(route->times, sizeof(Time) * (route->stopCount + 1));
    route->stops[route->stopCount] = stopId;
    route->times[route->stopCount] = time;
    route->stopCount++;
}

// Function to read routes from a file
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

int main() {
	Stop *stops = readStopsFromFile("stops.txt");
	printStops(stops);
	
	int routeCount = 0;
    Route *routes = readRoutesFromFile("../data/bus/new_stop_times.txt", &routeCount);

    if (routes == NULL) {
    	printf("Failed\n");
        return 1;  // Exit if file reading failed
    }

    for (int i = 0; i < routeCount; i++) {
        printf("Route ID: %d\n", routes[i].routeId);
        for (int j = 0; j < routes[i].stopCount; j++) {
            printf("%d ", routes[i].stops[j]);
        }
        	printf("\n");
        for (int j = 0; j < routes[i].stopCount; j++) {
            printf("%02d:%02d:%02d ", routes[i].times[j].hour, routes[i].times[j].min, routes[i].times[j].sec);
        }
            printf("\n");
            printf("\n");
    }

    for (int i = 0; i < routeCount; i++) {
        free(routes[i].stops);
        free(routes[i].times);
    }
    
    free(routes);

    return 0;
	return 0;
}
