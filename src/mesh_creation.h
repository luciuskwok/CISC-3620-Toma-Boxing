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
mesh_t *create_mesh_cube(void);
mesh_t *create_mesh_diamond(int sides, float top, float bottom);
mesh_t *create_mesh_sphere(int sides);

#endif /* MESH_CUBE_H */
