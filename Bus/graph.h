#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>

#define MAX_STOPS 22757

typedef struct {
    int hr;
    int min;
    int sec;
} Time;

typedef struct Route {
	int routeId;
	int *stops;
	Time *times;
	Time *startTimes;
	float *fare;
	int stopCount;
} Route;

typedef struct MinHeapNode {
	int stopId;
	Time time;
} MinHeapNode;

typedef struct MinHeap {
	int size;
	int capacity;
	int *pos;
	MinHeapNode *array;
} MinHeap;

typedef struct AdjStop {
    int stop_id;
    int* bus_ids;
    int bus_count;
    struct AdjStop* next;
} AdjStop;

typedef struct AdjList {
    AdjStop* head;
    Time* times;
    float* fares;
    int size;
} AdjList;

typedef struct Stop {
    int stop_id;
    float stop_lat;
    float stop_lon;
    char stop_name[50];
} Stop;

typedef struct Graph {
    int num_stops;
    Stop* stops;
    AdjList* adjLists;
    int* stop_id_map;
} Graph;

Graph* create_graph(int num_stops);
AdjStop* create_adj_stop(int stop_id, int bus_id);
Route* createRoute(int routeId, int numStops);
MinHeapNode* createMinHeapNode(int stopId, Time time);
MinHeap* createMinHeap(int capacity);
void addStopToRoute(Route *route, int stopId, Time time);
void printRoutes(Route *routes, int routeCount);
void add_edge(Graph* graph, int from_stop_id, int to_stop_id, Time travel_time, float fare, int bus_id);
void display_graph(Graph* graph);
void free_graph(Graph* graph);
void free_adj_stop(AdjStop* stop);

#endif

