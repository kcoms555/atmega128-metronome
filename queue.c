#include <stdlib.h>
#include <string.h>

typedef unsigned long time;

typedef struct {
	int size;
	time * arr;
} queue;

queue * create_queue(int size){
	queue * q = NULL;

	q = malloc(sizeof(queue));
	if(q == NULL) return NULL;

	q->arr = malloc(sizeof(time) * size);
	if(q->arr == NULL) return NULL;

	memset(q->arr, 0x00, sizeof(time) * size);
	q->size = size;
	return q;
}
void free_queue(queue * q){
	if(q){
		if(q->arr) free(q->arr);
		q->arr = NULL;
		free(q);
		q = NULL;
	}
}
void insert_queue(queue * q, time t){
	if(!q) return;
	for(int i=q->size-1; i>0; i--){
		q->arr[i] = q->arr[i-1];
	}
	q->arr[0] = t;
}

time get_timegap_average(queue * q){
	time average = 0;
	for(int i=0; i<q->size-1; i++)
		average += q->arr[i] - q->arr[i+1];
	return average/q->size;
}

time get_front(queue * q){
	if(!q) return 0;
	return q->arr[0];
}

#ifdef _STDIO_H_
void print_queue(queue * q){
	if(!q) return;
	printf("[ ");
	for(int i=0; i<q->size; i++)
		printf("%d ", q->arr[i]);
	printf("]");
}
#endif
