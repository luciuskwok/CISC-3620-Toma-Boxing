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
#include "array_list.h"
#include "mesh.h"
#include "mesh_creation.h"
#include "shape.h"


// Globals
array_list_t *title_shapes = NULL;

#define RAD_DEG ((float)M_PI / 180.0f)


shape_t *title_tomato(vec2_t position, float scale, float speed) {
	scale = 1.0f/7.0f * scale;
	shape_t *s = create_tomato_top_shape();
	s->angular_momentum = speed * RAD_DEG;
	s->scale = vec2_make(scale, scale);
	s->position = position;
	return s;
}

void title_init(void) {
	// Create tomato shapes that rotate
	title_shapes = array_list_new(16);
	const float grid = 0.2f;
		
	array_list_add(title_shapes, title_tomato(vec2_make(-3.5f * grid, 0), 0.5f, 5.0f));
	array_list_add(title_shapes, title_tomato(vec2_make(-2.875f * grid, -1.5f * grid), 1.0f, 10.0f));
	array_list_add(title_shapes, title_tomato(vec2_make(0.25f * grid, 1.5f * grid), 0.67f, 15.0f));
	array_list_add(title_shapes, title_tomato(vec2_make(0.5f * grid, -1.25f * grid), 0.75f, 8.0f));
	array_list_add(title_shapes, title_tomato(vec2_make(3.75f * grid, -1.0f * grid), 1.25f, 9.0f));
	
	// Testing: create shapes being tested
	//shape_t *test = create_heart_shape();
	//shape_t *test = create_rounded_rect_shape(3.2f, 2.0f, 0.25f);
	//shape_t *test = create_microphone_shape();
//	test->fill_color = rgba_to_abgr(COLOR_RGB_RED_1, 63);
//	test->scale = vec2_make(0.25f, 0.25f);
//	test->angular_momentum = 5.0f * RAD_DEG;
//	array_list_add(title_shapes, test);	
}

void title_start(void) {
	shape_t **s = (shape_t **)title_shapes->array;
	int n = title_shapes->length;
	for (int i=0; i<n; i++) {
		scene_add_shape(s[i]);
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

	// Draw background
	p.x = scr_w/2 - image_title_background->w/2;
	p.y = scr_h/2 - image_title_background->h/2;
	move_to(p);
	draw_image(image_title_background);
	
	// Draw meshes and shapes
	draw_meshes();
	draw_shapes();

	// Half-second flasher
	double t = fmod(scene_lifetime, 1.0);

	// Draw text
	
	// Press Space to Start
	const uint32_t shadow_color = COLOR_ABGR_BLACK;
	p.x = scr_w / 2 - (10 * 8);
	p.y = scr_h - 28;
	move_to(p);
	set_fill_color_abgr(COLOR_ABGR_WHITE);
	atari_draw_shadowed_text("Press ", 1, shadow_color);
	set_fill_color_abgr(rgb_to_abgr((t >= 0.5)? COLOR_RGB_GREEN_1 : COLOR_RGB_RED_1));
	atari_draw_shadowed_text("Space ", 1, shadow_color);
	set_fill_color_abgr(COLOR_ABGR_WHITE);
	atari_draw_shadowed_text("to Start", 1, shadow_color);

	// Test font
	// set_fill_color(0xFFCCFFCC);
	// atari_draw_test_text();
}

