#include "font.h"

typedef struct _FONT {
    TTF_Font *ttf_font;
    int size;
} FONT;

FONT* create_font(const char *path, int size, SDL_Color color) {
    FONT *font = NULL;

    if (TTF_Init() == false) {
        SDL_Log("TTF_Init error: %s", SDL_GetError());
        return NULL;
    }


    font = SDL_malloc(sizeof(*font));
    if (font == NULL) {
        SDL_Log("SDL_malloc error: %s", SDL_GetError());
        return NULL;
    }

    font->ttf_font = TTF_OpenFont(path, size);
    if (font->ttf_font == NULL) {
        SDL_Log("TTF_OpenFont error: %s", SDL_GetError());
        SDL_free(font);
        return NULL;
    }

    font->size = size;
}

void destroy_font(FONT *font) {
    if (font != NULL) {
        TTF_CloseFont(font->ttf_font);
        SDL_free(font);
    }
}

static int set_font_size(FONT *font, int size) {

    if (font == NULL) {
        return 0;
    }

    if (TTF_SetFontSize(font->ttf_font, size) == false) {
        SDL_Log("TTF_SetFontSize error: %s", SDL_GetError());
        return 0;
    }

    font->size = size;
}

int get_text_size(FONT *font, const char *text, int size, int *width, int *height) {

    if (font == NULL || text == NULL) {
        SDL_Log("Ne postoji font ili text");
        return 0;
    }

    if (set_font_size(font, size) == false) {
        SDL_Log("set_font_size error..");
        return 0;
    }


    if (TTF_GetStringSize(font->ttf_font, text, 0, width, height) == false) {
        SDL_Log("TTF_sizeUTF8 error: %s", SDL_GetError());
        return 0;
    }

    return 1;
}

int print_font_to_renderer(FONT *font, SDL_Renderer *renderer, const char *text, int size, SDL_Color color, SDL_Point position) {
    int status = 0;
    if (font == NULL || renderer == NULL) {
        SDL_Log("Ne postoji font ili renderer");
        return status;
    }

    if (set_font_size(font, size) == 0) {
        SDL_Log("set_font_size error..");
        return status;
    }

    SDL_Surface *surface = TTF_RenderText_Solid(font->ttf_font, text, 0, color);
    if (surface == NULL) {
        SDL_Log("TTF_RenderText_Solid error: %s", SDL_GetError());
        return status;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        SDL_Log("SDL_CreateTextureFromSurface error: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return status;
    }

    float width, height;
    status = SDL_GetTextureSize(texture, &width, &height);
    if (status == 0) {
        SDL_Log("SDL_GetTextureSize error: %s", SDL_GetError());
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
        return status;
    }
    
    status = SDL_RenderTexture(renderer, texture, NULL, &(SDL_FRect){position.x, position.y, width, height});
    if (status == 0) {
        SDL_Log("SDL_RenderTexture error: %s", SDL_GetError());
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
        return status;
    }

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
    return 1;
}