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

#define RAD_DEG ((float)M_PI / 180.0f)


void title_init(void) {
	title_image = load_bmp_image("assets/title.bmp");
}

void set_scale_and_translate(shape_t *shape, float scale, float tx, float ty) {
	shape->scale = vec2_make(scale, scale);
	shape->position = vec2_make(tx, ty);
}

// Return type for create_tomato_shapes()
typedef struct {
	shape_t *body;
	shape_t *top;
} tomato_shapes_t;

tomato_shapes_t create_tomato_shapes(vec2_t position, float scale, float speed) {
	tomato_shapes_t t;
	
	// Tomato body
	const float body_scale = 1.0f/7.0f * scale;
	t.body = create_polygon_shape(15);
	t.body->angular_momentum = speed * RAD_DEG;
	t.body->scale = vec2_make(body_scale, body_scale);
	t.body->position = position;
	t.body->line_color = COLOR_ABGR_WHITE;
	t.body->fill_color = rgb_to_abgr(COLOR_RGB_TOMATO_RED);
	
	// Tomato leaves
	const float top_scale = 1.0f/13.0f * scale;
	t.top = create_star_shape(5, 0.25f);
	t.top->angular_momentum = speed * RAD_DEG;
	t.top->scale = vec2_make(top_scale, top_scale);
	t.top->position = position;
	t.top->line_color = 0;
	t.top->fill_color = rgb_to_abgr(COLOR_RGB_TOMATO_GREEN);
	
	return t;
}

void title_start(void) {
	const float grid = 0.2f;
	const float ty = -1.0f / 15.0f;
		
	// Create tomato shapes that rotate
	tomato_shapes_t t;
	
	t = create_tomato_shapes(vec2_make(-2.5f * grid, 1.5f * grid + ty), 1.0f, 10.0f);
	add_shape(t.body);
	add_shape(t.top);
	
	t = create_tomato_shapes(vec2_make(-2.75f * grid, ty), 0.5f, 5.0f);
	add_shape(t.body);
	add_shape(t.top);

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
				if (title_shape) {
					title_shape->rotation -= angle;
					printf("Rotation: %1.0f\n", title_shape->rotation / rad_deg);
				}
				return true;
			case SDLK_e:
				if (title_shape) {
					title_shape->rotation += angle;
					printf("Rotation: %1.0f\n", title_shape->rotation / rad_deg);
				}
				return true;
			case SDLK_w:
				if (title_shape) {
					title_shape->position = vec2_add(title_shape->position, vec2_make(0.0f, -move));
				}
				return true;
			case SDLK_s:
				if (title_shape) {
					title_shape->position = vec2_add(title_shape->position, vec2_make(0.0f, move));
				}
				return true;
			case SDLK_a:
				if (title_shape) {
					title_shape->position = vec2_add(title_shape->position, vec2_make(-move, 0.0f));
				}
				return true;
			case SDLK_d:
				if (title_shape) {
					title_shape->position = vec2_add(title_shape->position, vec2_make(move, 0.0f));
				}
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

	// Draw image
	p.x = scr_w/2 - title_image->w/2;
	p.y = scr_h/2 - title_image->h/2;
	move_to(p);
	draw_image(title_image);
	
	// Draw meshes and shapes
	draw_meshes();
	draw_shapes();

	// Half-second flasher
	double t = fmod(get_scene_lifetime(), 1.0);

	// Draw text
	p.x = scr_w / 2 - (12 * 8);
	p.y = scr_h - 30;
	move_to(p);
	set_fill_color(COLOR_ABGR_WHITE);
	atari_draw_text("Press ", 1);
	set_fill_color(rgb_to_abgr((t >= 0.5)? COLOR_RGB_LIME : COLOR_RGB_PINK));
	atari_draw_text("Space ", 1);
	set_fill_color(COLOR_ABGR_WHITE);
	atari_draw_text("to Start", 1);

	// Test font
	// set_fill_color(0xFFCCFFCC);
	// atari_draw_test_text();
}

