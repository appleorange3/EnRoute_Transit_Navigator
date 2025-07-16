#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


// Define constants
#define MAX_STOPS 525     // Adjusted as per your requirements
#define MAX_ROUTES 36      // Adjusted as per your requirements

// Time structure to represent travel or schedule times
typedef struct {
    int hr;              
    int min;             
    int sec;             
} Time;

// Stop structure to represent metro stations
typedef struct {
    int stop_id;          
    char stop_name[100];  
    float latitude;       
    float longitude;     
} Stop;

// Adjacency List Node to represent connections between metro stops
typedef struct AdjStop {
    int stop_id;          // ID of the adjacent stop
    Time travel_time;     // Travel time to the adjacent stop
    float fare;           // Fare to the adjacent stop
    struct AdjStop* next;
    int num_lines;
    int *lines;
 // Pointer to the next adjacent stop
} AdjStop;

// Adjacency List for each metro stop
typedef struct AdjList {
    AdjStop* head;        // Head of the adjacency list
    int size;             // Number of adjacent stops
} AdjList;

// Route structure to represent metro routes
typedef struct {
    int route_id;        
    int* stops;          
    int stop_count;      
} Route;

// Graph structure to represent the entire metro system
typedef struct {
    int num_stops;       
    int num_routes;       
    Stop* stops;          
    AdjList* adjLists;    
    Route* routes;      
    int* stop_id_map;     // Mapping of stop IDs to indices in the stops array
} MetroGraph;

typedef struct MinHeapNode {
    int stop_id;              // ID of the stop
    int line_id;              // ID of the metro line used
    int total_time;           // Total time from source to this node
    int bus_transfer_count;   // Number of transfers so far
    int prev_stop_id;         // Previous stop in the path
} MinHeapNode;

// MinHeap
typedef struct MinHeap {
    int size;                 // Current number of elements in the heap
    int capacity;             // Maximum capacity of the heap
    int* positions;           // Maps stop IDs to their positions in the heap
    MinHeapNode* nodes;       // Array of heap nodes
} MinHeap;

// Function prototypes
MetroGraph* create_metro_graph(int num_stops, int num_routes);
void add_stop(MetroGraph* graph, int stop_id, const char* stop_name, float latitude, float longitude);
void add_edge(MetroGraph* graph, int from_stop_id, int to_stop_id, Time travel_time, int trip_id);
void add_route(MetroGraph* graph, int route_id, int* stops, int stop_count);
void free_metro_graph(MetroGraph* graph);
Time calculate_time_diff(const char* start_time, const char* end_time);
Time parseTime(const char* time_str);
void load_stops(const char* filename, MetroGraph* graph);
void load_stop_times(const char* filename, MetroGraph* graph);
void load_routes(const char* filename, MetroGraph* graph);
void load_trips(const char* filename, MetroGraph* graph);

void add_trip_id(AdjStop* adj, int trip_id) {
    // Reallocate the array to hold one more trip_id
    adj->lines = (int*)realloc(adj->lines, (adj->num_lines + 1) * sizeof(int));
    if (!adj->lines) {
        perror("Failed to allocate memory for trip IDs");
        exit(EXIT_FAILURE);
    }
    // Add the new trip_id to the array
    adj->lines[adj->num_lines] = trip_id;
    adj->num_lines++;
}

int timeToSeconds(Time time) {
    return time.hr * 3600 + time.min * 60 + time.sec;
}

MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->size = 0;
    heap->capacity = capacity;
    heap->positions = (int*)malloc(capacity * sizeof(int));
    heap->nodes = (MinHeapNode*)malloc(capacity * sizeof(MinHeapNode));
    for (int i = 0; i < capacity; i++) {
        heap->positions[i] = -1;
    }
    return heap;
}

void swapMinHeapNodes(MinHeapNode* a, MinHeapNode* b) {
    MinHeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->size && heap->nodes[left].total_time < heap->nodes[smallest].total_time) {
        smallest = left;
    }

    if (right < heap->size && heap->nodes[right].total_time < heap->nodes[smallest].total_time) {
        smallest = right;
    }

    if (smallest != idx) {
        MinHeapNode smallestNode = heap->nodes[smallest];
        MinHeapNode idxNode = heap->nodes[idx];

        heap->positions[smallestNode.stop_id] = idx;
        heap->positions[idxNode.stop_id] = smallest;

        swapMinHeapNodes(&heap->nodes[smallest], &heap->nodes[idx]);
        minHeapify(heap, smallest);
    }
}

