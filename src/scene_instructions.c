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


void instructions_init(void) {
	
	// Testing
	//mesh1 = mesh_create_grid(8);
	//mesh1 = mesh_create_diamond(4, 1.0f, 1.0f);
	mesh1 = mesh_create_3d_character('a');

	//mesh2 = mesh_create_pyramid();
	mesh2 = mesh_create_sphere(2);
	//mesh2 = mesh_create_cube();
}

void instructions_start(void) {
	const float deg = (float)M_PI / 180.0f;
	
	// Character
	mesh1->position = vec3_make(-2.5, 0, 0);
	mesh1->linear_momentum = vec3_zero();
	mesh1->rotation = vec3_zero();
	mesh1->angular_momentum = vec3_make(0, 0.5f * deg, 0.0f * deg);

	// Bouncing sphere
	mesh2->position = vec3_make(2.5, 0, 0);
	mesh2->linear_momentum = vec3_zero();
	mesh2->rotation = vec3_zero();
	mesh2->angular_momentum = vec3_make(0, -2 * deg, deg);
	
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
	double t = scene_lifetime;
	
	// Color cycling
	int hue = (int)round(t * 60.0) % 360;
	uint32_t line_color = color_from_hsv(hue, 1.0, 1.0, 0.5);
	uint32_t point_color = color_from_hsv((hue + 60) % 360, 1.0, 1.0, 0.5);
	
	mesh_set_children_color(mesh1, line_color, 0);
	mesh_set_children_color(mesh2, line_color, point_color);
	
	// Reset momentum when cube hits bottom
	if (mesh2->position.y < -1.0f) {
		mesh2->linear_momentum.y = 7.0f;
	}
}

void instructions_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	// Draw backgroujd
	p.x = scr_w/2 - image_basic_background->w/2;
	p.y = scr_h/2 - image_basic_background->h/2;
	move_to(p);
	draw_image(image_basic_background);

	// Draw meshes and shapes
	draw_shapes();
	draw_meshes();

	// Colors
	const uint32_t text1_color = COLOR_ABGR_BLACK;
	const uint32_t text2_color = rgb_to_abgr(COLOR_RGB_GRAY_20);
	const uint32_t red = rgb_to_abgr(COLOR_RGB_RED);
	
	// Draw text
	// "Instructions"
	p.x = scr_w / 2;
	p.y = 8;
	move_to(p);
	set_fill_color_abgr(text1_color);
	atari_draw_centered_text("Instructions", 2);
	
	// Body
	p.x = scr_w / 2;
	p.y = 32;
	move_to(p);
	set_fill_color_abgr(text1_color);
//	atari_draw_centered_text("Punch tomatoes to the beat!", 1);
	atari_draw_centered_text("Sit back and watch the show!", 1);

	// Controls
	const int line_height = 10;
	int left_margin = scr_w / 2 - 50;
	p.x = left_margin;
	p.y += line_height * 2;
	move_to(p);
	set_key_text_color(red);
//	set_fill_color_abgr(text1_color);
//	draw_key_text_line(NULL, "Controls:");
//	set_fill_color_abgr(text2_color);
//	draw_key_text_line("A", "punch left");
//	draw_key_text_line("D", "punch right");
//	draw_key_text_line(NULL, NULL);
	set_fill_color_abgr(text1_color);
	draw_key_text_line(NULL, "Music:");
	set_fill_color_abgr(text2_color);
	draw_key_text_line("<", "softer");
	draw_key_text_line(">", "louder");
	draw_key_text_line("M", "mute");

	p.x = scr_w / 2;
	p.y = scr_h - 16;
	move_to(p);
	set_fill_color_abgr(text1_color);
	atari_draw_centered_text("Press Space to Play", 1);
}

