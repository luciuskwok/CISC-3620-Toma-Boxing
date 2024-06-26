//
//  mesh_creation.h
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/10/24.
//

#ifndef MESH_CUBE_H
#define MESH_CUBE_H

#include <stdint.h>
#include "mesh.h"

// Create various solids
mesh_t *mesh_create_cube(void);
mesh_t *mesh_create_diamond(int sides, float top, float bottom);
mesh_t *mesh_create_icosahedron(void);
mesh_t *mesh_create_sphere(int subdivisions);

mesh_t *mesh_create_grid(int subdivisions);
mesh_t *mesh_create_pyramid(void);
mesh_t *mesh_create_ufo(void);
mesh_t *mesh_create_traffic_cone(void);

// 3D Text
mesh_t *mesh_create_3d_character(char c);

#endif /* MESH_CUBE_H */
