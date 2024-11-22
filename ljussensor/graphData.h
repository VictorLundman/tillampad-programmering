#ifndef GRAPH_DATA_H
#define GRAPH_DATA_H

typedef struct {
  int min;
  int max;
} MinMax;

#define GRAPH_DATA_LENGTH 128
typedef int GraphDataArray[GRAPH_DATA_LENGTH];

void advanceGraphData(GraphDataArray* data, int average);
MinMax getDataMinMax(GraphDataArray* data);

#endif