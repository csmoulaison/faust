#define MAX_ANIMATED_CHARS 4096
#define MAX_ANIMATED_STRINGS 64

#define MAX_PAGE_LINES 32

struct AnimatedChar {
	float opacity;
	float x_offset;
	float speed;
};

struct PageLine {
	u32 string_id;
};

struct Game {
	bool close_requested;
	u32 frames_since_init;

	Windowing::ButtonHandle quit_button;

	char char_buffer[MAX_ANIMATED_CHARS];
	AnimatedChar char_attributes[MAX_ANIMATED_CHARS];
	u32 string_offsets[MAX_ANIMATED_STRINGS];
	u32 strings_len;

	PageLine page_lines[MAX_PAGE_LINES];
	u32 page_lines_len;
};

u32 create_animated_string(Game* game, const char* string)
{
	u32 len = strlen(string) + 1;
	game->strings_len++;
	game->string_offsets[game->strings_len] = game->string_offsets[game->strings_len - 1] + len;
	
	for(i32 i = 0; i < len; i++) {
		u32 off = game->string_offsets[game->strings_len - 1];
		u32 index = off + i;

		AnimatedChar* attributes = &game->char_attributes[index];
		attributes->opacity = 0.0f - random_f32() * 0.2f;
		attributes->x_offset = 48.0f * random_f32();
		attributes->speed = 7.5f + random_f32() * 2.5f;
		game->char_buffer[index] = string[i];
	}

	return game->strings_len - 1;
}

Game* game_init(Windowing::Context* window, Arena* arena) 
{
	Game* game = (Game*)arena_alloc(arena, sizeof(Game));

	game->close_requested = false;
	game->frames_since_init = 0;

	game->quit_button = Windowing::register_key(window, Windowing::Keycode::Escape);

	game->strings_len = 0;
	game->string_offsets[0] = 0;

	game->page_lines[0].string_id = create_animated_string(game, "Doctor Faust in Neu-Ruppin");
	game->page_lines[1].string_id = create_animated_string(game, "It is said that Doctor Faust also once lived in Neu-Ruppin.");
	game->page_lines[2].string_id = create_animated_string(game, "They say that in the evening he usually played cards with some of the residents, and that he won a lot.");
	game->page_lines[3].string_id = create_animated_string(game, "One evening one of the players dropped a card beneath the table, and when he picked it up he saw that the doctor had horse-feet.");
	game->page_lines[4].string_id = create_animated_string(game, "With that it became evident why he always won so much.");
	game->page_lines[5].string_id = create_animated_string(game, "Long after his death he could often be seen in a thicket on the lake shore sitting at a table with a number of people and playing cards.");
	game->page_lines[6].string_id = create_animated_string(game, "That is where he continues to practice his art.");
	game->page_lines_len = 7;

	return game;
}

void game_update(Game* game, Windowing::Context* window, Render::Context* renderer)
{
	game->close_requested = Windowing::button_down(window, game->quit_button);
	game->frames_since_init++;

	float t = game->frames_since_init / 200.0f;

	for(i32 i = 0; i < game->page_lines_len; i++) {
		PageLine* line = &game->page_lines[i];
		u32 string_offset = game->string_offsets[line->string_id];
		char* string = &game->char_buffer[string_offset];

		u32 len = game->string_offsets[line->string_id + 1] - game->string_offsets[line->string_id];
		float x_placements[len];
		float y_placements[len];

		FontFace face = FONT_FACE_SMALL;
		float yoff = window->window_height - 32.0f;
		if(i == 0) {
			face = FONT_FACE_LARGE;
		} else {
			yoff -= 64.0f + 96.0f * i;
		}

		text_line_placements(renderer, string, x_placements, y_placements, 32.0f, yoff, 0.0f, 1.0f, face);

		for(i32 j = 0; j < len; j++) {
			u32 char_index = string_offset + j;
			AnimatedChar* attributes = &game->char_attributes[char_index];

			if(t > (i - 0.5f) * 6.5f + j * 0.025f) {
				attributes->opacity += 0.001f * attributes->speed;
				if(attributes->opacity > 1.0f) {
					attributes->opacity = 1.0f;
				}
				attributes->x_offset = lerp(attributes->x_offset, 0.0f, attributes->speed * 0.0016f);
			}
			
			char c = string[j];
			Render::FontGlyph* glyph = &renderer->fonts[FONT_FACE_SMALL].glyphs[c];
			Render::character(renderer, c, x_placements[j] + attributes->x_offset, y_placements[j], 0.2f + attributes->opacity * 0.8f, attributes->opacity, 1.0f, attributes->opacity, face);
		}
	}
}

bool game_close_requested(Game* game)
{
	return game->close_requested;
}
