#include <stdio.h>
#include <stdlib.h>

int main() {
	FILE *input_file = fopen("./data/bus/trips.txt", "r");
	if(input_file == NULL) {
		perror("Failed to open input file");
        exit(EXIT_FAILURE);
	}
	FILE *output_file = fopen("./data/bus/new_trips.txt", "w");
	if(output_file == NULL) {
		perror("Failed to open output file");
        exit(EXIT_FAILURE);
	}
	char line[1024];
	int routeId, hr, min;
	fgets(line, sizeof(line), input_file);
	while(fgets(line, sizeof(line), input_file)) {
		sscanf(line, "%d,%*d,%*[^_]_%2d_%2d", &routeId, &hr, &min);
		fprintf(output_file, "%d, %d:%d:00\n", routeId, hr, min);
	}
	fclose(input_file);
	fclose(output_file);
	printf("Data has been written into output file\n");
	return 0;
}
