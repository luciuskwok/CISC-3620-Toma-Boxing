//
//  array_list.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/25/24.
//

#ifndef array_list_h
#define array_list_h

#include <stdbool.h>

typedef struct {
	int length;
	int capacity;
	void **array;
} array_list_t;

array_list_t *array_list_make(int initial_capacity);
void array_list_destroy(array_list_t *a);
bool array_list_add(array_list_t *a, void *element);
bool array_list_remove(array_list_t *a, void *element);
void array_list_remove_all(array_list_t *a);


#endif /* array_list_h */
