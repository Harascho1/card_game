#include "menu.h"
#include "font.h"
#include "game.h"

static SDL_Texture *g_start_menu_pic = NULL;
static SDL_Texture *g_start_menu_decorations = NULL;

SDL_Texture* CreateTextureFromImage(SDL_Renderer *renderer, const char *path) {
    if (renderer == NULL && path == NULL) {
        return NULL;
    }

    SDL_Surface *surface = IMG_Load(path);

    if (surface == NULL) {
        SDL_Log("IMG_Load error = %s", SDL_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture == NULL) {
        SDL_Log("SDL_CreateTextureFromSurface error = %s\n", SDL_GetError());
        SDL_DestroySurface(surface);
    }
}

enum {
    title_width = 256,
    title_heigh = 128
};

MENU* create_menu(int count, const char **strings) {
    MENU *menu = NULL;

    menu = SDL_malloc(sizeof(MENU) + sizeof(MENU_ITEM) * count);
    if (menu == NULL) {
        return menu;
    }

    menu->count = count;
    for (int i = 0; i < count; i++) {
        menu->items[i].state = item_state_idle;
        menu->items[i].text = strings[i];
        menu->items[i].type = i;
    }

    menu->items[0].state = item_state_hover;
    menu->active_type = 0;
}

void destroy_menu(MENU *menu) {
    if (menu != NULL) {
        SDL_free(menu);
    }
}

int
start_menu_handle_events(GAME *game, const SDL_Event *event) {
    int status = 0;
    if (event == NULL || game == NULL) {
        status = -1;
        return status;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_UP: {
                if (game->start_menu->active_type > start_menu_start) {
                    game->start_menu->items[game->start_menu->active_type--].state = item_state_idle;
                    game->start_menu->items[game->start_menu->active_type].state = item_state_hover;
                }
            }
            break;
            case SDLK_DOWN: {
                if (game->start_menu->active_type < start_menu_max - 1) {
                    game->start_menu->items[game->start_menu->active_type++].state = item_state_idle;
                    game->start_menu->items[game->start_menu->active_type].state = item_state_hover;
                }
            }
            break;
            case SDLK_RETURN: {
                if (game->start_menu->active_type == start_menu_start) {
                    push_user_event(g_change_scene_event_type, state_gameplay);
                }
                else if (game->start_menu->active_type == start_menu_exit) {
                    SDL_Event quit_event;
                    quit_event.type = SDL_EVENT_QUIT;
                    SDL_PushEvent(&quit_event);
                }
            }
            break;
        default:
            break;
        }
    }
}

int start_menu_update(GAME *game) {
    return 0;
}

int start_menu_render(GAME *game) {
    if (game == NULL) {
        return -1;
    }

    if (g_start_menu_pic == NULL) {
        g_start_menu_pic = CreateTextureFromImage(game->renderer, "../assets/background.png");
        if (g_start_menu_pic == NULL) {
            return -1;
        }
    }

    if (g_start_menu_decorations == NULL) {
        g_start_menu_decorations = CreateTextureFromImage(game->renderer, "../assets/Day/3.png");
        if (g_start_menu_decorations == NULL) {
            return -1;
        }
    }

    SDL_SetRenderViewport(game->renderer, NULL);
    SDL_RenderTexture(game->renderer, g_start_menu_pic, NULL, NULL);
    SDL_RenderTexture(game->renderer, g_start_menu_decorations, NULL, NULL);

    int width, height;
    if (SDL_GetWindowSizeInPixels(game->window, &width, &height) == 0) {
        SDL_Log("SDL_GetWindowSize error: %s\n", SDL_GetError());
        return -1;

    }

    int text_width, text_height;
    get_text_size(game->font, "start", game->field.relative_size, &text_width, NULL);

    int pos_x = (width - text_width) / 2;
    int pos_y = height / 2;

    for (int i = 0; i < game->start_menu->count; i++) {
        if (print_font_to_renderer(
            game->font,
            game->renderer,
            game->start_menu->items[i].text,
            game->field.relative_size,
            (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255},
            (SDL_Point){pos_x, pos_y}
        ) == -1) {
            return -1;
        }
        if (game->start_menu->items[i].state == item_state_hover) {
            if (print_font_to_renderer(
                game->font,
                game->renderer,
                ">",
                game->field.relative_size,
                (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 0},
                (SDL_Point){pos_x - game->field.relative_size, pos_y}
            ) == -1) {
                return -1;
            }
        }

        pos_y += game->field.relative_size;
    }
    return SDL_RenderPresent(game->renderer);

}