MinHeapNode extractMin(MinHeap* heap) {
    if (heap->size == 0) {
        MinHeapNode emptyNode = {-1, -1, INT_MAX, INT_MAX, -1};
        return emptyNode;
    }

    MinHeapNode root = heap->nodes[0];
    MinHeapNode lastNode = heap->nodes[heap->size - 1];

    heap->nodes[0] = lastNode;

    heap->positions[root.stop_id] = -1;
    heap->positions[lastNode.stop_id] = 0;

    heap->size--;

    minHeapify(heap, 0);

    return root;
}

void decreaseKey(MinHeap* heap, int stop_id, int new_time, int line_id, int prev_stop_id, int transfer_count) {
    int i = heap->positions[stop_id];
    heap->nodes[i].total_time = new_time;
    heap->nodes[i].line_id = line_id;
    heap->nodes[i].prev_stop_id = prev_stop_id;
    heap->nodes[i].bus_transfer_count = transfer_count;

    while (i > 0 && heap->nodes[i].total_time < heap->nodes[(i - 1) / 2].total_time) {
        heap->positions[heap->nodes[i].stop_id] = (i - 1) / 2;
        heap->positions[heap->nodes[(i - 1) / 2].stop_id] = i;

        swapMinHeapNodes(&heap->nodes[i], &heap->nodes[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

void insertMinHeap(MinHeap* heap, MinHeapNode node) {
    if (heap->size == heap->capacity) {
        return;
    }

    heap->size++;
    int i = heap->size - 1;
    heap->nodes[i] = node;
    heap->positions[node.stop_id] = i;

    while (i > 0 && heap->nodes[i].total_time < heap->nodes[(i - 1) / 2].total_time) {
        heap->positions[heap->nodes[i].stop_id] = (i - 1) / 2;
        heap->positions[heap->nodes[(i - 1) / 2].stop_id] = i;

        swapMinHeapNodes(&heap->nodes[i], &heap->nodes[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

int isInMinHeap(MinHeap* heap, int stop_id) {
    return heap->positions[stop_id] != -1;
}




// Assuming other necessary structs and functions (like MinHeap) are defined above this

char* dijkstraShortestTime(MetroGraph* graph, int src_stop_id, int dest_stop_id) {
    const int TRANSFER_PENALTY_SECONDS = 5 * 60; // Transfer penalty (5 minutes)

    int num_stops = graph->num_stops;
    int* times = (int*)malloc(num_stops * sizeof(int));
    int* line_transfer_counts = (int*)malloc(num_stops * sizeof(int));
    int* prev_stop_ids = (int*)malloc(num_stops * sizeof(int));
    int* prev_line_ids = (int*)malloc(num_stops * sizeof(int));

    for (int i = 0; i < num_stops; ++i) {
        times[i] = INT_MAX;
        line_transfer_counts[i] = INT_MAX;
        prev_stop_ids[i] = -1;
        prev_line_ids[i] = -1;
    }

    int src_index = graph->stop_id_map[src_stop_id];
    int dest_index = graph->stop_id_map[dest_stop_id];
    times[src_index] = 0;
    line_transfer_counts[src_index] = 0;

    MinHeap* minHeap = createMinHeap(num_stops);
    insertMinHeap(minHeap, (MinHeapNode){src_stop_id, -1, 0, 0, -1});

    while (minHeap->size > 0) {
        MinHeapNode minNode = extractMin(minHeap);
        int current_stop_id = minNode.stop_id;
        int current_time = minNode.total_time;
        int current_transfer_count = minNode.bus_transfer_count;

        if (current_stop_id == dest_stop_id) {
            break;
        }

        int u_index = graph->stop_id_map[current_stop_id];
        AdjStop* adj = graph->adjLists[u_index].head;

        while (adj != NULL) {
            int v_index = graph->stop_id_map[adj->stop_id];

            // Consider all lines between the current stop and the adjacent stop
            for (int i = 0; i < adj->num_lines; i++) {
                int line_id = adj->lines[i];
                int travel_time = timeToSeconds(adj->travel_time);
                int total_time = current_time + travel_time;
                int total_transfer_count = current_transfer_count;

                // Check for a line transfer
                if (minNode.line_id != -1 && line_id != minNode.line_id) {
                    total_transfer_count++;
                    total_time += TRANSFER_PENALTY_SECONDS;
                }

                // Update the path if it has a better time or fewer transfers
                // If times and transfers are equal, prefer the path with the higher line ID
                if (total_time < times[v_index] || 
                   (total_time == times[v_index] && total_transfer_count < line_transfer_counts[v_index]) ||
                   (total_time == times[v_index] && total_transfer_count == line_transfer_counts[v_index] &&
                    line_id > prev_line_ids[v_index])) {

                    times[v_index] = total_time;
                    line_transfer_counts[v_index] = total_transfer_count;
                    prev_stop_ids[v_index] = current_stop_id;
                    prev_line_ids[v_index] = line_id;

                    insertMinHeap(minHeap, (MinHeapNode){adj->stop_id, line_id, total_time, total_transfer_count, current_stop_id});
                }
            }

            adj = adj->next;
        }
    }

    // Allocate a buffer for the result string
    char* result = (char*)malloc(10000 * sizeof(char));
    result[0] = '\0';  // Initialize the result string

    // Output the shortest path with the least travel time
    if (times[dest_index] != INT_MAX) {
        char temp[1024];
        snprintf(temp, sizeof(temp), "Shortest path from %d to %d:\n", src_stop_id, dest_stop_id);
        strcat(result, temp);

        int path[1000], path_index = 0, current_stop = dest_stop_id;

        while (current_stop != src_stop_id) {
            path[path_index++] = current_stop;
            current_stop = prev_stop_ids[graph->stop_id_map[current_stop]];
        }
        path[path_index++] = src_stop_id;

        int current_line = -1, start_stop = -1, accumulated_time = 0;
        for (int i = path_index - 1; i > 0; i--) {
            int stop = path[i];
            int next_stop = path[i - 1];
            int line_id = prev_line_ids[graph->stop_id_map[next_stop]];
            int travel_time = times[graph->stop_id_map[next_stop]] - times[graph->stop_id_map[stop]];

            if (line_id != current_line) {
                if (current_line != -1) {
                    snprintf(temp, sizeof(temp), "Line %d: Stop %d -> Stop %d, travel time: %02d:%02d:%02d\n", 
                           current_line, start_stop, stop, accumulated_time / 3600, 
                           (accumulated_time % 3600) / 60, accumulated_time % 60);
                    strcat(result, temp);
                }
                start_stop = stop;
                current_line = line_id;
                accumulated_time = travel_time;
            } else {
                accumulated_time += travel_time;
            }
        }

        snprintf(temp, sizeof(temp), "Line %d: Stop %d -> Stop %d, travel time: %02d:%02d:%02d\n", 
               current_line, start_stop, dest_stop_id, accumulated_time / 3600, 
               (accumulated_time % 3600) / 60, accumulated_time % 60);
        strcat(result, temp);

        snprintf(temp, sizeof(temp), "Total time: %02d:%02d:%02d\n", times[dest_index] / 3600, 
               (times[dest_index] % 3600) / 60, times[dest_index] % 60);
        strcat(result, temp);
    } else {
        snprintf(result, 10000, "No path found from %d to %d.\n", src_stop_id, dest_stop_id);
    }

    free(times);
    free(line_transfer_counts);
    free(prev_stop_ids);
    free(prev_line_ids);
    free(minHeap->positions);
    free(minHeap->nodes);
    free(minHeap);

    return result;  // Return the formatted result string
}



// Assuming other necessary structs and functions (like MinHeap) are defined above this

char* dijkstraFewestTransfers(MetroGraph* graph, int src_stop_id, int dest_stop_id) {
    const int TRANSFER_PENALTY_SECONDS = 5 * 60; // Transfer penalty (5 minutes)

    int num_stops = graph->num_stops;
    int* times = (int*)malloc(num_stops * sizeof(int));
    int* line_transfer_counts = (int*)malloc(num_stops * sizeof(int));
    int* prev_stop_ids = (int*)malloc(num_stops * sizeof(int));
    int* prev_line_ids = (int*)malloc(num_stops * sizeof(int));

    for (int i = 0; i < num_stops; ++i) {
        times[i] = INT_MAX;
        line_transfer_counts[i] = INT_MAX;
        prev_stop_ids[i] = -1;
        prev_line_ids[i] = -1;
    }

    int src_index = graph->stop_id_map[src_stop_id];
    int dest_index = graph->stop_id_map[dest_stop_id];
    times[src_index] = 0;
    line_transfer_counts[src_index] = 0;

    MinHeap* minHeap = createMinHeap(num_stops);
    insertMinHeap(minHeap, (MinHeapNode){src_stop_id, -1, 0, 0, -1});

    while (minHeap->size > 0) {
        MinHeapNode minNode = extractMin(minHeap);
        int current_stop_id = minNode.stop_id;
        int current_time = minNode.total_time;
        int current_transfer_count = minNode.bus_transfer_count;

        if (current_stop_id == dest_stop_id) {
            break;
        }

        int u_index = graph->stop_id_map[current_stop_id];
        AdjStop* adj = graph->adjLists[u_index].head;

        while (adj != NULL) {
            int v_index = graph->stop_id_map[adj->stop_id];

            // Evaluate all metro lines between the current stop and the adjacent stop
            for (int i = 0; i < adj->num_lines; i++) {
                int line_id = adj->lines[i];
                int travel_time = timeToSeconds(adj->travel_time);
                int total_time = current_time + travel_time;
                int total_transfer_count = current_transfer_count;

                // Check for a line transfer
                if (prev_line_ids[u_index] != line_id) {
                    total_transfer_count++;
                    total_time += TRANSFER_PENALTY_SECONDS;
                }

                // Prioritize fewest transfers first; if transfer count is the same, minimize time
                if (total_transfer_count < line_transfer_counts[v_index] || 
                   (total_transfer_count == line_transfer_counts[v_index] && total_time < times[v_index])) {
                    times[v_index] = total_time;
                    line_transfer_counts[v_index] = total_transfer_count;
                    prev_stop_ids[v_index] = current_stop_id;
                    prev_line_ids[v_index] = line_id;

                    insertMinHeap(minHeap, (MinHeapNode){adj->stop_id, line_id, total_time, total_transfer_count, current_stop_id});
                }
            }

            adj = adj->next;
        }
    }

    // Allocate a buffer for the result string
    char* result = (char*)malloc(10000 * sizeof(char));
    result[0] = '\0';  // Initialize the result string

    // Output the shortest path with fewest transfers
    if (times[dest_index] != INT_MAX) {
        char temp[1024];
        snprintf(temp, sizeof(temp), "Shortest path from %d to %d with fewest transfers:\n", src_stop_id, dest_stop_id);
        strcat(result, temp);

        int path[1000], path_index = 0, current_stop = dest_stop_id;

        while (current_stop != src_stop_id) {
            path[path_index++] = current_stop;
            current_stop = prev_stop_ids[graph->stop_id_map[current_stop]];
        }
        path[path_index++] = src_stop_id;

        int current_line = -1, start_stop = -1, accumulated_time = 0;
        for (int i = path_index - 1; i > 0; i--) {
            int stop = path[i];
            int next_stop = path[i - 1];
            int line_id = prev_line_ids[graph->stop_id_map[next_stop]];
            int travel_time = times[graph->stop_id_map[next_stop]] - times[graph->stop_id_map[stop]];

            if (line_id != current_line) {
                if (current_line != -1) {
                    snprintf(temp, sizeof(temp), "Line %d: Stop %d -> Stop %d, travel time: %02d:%02d:%02d\n", 
                           current_line, start_stop, stop, accumulated_time / 3600, 
                           (accumulated_time % 3600) / 60, accumulated_time % 60);
                    strcat(result, temp);
                }
                start_stop = stop;
                current_line = line_id;
                accumulated_time = travel_time;
            } else {
                accumulated_time += travel_time;
            }
        }

        snprintf(temp, sizeof(temp), "Line %d: Stop %d -> Stop %d, travel time: %02d:%02d:%02d\n", 
               current_line, start_stop, dest_stop_id, accumulated_time / 3600, 
               (accumulated_time % 3600) / 60, accumulated_time % 60);
        strcat(result, temp);

        snprintf(temp, sizeof(temp), "Total time: %02d:%02d:%02d\n", times[dest_index] / 3600, 
               (times[dest_index] % 3600) / 60, times[dest_index] % 60);
        strcat(result, temp);
    } else {
        snprintf(result, 10000, "No path found from %d to %d.\n", src_stop_id, dest_stop_id);
    }

    free(times);
    free(line_transfer_counts);
    free(prev_stop_ids);
    free(prev_line_ids);
    free(minHeap->positions);
    free(minHeap->nodes);
    free(minHeap);

    return result;  // Return the formatted result string
}




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

// Create a new metro graph
MetroGraph* create_metro_graph(int num_stops, int num_routes) {
    MetroGraph* graph = (MetroGraph*)malloc(sizeof(MetroGraph));
    graph->num_stops = num_stops;
    graph->num_routes = num_routes;

    graph->stops = (Stop*)malloc(num_stops * sizeof(Stop));
    graph->adjLists = (AdjList*)malloc(num_stops * sizeof(AdjList));
    graph->routes = (Route*)malloc(num_routes * sizeof(Route));
    graph->stop_id_map = (int*)malloc(num_stops * sizeof(int));

    // Initialize adjacency lists and stop_id_map
    for (int i = 0; i < num_stops; i++) {
        graph->adjLists[i].head = NULL;
        graph->adjLists[i].size = 0;
        graph->stop_id_map[i] = -1;
    }

    return graph;
}

// Add a stop to the graph


// Add an edge between two stops
void add_edge(MetroGraph* graph, int from_stop_id, int to_stop_id, Time travel_time, int trip_id) {
    int from_index = graph->stop_id_map[from_stop_id];
    int to_index = graph->stop_id_map[to_stop_id];

    // Validate indices
    if (from_index == -1 || to_index == -1) {
        fprintf(stderr, "Invalid stop IDs: from %d to %d\n", from_stop_id, to_stop_id);
        return;
    }
    AdjStop* adj = graph->adjLists[from_index].head;
    while (adj) {
        if (adj->stop_id == to_stop_id) {
            // Edge exists; add the trip_id to the lines array
            add_trip_id(adj, trip_id);
            return;
        }
        adj = adj->next;
    }
    // Create a new adjacency node
    AdjStop* new_adj = (AdjStop*)malloc(sizeof(AdjStop));
    if (!new_adj) {
        perror("Failed to allocate memory for AdjStop");
        exit(EXIT_FAILURE);
    }

    new_adj->stop_id = to_stop_id;
    new_adj->travel_time = travel_time;
    new_adj->fare = -1;
    new_adj->next = graph->adjLists[from_index].head;
    new_adj->lines = NULL;
    new_adj->num_lines = 0;

    add_trip_id(new_adj, trip_id);  // Link to existing list
    graph->adjLists[from_index].head = new_adj;       // Update head
    graph->adjLists[from_index].size++;               // Increment size
}

// Function to free the graph
void free_metro_graph(MetroGraph* graph) {
    if (!graph) return;

    for (int i = 0; i < graph->num_stops; i++) {
        AdjStop* current = graph->adjLists[i].head;
        while (current) {
            AdjStop* temp = current;
            current = current->next;
            free(temp);  // Free each AdjStop node
        }
        graph->adjLists[i].head = NULL;  // Avoid dangling pointer
    }

    free(graph->stops);
    graph->stops = NULL;

    free(graph->adjLists);
    graph->adjLists = NULL;

    free(graph->routes);
    graph->routes = NULL;

    free(graph->stop_id_map);
    graph->stop_id_map = NULL;

    free(graph);
}

// Load metro stops from stops.txt
void load_stops(const char* filename, MetroGraph* graph) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open stops file");
        exit(EXIT_FAILURE);
    }

    char line[200];
    fgets(line, sizeof(line), file); // Skip header line

    int i = 0;
    int line_number = 1; // Start counting from the first data line

    while (fgets(line, sizeof(line), file) && i < MAX_STOPS) {
        line_number++; // Increment line number for each line read

        int stop_id;
        float stop_lat, stop_lon;
        char stop_name[100];

        // Parse the fields from the line
        if (sscanf(line, "%d,%99[^,],%f,%f", &stop_id, stop_name, &stop_lat, &stop_lon) < 4) {
            fprintf(stderr, "Error parsing line %d: %s\n", line_number, line);
            continue; // Skip malformed lines
        }

        // Store the stop information in the graph
        graph->stops[i].stop_id = stop_id;
        graph->stops[i].latitude = stop_lat;
        graph->stops[i].longitude = stop_lon;
        strcpy(graph->stops[i].stop_name, stop_name);
        //printf("%d %d\n",graph->stops[i].stop_id,line_number);

        graph->stop_id_map[stop_id] = i;  // Map stop_id to index
        i++;
    }

    graph->num_stops = i; // Update the actual number of stops parsed
    fclose(file);
}



// Load stop times from stop_times.txt and create edges in the graph
void load_stop_times(const char* filename, MetroGraph* graph) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open stop_times file");
        exit(EXIT_FAILURE);
    }

    char line[500];
    fgets(line, sizeof(line), file);  // Skip the header line

    int prev_stop_id = -1, curr_stop_id, seq_no;
    char prev_time[10] = "", curr_time[10];
    int trip_id;

    while (fgets(line, sizeof(line), file)) {
        // Trim trailing newline
        line[strcspn(line, "\r\n")] = '\0';

        if (sscanf(line, "%d,%[^,],%*[^,],%d,%d,%*[^,],%*d,%*d,%*f,%*d,%*[^,],%*[^,]",
                   &trip_id, curr_time, &curr_stop_id, &seq_no) < 4) {
            fprintf(stderr, "Error parsing stop_times data: %s\n", line);
            continue;
        }

        if (seq_no > 0 && prev_stop_id != -1) {
            Time travel_time = calculate_time_diff(prev_time, curr_time);
            add_edge(graph, prev_stop_id, curr_stop_id, travel_time, trip_id);
        }

        prev_stop_id = curr_stop_id;
        strncpy(prev_time, curr_time, sizeof(prev_time) - 1);
        prev_time[sizeof(prev_time) - 1] = '\0';  // Ensure null termination
    }

    fclose(file);
}


// Load routes from routes.txt
void display_graph(const MetroGraph* graph) {
    if (!graph) {
        printf("Error: Graph is NULL.\n");
        return;
    }

    if (!graph->stops || graph->num_stops == 0) {
        printf("Error: No stops in the graph.\n");
        return;
    }

    if (!graph->adjLists) {
        printf("Error: Adjacency lists are NULL.\n");
        return;
    }

    printf("Metro Graph:\n");
    printf("=========================================\n");

    // Iterate over all stops
    for (int i = 0; i < graph->num_stops; i++) {
        Stop stop = graph->stops[i];

        // Print the stop details
        printf("Stop ID: %d\n", stop.stop_id);
        printf("Name: %s\n", stop.stop_name);
        printf("Location: (%.6f, %.6f)\n", stop.latitude, stop.longitude);

        // Display the adjacency list for the current stop
        AdjStop* adj = graph->adjLists[i].head;
        if (adj == NULL) {
            printf("  No connections from this stop.\n");
        } else {
            printf("  Connections:\n");
            while (adj) {
                // Print details of the connection
                printf("    -> Stop ID: %d, Travel Time: %02d:%02d:%02d, Fare: %.2f\n",
                       adj->stop_id,
                       adj->travel_time.hr,
                       adj->travel_time.min,
                       adj->travel_time.sec,
                       adj->fare);

                // Print the trip IDs (lines) between the two stops
                if (adj->num_lines > 0) {
                    printf("       Lines: ");
                    for (int j = 0; j < adj->num_lines; j++) {
                        printf("%d", adj->lines[j]);
                        if (j < adj->num_lines - 1) {
                            printf(", ");
                        }
                    }
                    printf("\n");
                } else {
                    printf("       No lines available.\n");
                }

                adj = adj->next;
            }
        }

        printf("=========================================\n");
    }
}



int main() {
    // File paths
    const char* stops_file = "cleaned_stops.txt";
    const char* stop_times_file = "final_routes.txt";
    

    // Initialize the graph
    int num_stops = 525;  // Adjust based on the expected number of stops
    int num_routes = 36;  // Adjust based on the expected number of routes
    MetroGraph* metro_graph = create_metro_graph(num_stops, num_routes);

    // Load data into the graph
    printf("Loading stops...\n");
    load_stops(stops_file, metro_graph);
    printf("Stops loaded.\n");

    printf("Loading stop times...\n");
    load_stop_times(stop_times_file, metro_graph);
    printf("Stop times loaded.\n");


    char* result1 = dijkstraShortestTime(metro_graph, 16,44);
printf("%s", result1);
free(result1);  // Don't forget to free the result string after use

    char* result = dijkstraFewestTransfers(metro_graph, 16,44);
printf("%s", result);
free(result);  // Remember to free the allocated memory for the result string

   //display_graph(metro_graph);

    // Free the graph memory
    free_metro_graph(metro_graph);
    printf("Memory freed and program completed.\n");



    return 0;
}

