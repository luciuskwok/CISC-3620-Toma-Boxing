//
//  point_list.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/30/24.
//

#include "point_list.h"
#include <stdlib.h>


point_list_t *point_list_new(int initial_capacity) {
	point_list_t *a = malloc(sizeof(point_list_t));
	if (!a) return NULL;
	
	vec2_t *array = malloc((size_t)initial_capacity * sizeof(vec2_t));
	if (!array) {
		free(a);
		return NULL;
	}
	
	a->array = array;
	a->length = 0;
	a->capacity = initial_capacity;
	return a;
}

void point_list_destroy(point_list_t *a) {
	free(a->array);
	free(a);
}

bool point_list_add(point_list_t *a, vec2_t point) {
	if (a->length >= a->capacity) {
		int new_cap = a->capacity * 2;
		vec2_t *new_array = malloc((size_t)new_cap * sizeof(vec2_t));
		if (!new_array) return false;
		vec2_t *old_array = a->array;
		for (int i=0; i<a->capacity; i++) {
			new_array[i] = old_array[i];
		}
		a->array = new_array;
		a->capacity = new_cap;
		free(old_array);
	}
	
	a->array[a->length] = point;
	a->length++;
	
	return true;
}

bool point_list_remove_at_index(point_list_t *a, int index) {
	if (index < 0) return false;
	// Shift items after found index
	for (int i=index+1; i<a->length; i++) {
		a->array[i-1] = a->array[i];
	}
	a->length--;
	
	return true;
}

void point_list_remove_all(point_list_t *a) {
	a->length = 0;
}
