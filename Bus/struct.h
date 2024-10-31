#include <stdio.h>
#pragma once

typedef struct Time {
	int hour;
	int min;
	int sec;
} Time;

typedef struct Stop {
	int stopId;
	char stopName[64];
	float lat, lon;
	struct Stop *next;
} Stop;

typedef struct AdjList {
	Stop *head;
	Time *AdjTime;
	float *fare;
} AdjList;

typedef struct Graph {
	int numStops;
	AdjList *arr;
} Graph;

typedef struct Route {
	int routeId;
	int *stops;
	Time *times;
	float *fare;
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
