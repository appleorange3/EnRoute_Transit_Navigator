#include "graph.h"
#include "file_operations.h"
#include <stdlib.h>

int main() {
    int num_stops = 10560;
    Graph* graph = create_graph(num_stops);

    load_stops("stops.txt", graph);
    load_stop_times("stop_times.txt", graph);
    load_fares("fare_attributes.txt", graph);

    display_graph(graph);

    free_graph(graph);

    int routeCount = 0;
    Route *routes = readRoutesFromFile("new_stop_times.txt", &routeCount);
    

    if (routes == NULL) {
    	printf("Failed\n");
        return 1;  // Exit if file reading failed
    }
	
    //printRoutes(routes, routeCount);

    for (int i = 0; i < routeCount; i++) {
        free(routes[i].stops);
        free(routes[i].times);
    }
    
    free(routes);

    return 0;
}

