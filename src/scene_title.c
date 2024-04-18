//
//  scene_title.c
//  Toma Boxing
//
//  Created by Lucius Kwok on 3/28/24.
//

#include "scene_title.h"
#include "atari_text.h"
#include "color.h"
#include "drawing.h"
#include "image.h"
#include "scene_manager.h"

#include "mesh.h"
#include "shape.h"


// Globals
image_t *title_image = NULL;


void title_init(void) {
	title_image = load_bmp_image("assets/title.bmp");
}

void set_scale_and_translate(mat3_t transform, float scale, float tx, float ty) {
	mat3_scale(transform, scale, scale);
	mat3_translate(transform, tx / scale, ty / scale);
}

void title_start(void) {
	const float grid = 0.2f;
	const float ty = -1.0f / 15.0f;
	
	// Create shapes that rotate
	shape_t *s[6];
	s[0] = create_polygon_shape(3);
	s[0]->momentum.rotation = 15.0f;
	set_scale_and_translate(s[0]->transform, 0.125f, -2 * grid, -grid + ty);
	s[0]->line_color = COLOR_LIME;
	s[0]->fill_color = 0x800000FF;
	
/*	// Debug: print triangle points
	printf("Triangle:\n");
	for (int i=0; i<3; i++) {
		printf("  %1.3f, %1.3f\n", s[0]->points[i].x, s[0]->points[i].y);
	}
*/
	s[1] = create_polygon_shape(4);
	s[1]->momentum.rotation = 10.0f;
	set_scale_and_translate(s[1]->transform, 0.125f, 0, -grid + ty);
	s[1]->line_color = COLOR_PINK;
	s[1]->fill_color = 0x800000FF;

	s[2] = create_polygon_shape(5);
	s[2]->momentum.rotation = 6.0f;
	set_scale_and_translate(s[2]->transform, 0.125f, 2 * grid, -grid + ty);
	s[2]->line_color = COLOR_RED;
	s[2]->fill_color = 0x800000FF;

	s[3] = create_polygon_shape(6);
	s[3]->momentum.rotation = 8.0f;
	set_scale_and_translate(s[3]->transform, 0.125f, -2 * grid, grid + ty);
	s[3]->line_color = COLOR_GREEN;
	s[3]->fill_color = 0x800000FF;

	s[4] = create_polygon_shape(8);
	s[4]->momentum.rotation = 12.0f;
	set_scale_and_translate(s[4]->transform, 0.125f, 0, grid + ty);
	s[4]->line_color = COLOR_BLUE;
	s[4]->fill_color = 0x800000FF;

	s[5] = create_polygon_shape(16);
	s[5]->momentum.rotation = 16.0f;
	s[5]->line_color = COLOR_WHITE;
	s[5]->fill_color = 0x800000FF;
	set_scale_and_translate(s[5]->transform, 0.125f, 2 * grid, grid + ty);

	for (int i = 0; i < 6; i++) {
		add_shape(s[i]);
	}
}

bool title_handle_keyboard(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				// Go to instructions scene
				set_scene_index(SCENE_INSTRUCTIONS);
				return true;
		}
	}
	
	return false;
}

void title_update(double delta_time) {
}

void title_render(void) {
	vec2_t p;
	int scr_w = get_screen_width();
	int scr_h = get_screen_height();

	set_fill_color(COLOR_BLACK);
	fill_screen();

	// Draw image
	p.x = scr_w/2 - title_image->w/2;
	p.y = scr_h/2 - title_image->h/2;
	move_to(p);
	//draw_image(title_image);
	
	// Half-second flasher
	double t = fmod(get_scene_lifetime(), 1.0);

	// Draw text
	p.x = scr_w / 2 - (20 * 8) / 2;
	p.y = scr_h - 16;
	move_to(p);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("Press ", 1);
	set_fill_color((t >= 0.5)? COLOR_LIME : COLOR_PINK);
	atari_draw_text("Space ", 1);
	set_fill_color(COLOR_WHITE);
	atari_draw_text("to Start", 1);

	// Test font
	// set_fill_color(0xFFCCFFCC);
	// atari_draw_test_text();
	
	// Draw meshes and shapes
	draw_meshes();
	draw_shapes();
}

