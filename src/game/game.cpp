struct FadeText {
	u32 len;
	char* string;

	float* opacities;
	float* offsets;
	float* speeds;
};

struct Game {
	bool close_requested;
	u32 frames_since_init;

	Windowing::ButtonHandle quit_button;

	FadeText text;
};

Game* game_init(Windowing::Context* window, Arena* arena) 
{
	Game* game = (Game*)arena_alloc(arena, sizeof(Game));

	game->close_requested = false;
	game->frames_since_init = 0;

	game->quit_button = Windowing::register_key(window, Windowing::Keycode::Escape);

	const char* str = "Faust looked at his creation and smiled.";
	game->text.len = strlen(str);
	game->text.string = (char*)arena_alloc(arena, game->text.len + 1);
	game->text.offsets = (float*)arena_alloc(arena, sizeof(float) * game->text.len);
	game->text.opacities = (float*)arena_alloc(arena, sizeof(float) * game->text.len);
	game->text.speeds = (float*)arena_alloc(arena, sizeof(float) * game->text.len);
	strcpy(game->text.string, str);
	for(i32 i = 0; i < game->text.len; i++) {
		game->text.opacities[i] = 0.0f;
		game->text.offsets[i] = 32.0f + random_f32() * 64.0f;
		game->text.speeds[i] = 1.5f + random_f32() * 0.5f;
	}

	return game;
}

void game_update(Game* game, Windowing::Context* window, Render::Context* renderer)
{
	game->close_requested = Windowing::button_down(window, game->quit_button);
	game->frames_since_init++;

	FadeText* text = &game->text;
	float x_placements[text->len];
	float y_placements[text->len];
	text_line_placements(renderer, text->string, x_placements, y_placements, 32.0f, window->window_height - 32.0f, 0.0f, 1.0f, FONT_FACE_SMALL);

	for(i32 i = 0; i < text->len; i++) {
		text->opacities[i] += 0.001f * text->speeds[i];
		if(text->opacities[i] > 1.0f) {
			text->opacities[i] = 1.0f;
		}
		text->offsets[i] = lerp(text->offsets[i], 0.0f, text->speeds[i] * 0.001f);
		
		char c = text->string[i];
		Render::FontGlyph* glyph = &renderer->fonts[FONT_FACE_SMALL].glyphs[c];
		Render::character(renderer, c, x_placements[i] + text->offsets[i], y_placements[i], 1.0f, 1.0f, 1.0f, text->opacities[i], FONT_FACE_SMALL);
	}
}

bool game_close_requested(Game* game)
{
	return game->close_requested;
}
