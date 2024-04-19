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
shape_t *title_shape = NULL;


void title_init(void) {
	title_image = load_bmp_image("assets/title.bmp");
}

void set_scale_and_translate(shape_t *shape, float scale, float tx, float ty) {
	shape->scale = vec2_make(scale, scale);
	shape->position = vec2_make(tx, ty);
}

void title_start(void) {
	const float grid = 0.2f;
	const float ty = -1.0f / 15.0f;
	const float rad_deg = (float)M_PI / 180.0f;
		
	// Create shapes that rotate
	shape_t *s[6];
	s[0] = create_polygon_shape(3);
	s[0]->angular_momentum = 15.0f * rad_deg;
	set_scale_and_translate(s[0], 0.125f, -2 * grid, -grid + ty);
	s[0]->line_color = COLOR_LIME;
	s[0]->fill_color = color_set_alpha(COLOR_LIME, 96);
	title_shape = s[0];

	s[1] = create_polygon_shape(4);
	s[1]->angular_momentum = 10.0f * rad_deg;
	set_scale_and_translate(s[1], 0.125f, 0, -grid + ty);
	s[1]->line_color = COLOR_PINK;
	s[0]->fill_color = color_set_alpha(COLOR_PINK, 96);

	s[2] = create_star_shape(5, 0.5f);
	s[2]->angular_momentum = 6.0f * rad_deg;
	set_scale_and_translate(s[2], 0.125f, 2 * grid, -grid + ty);
	s[2]->line_color = COLOR_RED;
	s[0]->fill_color = color_set_alpha(COLOR_RED, 96);

	s[3] = create_polygon_shape(6);
	s[3]->angular_momentum = 8.0f * rad_deg;
	set_scale_and_translate(s[3], 0.125f, -2 * grid, grid + ty);
	s[3]->line_color = COLOR_GREEN;
	s[0]->fill_color = color_set_alpha(COLOR_GREEN, 96);

	s[4] = create_star_shape(8, 0.25f);
	s[4]->angular_momentum = 12.0f * rad_deg;
	set_scale_and_translate(s[4], 0.125f, 0, grid + ty);
	s[4]->line_color = COLOR_BLUE;
	s[0]->fill_color = color_set_alpha(COLOR_BLUE, 96);

	s[5] = create_star_shape(16, 0.75f);
	s[5]->angular_momentum = 16.0f * rad_deg;
	s[5]->line_color = COLOR_WHITE;
	s[0]->fill_color = color_set_alpha(COLOR_WHITE, 96);
	set_scale_and_translate(s[5], 0.125f, 2 * grid, grid + ty);

	for (int i = 0; i < 6; i++) {
		add_shape(s[i]);
	}

}

bool title_handle_keyboard(SDL_Event event) {
	const float rad_deg = (float)M_PI / 180.0f;
	const float angle = 5.0f * rad_deg;
	const float move = 1.0f / 32.0f;
	
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				// Go to instructions scene
				set_scene_index(SCENE_INSTRUCTIONS);
				return true;
			case SDLK_q:
				title_shape->rotation -= angle;
				printf("Rotation: %1.0f\n", title_shape->rotation / rad_deg);
				return true;
			case SDLK_e:
				title_shape->rotation += angle;
				printf("Rotation: %1.0f\n", title_shape->rotation / rad_deg);
				return true;
			case SDLK_w:
				title_shape->position = vec2_add(title_shape->position, vec2_make(0.0f, -move));
				return true;
			case SDLK_s:
				title_shape->position = vec2_add(title_shape->position, vec2_make(0.0f, move));
				return true;
			case SDLK_a:
				title_shape->position = vec2_add(title_shape->position, vec2_make(-move, 0.0f));
				return true;
			case SDLK_d:
				title_shape->position = vec2_add(title_shape->position, vec2_make(move, 0.0f));
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

