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
#include "mesh_cube.h"


// Globals
image_t *instr_track_image = NULL;
mesh_t *cube1 = NULL;
mesh_t *cube2 = NULL;


void instructions_init(void) {
	instr_track_image = load_bmp_image("assets/track.bmp");
	cube1 = create_mesh_cube();
	cube2 = create_mesh_cube();
	
}

void instructions_start(void) {
	add_mesh(cube1);
	add_mesh(cube2);
	
	// Have cube spin by itself
	mesh_reset_transform(cube1);
	mat4_translate(cube1->transform, 2.5, 0, 0);
	mesh_reset_transform(cube2);
	mat4_translate(cube2->transform, -2.5, 0, 0);
	
	mesh_reset_momentum(cube1);
	mesh_add_yaw(cube1, 5.0f);
	mesh_add_pitch(cube1, 1.0f);
	
	mesh_reset_momentum(cube2);
	mesh_add_yaw(cube2, -10.0f);
	mesh_add_pitch(cube2, -5.0f);
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
	
	cube1->line_color = line_color;
	cube2->line_color = line_color;
	cube1->point_color = point_color;
	cube2->point_color = point_color;

}

void instructions_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color(COLOR_BLACK);
	fill_screen();

	// Draw 2 tracks
	p.x = scr_w / 4 - instr_track_image->w / 2;
	p.y = scr_h - instr_track_image->h;
	move_to(p);
	draw_image(instr_track_image);
	p.x = scr_w * 3 / 4 - instr_track_image->w / 2;
	move_to(p);
	draw_image(instr_track_image);

	// Draw meshes and shapes
	draw_shapes();
	draw_meshes();

	// Draw text
	// "Instructions"
	p.x = scr_w / 2;
	p.y = 8;
	move_to(p);
	set_fill_color(COLOR_WHITE);
	atari_draw_centered_text("Instructions", 2);
	
	// Body
	const int line_height = 12;
	int left_margin = scr_w / 4 + 16;
	p.x = left_margin;
	p.y += line_height * 4;
	move_to(p);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("Controls: ", 1);

	p.y += line_height;
	move_to(p);
	set_fill_color(COLOR_RED);
	atari_draw_text("A ", 1);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("Punch left", 1);
	
	p.y += line_height;
	move_to(p);
	set_fill_color(COLOR_RED);
	atari_draw_text("D ", 1);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("Punch right", 1);
	
	p.y += line_height * 2;
	move_to(p);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("Volume:", 1);
	
	p.y += line_height;
	move_to(p);
	set_fill_color(COLOR_RED);
	atari_draw_text("< ", 1);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("Lower volume", 1);
	
	p.y += line_height;
	move_to(p);
	set_fill_color(COLOR_RED);
	atari_draw_text("> ", 1);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("Increase volume", 1);

	p.y += line_height;
	move_to(p);
	set_fill_color(COLOR_RED);
	atari_draw_text("M ", 1);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("Mute volume", 1);

	p.x = scr_w - 2;
	p.y = scr_h - 10;
	move_to(p);
	atari_draw_right_justified_text("Press Space to Play", 1);
}

