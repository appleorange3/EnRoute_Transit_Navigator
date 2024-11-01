#include "struct.h"
#include "file.h"
#include "create.h"
#include <stdio.h>
#include <stdlib.h>


int main() {
	Stop *stops = readStopsFromFile("../data/bus/stops.txt");
	printStops(stops);
	
	int routeCount = 0;
    Route *routes = readRoutesFromFile("../data/bus/new_stop_times.txt", &routeCount);
    

    if (routes == NULL) {
    	printf("Failed\n");
        return 1;  // Exit if file reading failed
    }
	
    printRoutes(routes, routeCount);

    for (int i = 0; i < routeCount; i++) {
        free(routes[i].stops);
        free(routes[i].times);
    }
    
    free(routes);

	return 0;
}
