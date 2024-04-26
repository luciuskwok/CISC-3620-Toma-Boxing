//
//  scene_instructions.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_instructions.h"
#include "atari_text.h"
#include "color.h"
#include "drawing.h"
#include "image.h"
#include "scene_manager.h"

#include "mesh.h"
#include "mesh_creation.h"


// Globals
mesh_t *mesh1 = NULL;
mesh_t *mesh2 = NULL;
image_t *instructions_image = NULL;


void instructions_init(void) {
	//mesh1 = mesh_create_diamond(4, 1.0f, 1.0f);
	mesh1 = mesh_create_sphere(1);
	mesh2 = mesh_create_cube();
	instructions_image = load_bmp_image("assets/instructions_background.bmp");
}

void instructions_start(void) {
	const float rotationUnit = 1.0f * (float)M_PI / 180.0f;
	
	// Diamond
	mesh1->position = vec3_make(2.5, 0, 0);
	mesh1->linear_momentum = vec3_zero();
	mesh1->rotation = vec3_zero();
	mesh1->angular_momentum = vec3_make(0, rotationUnit, 0.125f * rotationUnit);

	// Bouncing cube
	mesh2->position = vec3_make(-2.5, 0, 0);
	mesh2->linear_momentum = vec3_zero();
	mesh2->rotation = vec3_zero();
	mesh2->angular_momentum = vec3_make(0, -2 * rotationUnit, rotationUnit);
	
	// Set translation momentum & gravity
	mesh2->gravity = true;
	
	// Add meshes to scene
	scene_add_mesh(mesh1);
	scene_add_mesh(mesh2);
}

bool instructions_handle_keyboard(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				// Go to gameplay scene
				set_scene_index(SCENE_GAMEPLAY);
				return true;
		}
	}
	
	return false;
}

void instructions_update(double delta_time) {
	double t = get_scene_lifetime();
	
	// Color cycling
	int hue = (int)round(t * 60.0) % 360;
	uint32_t line_color = color_from_hsv(hue, 1.0, 1.0, 0.5);
	uint32_t point_color = color_from_hsv((hue + 60) % 360, 1.0, 1.0, 0.5);
	
	mesh1->line_color = line_color;
	mesh2->line_color = line_color;
	mesh1->point_color = point_color;
	mesh2->point_color = point_color;
	
	// Reset momentum when cube hits bottom
	if (mesh2->position.y < -1.0f) {
		mesh2->linear_momentum.y = 7.0f;
	}
}

void instructions_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color(COLOR_ABGR_BLACK);
	fill_screen();

	// Draw image
	p.x = scr_w/2 - instructions_image->w/2;
	p.y = scr_h/2 - instructions_image->h/2;
	move_to(p);
	draw_image(instructions_image);

	// Draw meshes and shapes
	draw_shapes();
	draw_meshes();

	// Colors
	const uint32_t text = COLOR_ABGR_BLACK;
	const uint32_t red = rgb_to_abgr(COLOR_RGB_RED);
	
	// Draw text
	// "Instructions"
	p.x = scr_w / 2;
	p.y = 8;
	move_to(p);
	set_fill_color(text);
	atari_draw_centered_text("Instructions", 2);
	
	// Body
	p.x = scr_w / 2;
	p.y = 32;
	move_to(p);
	set_fill_color(text);
	atari_draw_centered_text("Punch tomatoes to the beat!", 1);

	// Controls
	const int line_height = 12;
	int left_margin = scr_w / 4 + 16;
	p.x = left_margin;
	p.y += line_height * 2;
	move_to(p);
	set_fill_color(text);
	atari_draw_text("Controls: ", 1);

	p.y += line_height;
	move_to(p);
	set_fill_color(red);
	atari_draw_text("A ", 1);
	set_fill_color(text);
	atari_draw_text("Punch left", 1);
	
	p.y += line_height;
	move_to(p);
	set_fill_color(red);
	atari_draw_text("D ", 1);
	set_fill_color(text);
	atari_draw_text("Punch right", 1);
	
	p.y += line_height * 2;
	move_to(p);
	set_fill_color(text);
	atari_draw_text("Volume:", 1);
	
	p.y += line_height;
	move_to(p);
	set_fill_color(red);
	atari_draw_text("< ", 1);
	set_fill_color(text);
	atari_draw_text("Quieter", 1);
	
	p.y += line_height;
	move_to(p);
	set_fill_color(red);
	atari_draw_text("> ", 1);
	set_fill_color(text);
	atari_draw_text("Louder", 1);

	p.y += line_height;
	move_to(p);
	set_fill_color(red);
	atari_draw_text("M ", 1);
	set_fill_color(text);
	atari_draw_text("Mute", 1);

	p.x = scr_w / 2;
	p.y = scr_h - 16;
	move_to(p);
	atari_draw_centered_text("Press Space to Play", 1);
}

