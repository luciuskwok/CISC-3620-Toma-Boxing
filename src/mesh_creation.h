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

#endif /* MESH_CUBE_H */
