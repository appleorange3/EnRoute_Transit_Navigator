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

int main() {
	Stop *stops = readStopsFromFile("stops.txt");
	printStops(stops);
	return 0;
}