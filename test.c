#include <stdio.h>

int main() {
    const char *line = "142,1,142_12_50,";
    int routeId, hr, min;

    // Adjusted format to capture routeId, hour, and minute properly
    sscanf(line, "%d,%*d,%*[^_]_%2d_%2d", &routeId, &hr, &min);

    printf("Route ID: %d\n", routeId);
    printf("Hour: %02d\n", hr);
    printf("Minute: %02d\n", min);

    return 0;
}

