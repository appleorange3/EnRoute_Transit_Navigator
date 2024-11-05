#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "utilities.h"
AdjStop* create_adj_stop(int stop_id, int bus_id) {
    AdjStop* new_adj = (AdjStop*)malloc(sizeof(AdjStop));
    debug_memory(new_adj, "Failed to allocate memory for adjacency stop");

    new_adj->stop_id = stop_id;
    new_adj->bus_ids = (int*)malloc(sizeof(int));  // Initialize array for one bus ID
    debug_memory(new_adj->bus_ids, "Failed to allocate memory for bus_ids");
    
    new_adj->bus_ids[0] = bus_id;
    new_adj->bus_count = 1;
    new_adj->next = NULL;
    return new_adj;
}

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
void addStopToRoute(Route *route, int stopId, Time time) {
    route->stops = realloc(route->stops, sizeof(int) * (route->stopCount + 1));
    route->times = realloc(route->times, sizeof(Time) * (route->stopCount + 1));
    route->stops[route->stopCount] = stopId;
    route->times[route->stopCount] = time;
    route->stopCount++;
}

// Function to read routes from a file

void printRoutes(Route *routes, int routeCount) {
	for (int i = 0; i < routeCount; i++) {
        printf("Route ID: %d\n", routes[i].routeId);
        for (int j = 0; j < routes[i].stopCount; j++) {
            printf("%d ", routes[i].stops[j]);
        }
        	printf("\n");
        for (int j = 0; j < routes[i].stopCount; j++) {
            printf("%02d:%02d:%02d ", routes[i].times[j].hr, routes[i].times[j].min, routes[i].times[j].sec);
        }
            printf("\n");
            printf("\n");
    }
    return;
}

Graph* create_graph(int num_stops) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    debug_memory(graph, "Failed to allocate memory for graph");

    graph->num_stops = num_stops;
    graph->stops = (Stop*)malloc(num_stops * sizeof(Stop));
    debug_memory(graph->stops, "Failed to allocate memory for stops");

    graph->adjLists = (AdjList*)malloc(num_stops * sizeof(AdjList));
    debug_memory(graph->adjLists, "Failed to allocate memory for adjLists");

    graph->stop_id_map = (int*)malloc(MAX_STOPS * sizeof(int));
    for (int i = 0; i < MAX_STOPS; i++) graph->stop_id_map[i] = -1;

    for (int i = 0; i < num_stops; i++) {
        graph->adjLists[i].head = NULL;
        graph->adjLists[i].times = NULL;
        graph->adjLists[i].fares = NULL;
        graph->adjLists[i].size = 0;
    }
    return graph;
}

void add_edge(Graph* graph, int from_stop_id, int to_stop_id, Time travel_time, float fare, int bus_id) {
    int from_index = graph->stop_id_map[from_stop_id];
    int to_index = graph->stop_id_map[to_stop_id];

    if (from_index == -1 || to_index == -1) {
        fprintf(stderr, "Invalid stop_id: %d or %d\n", from_stop_id, to_stop_id);
        return;
    }

    AdjStop* adj = graph->adjLists[from_index].head;
    AdjStop* found_adj = NULL;

    while (adj) {
        if (adj->stop_id == to_stop_id) {
            found_adj = adj;
            break;
        }
        adj = adj->next;
    }

    if (found_adj) {
        add_bus_id(found_adj, bus_id);
    } else {

        AdjStop* new_adj = create_adj_stop(to_stop_id, bus_id);
        new_adj->next = graph->adjLists[from_index].head;
        graph->adjLists[from_index].head = new_adj;

        graph->adjLists[from_index].times = realloc(graph->adjLists[from_index].times,
                                                    (graph->adjLists[from_index].size + 1) * sizeof(Time));
        graph->adjLists[from_index].fares = realloc(graph->adjLists[from_index].fares,
                                                    (graph->adjLists[from_index].size + 1) * sizeof(float));

        graph->adjLists[from_index].times[graph->adjLists[from_index].size] = travel_time;
        graph->adjLists[from_index].fares[graph->adjLists[from_index].size] = fare;
        graph->adjLists[from_index].size++;
    }
}


void display_graph(Graph* graph) {
    for (int i = 0; i < 150; i++) {
        printf("Stop ID: %d, Name: %s\n", graph->stops[i].stop_id, graph->stops[i].stop_name);
        AdjStop* adj = graph->adjLists[i].head;
        int j = 0;
        while (adj) {
            printf("    -> Stop ID: %d, Bus IDs: ", adj->stop_id);
            for (int k = 0; k < (adj->bus_count); k++) {
                printf("%d ", adj->bus_ids[k]);
            }
            printf(", Time: %02d:%02d:%02d, Fare: %.2f\n",
                   graph->adjLists[i].times[j].hr,
                   graph->adjLists[i].times[j].min,
                   graph->adjLists[i].times[j].sec,
                   graph->adjLists[i].fares[j]);
            adj = adj->next;
            j++;
        }
    }
}

void free_graph(Graph* graph) {
    for (int i = 0; i < 2; i++) {
        free_adj_stop(graph->adjLists[i].head);
        free(graph->adjLists[i].times);
        free(graph->adjLists[i].fares);
    }
    free(graph->adjLists);
    free(graph->stops);
    free(graph);
}
void free_adj_stop(AdjStop* stop) {
    while (stop) {
        AdjStop* temp = stop;
        stop = stop->next;
        free(temp);
    }
}

