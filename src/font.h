#ifndef FONT_H
#define FONT_H

typedef struct _FONT FONT;

FONT* create_font(const char *path, int size, SDL_Color color);
void destroy_font(FONT *font);
int get_text_size(FONT *font, const char *text, int size, int *width, int *height);
int print_font_to_renderer(FONT *font, SDL_Renderer *renderer, const char *text, int size, SDL_Color color, SDL_Point position);

#endif //FONT_H