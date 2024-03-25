//
//  cube_mesh.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/10/24.
//

#ifndef CUBE_MESH_H
#define CUBE_MESH_H

#include <stdint.h>
#include "vector.h"

// Face (triangle)
typedef struct {
    int a, b, c;
} face_t;

typedef struct {
    vec2_t a, b, c;
} triangle_t;


// Functions
void init_cube(void);
void update_cube(uint64_t frame_index);
void render_cube(void);

void cube_reset_transform(void);
void cube_reset_momentum(void);
void cube_add_pitch(float x);
void cube_add_roll(float x);
void cube_add_yaw(float x);

#endif /* CUBE_MESH_H */