SCENE g_menu_scene = {
    .handle_events = start_menu_handle_events,
    .update = start_menu_update,
    .renderer = start_menu_render
};

int
game_over_menu_handle_events(GAME *game, const SDL_Event *event) {
    if (event == NULL || game == NULL) {
        return -1;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_UP: {
                if (game->start_menu->active_type > start_menu_start) {
                    game->start_menu->items[game->start_menu->active_type--].state = item_state_idle;
                    game->start_menu->items[game->start_menu->active_type].state = item_state_hover;
                }
            }
            break;
            case SDLK_DOWN: {
                if (game->start_menu->active_type < start_menu_max - 1) {
                    game->start_menu->items[game->start_menu->active_type++].state = item_state_idle;
                    game->start_menu->items[game->start_menu->active_type].state = item_state_hover;
                }
            }
            break;
            case SDLK_RETURN: {
                if (game->start_menu->active_type == game_over_menu_exit) {
                    push_user_event(g_change_scene_event_type, state_gameplay);
                }
                else if (game->start_menu->active_type == game_over_menu_retry) {
                    // fali push_user_event
                }
            }
            case SDLK_ESCAPE: {
                push_user_event(g_change_scene_event_type, state_menu);
            }
            break;
        default:
            break;
        }
    }

    return 0;
}

int
game_over_menu_update(GAME *game) {
    return 0;
}

int
game_over_menu_render(GAME *game) {
    int status = true;
    if (game == NULL) {
        return false;
    }

    int width, height;
    status = SDL_GetWindowSizeInPixels(game->window, &width, &height);
    if (status == 0) {
        SDL_Log("SDL_GetWindowSize error: %s\n", SDL_GetError());
        return status;
    }
    
    status = SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    if (status == 0) {
        SDL_Log("SDL_SetRenderDrawColor error: %s\n", SDL_GetError());
        return status;
    }

    status = SDL_RenderClear(game->renderer);
    if (status == 0) {
        SDL_Log("SDL_RenderClear error: %s\n", SDL_GetError());
        return status;
    }

    status = SDL_SetRenderViewport(game->renderer, NULL);
    if (status == 0) {
        SDL_Log("SDL_SetRenderViewport error: %s\n", SDL_GetError());
        return status;
    }

    status = SDL_RenderFillRect(game->renderer, NULL);
    if (status == 0) {
        SDL_Log("SDL_FillRect error: %s\n", SDL_GetError());
        return status;
    }

    int text_width, text_height;
    const char* game_over_text = "Game Over";
    status = get_text_size(game->font, game_over_text, (int)game->field.relative_size * 1.5f, &text_width, &text_height);
    if (status == 0) {
        return status;
    }

    status = print_font_to_renderer(
        game->font,
        game->renderer,
        game_over_text,
        (int)game->field.relative_size * 1.5f,
        (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255},
        (SDL_Point){(width - text_width) / 2, (height - text_height) / 2}
    );
    if (status == 0) {
        return status;
    }

    status = SDL_RenderPresent(game->renderer);
    if (status == 0) {
        SDL_Log("SDL_RenderPresent error: %s\n", SDL_GetError());
        return status;
    }

    return status;

}

SCENE g_game_over_scene = {
    .handle_events = game_over_menu_handle_events,
    .update = game_over_menu_update,
    .renderer = game_over_menu_render
};
