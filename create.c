#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "struct.h"

// Function to create a new Stop node
Stop* createStop(int stopId, float lat, float lon, const char *stopName) {
    Stop *newStop = (Stop *)malloc(sizeof(Stop));
    newStop->stopId = stopId;
    newStop->lat = lat;
    newStop->lon = lon;
    strncpy(newStop->stopName, stopName, 63);
    newStop->stopName[63] = '\0';  
    newStop->next = NULL;
    return newStop;
}

// Function to create a Graph
Graph* createGraph(int numStops) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numStops = numStops;

    graph->arr = (AdjList*)malloc(numStops * sizeof(AdjList));

    for (int i = 0; i < numStops; i++) {
        graph->arr[i].head = NULL;
        graph->arr[i].AdjTime = NULL;
        graph->arr[i].fare = NULL;
    }

    return graph;
}

// Function to add a Stop to the Adjacency List
void addStopToAdjList(Graph* graph, int src, int stopId, const char* stopName, float lat, float lon) {
    Stop* stop = createStop(stopId, lat, lon, stopName);

    stop->next = graph->arr[src].head;
    graph->arr[src].head = stop;
}

// Function to create a Bus
Route* createRoute(int routeId, int numStops) {
    Route* route = (Route*)malloc(sizeof(Route));
    route->routeId = routeId;

    route->stops = (int*)malloc(numStops * sizeof(int));
    route->times = (Time*)malloc(numStops * sizeof(Time));
    route->fare = (float*)malloc(numStops * sizeof(float));

    return route;
}

// Function to create a MinHeapNode
MinHeapNode* createMinHeapNode(int stopId, Time time) {
    MinHeapNode* node = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    node->stopId = stopId;
    node->time = time;
    return node;
}

// Function to create a MinHeap
MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->pos = (int*)malloc(capacity * sizeof(int));
    minHeap->array = (MinHeapNode*)malloc(capacity * sizeof(MinHeapNode));
    return minHeap;
}