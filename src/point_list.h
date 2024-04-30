//
//  point_list.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 4/30/24.
//

#ifndef point_list_h
#define point_list_h

#include "vector.h"
#include <stdbool.h>


typedef struct {
	int length;
	int capacity;
	vec2_t *array;
} point_list_t;

point_list_t *point_list_new(int initial_capacity);
void point_list_destroy(point_list_t *a);
bool point_list_add(point_list_t *a, vec2_t point);
bool point_list_remove(point_list_t *a, vec2_t point);
void point_list_remove_all(point_list_t *a);

#endif /* point_list_h */
