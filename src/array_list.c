//
//  array_list.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/25/24.
//

#include "array_list.h"
#include <stdlib.h>

array_list_t *array_list_make(int initial_capacity) {
	array_list_t *a = malloc(sizeof(array_list_t));
	if (!a) return NULL;
	
	void **array = malloc((size_t)initial_capacity * sizeof(void*));
	if (!array) {
		free(a);
		return NULL;
	}
	
	a->array = array;
	a->length = 0;
	a->capacity = initial_capacity;
	return a;
}

void array_list_destroy(array_list_t *a) {
	free(a->array);
	free(a);
}

bool array_list_add(array_list_t *a, void *element) {
	if (a->length >= a->capacity) {
		int new_cap = a->capacity * 2;
		void **new_array = malloc((size_t)new_cap * sizeof(void*));
		if (!new_array) return false;
		void **old_array = a->array;
		for (int i=0; i<a->capacity; i++) {
			new_array[i] = old_array[i];
		}
		a->array = new_array;
		a->capacity = new_cap;
		free(old_array);
	}
	
	a->array[a->length] = element;
	a->length++;
	
	return true;
}

bool array_list_remove(array_list_t *a, void *element) {
	int index = -1;
	for (int i=0; i<a->length; i++) {
		if (a->array[i] == element) {
			index = i;
		}
	}
	if (index < 0) return false;
	// Shift items after found index
	for (int i=index+1; i<a->length; i++) {
		a->array[i-1] = a->array[i];
	}
	a->length--;
	
	return true;
}

void array_list_remove_all(array_list_t *a) {
	a->length = 0;
}
