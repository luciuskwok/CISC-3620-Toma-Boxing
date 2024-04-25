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

array_list_t *make_array_list(int initial_capacity);
void destroy_array_list(array_list_t *a);
bool array_list_add(array_list_t *a, void *element);
bool array_list_remove(array_list_t *a, void *element);
void array_list_remove_all(array_list_t *a);
int array_list_length(array_list_t *a);
void **array_list_array(array_list_t *a);


#endif /* array_list_h */
