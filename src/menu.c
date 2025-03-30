#include "menu.h"
#include "font.h"
#include "game.h"

static SDL_Texture *g_background = NULL;

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
                if (game->start_menu->active_type > start_menu_create_lobby) {
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
                if (game->start_menu->active_type == start_menu_create_lobby) {
                    push_user_event(g_change_scene_event_type, state_gameplay);
                }
                if (game->start_menu->active_type == start_menu_join_lobby) {
                    push_user_event(g_change_scene_event_type, state_join_lobby);
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
    int status = 0;

    if (game == NULL) {
        return 0;
    }

    if (g_background == NULL) {
        g_background = CreateTextureFromImage(game->renderer, "../assets/background.png");
        if (g_background == NULL) {
            return 0;
        }
    }


    SDL_SetRenderViewport(game->renderer, NULL);
    SDL_RenderTexture(game->renderer, g_background, NULL, NULL);

    int width, height;
    status = SDL_GetWindowSizeInPixels(game->window, &width, &height);
    if (status == 0) {
        SDL_Log("SDL_GetWindowSize error: %s\n", SDL_GetError());
        return status;
    }

    char *author = "Harascho1";
    int author_width;
    status = get_text_size(game->font, author, game->field.relative_size, &author_width, NULL);
    if (status == 0) {
        return status;
    }
    if (print_font_to_renderer(
        game->font,
        game->renderer,
        author,
        game->field.relative_size,
        (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255},
        (SDL_Point){width - author_width, height - game->field.relative_size}
    ) == 0) {
        status = 0;
        return status;
    }

    char *title = "baronisanje";
    int title_width;
    status = get_text_size(game->font, title, game->field.relative_size * 2, &title_width, NULL);
    if (status == 0) {
        return status;
    }
    if (print_font_to_renderer(
        game->font,
        game->renderer,
        title,
        game->field.relative_size * 2,
        (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255},
        (SDL_Point){(width - title_width) / 2, height / 4}
    ) == 0) {
        status = 0;
        return status;
    }

    int text_width, text_height;
    get_text_size(game->font, "create lobby", game->field.relative_size, &text_width, NULL);

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

//* GAME OVER SCENE
int
game_over_menu_handle_events(GAME *game, const SDL_Event *event) {
    if (event == NULL || game == NULL) {
        return -1;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_UP: {
                if (game->game_over_menu->active_type > game_over_menu_retry) {
                    game->game_over_menu->items[game->game_over_menu->active_type--].state = item_state_idle;
                    game->game_over_menu->items[game->game_over_menu->active_type].state = item_state_hover;
                }
            }
            break;
            case SDLK_DOWN: {
                if (game->game_over_menu->active_type < start_menu_max - 1) {
                    game->game_over_menu->items[game->game_over_menu->active_type++].state = item_state_idle;
                    game->game_over_menu->items[game->game_over_menu->active_type].state = item_state_hover;
                }
            }
            break;
            case SDLK_RETURN: {
                if (game->game_over_menu->active_type == game_over_menu_exit) {
                    push_user_event(g_change_scene_event_type, state_menu);
                }
                else if (game->game_over_menu->active_type == game_over_menu_retry) {
                    push_user_event(g_change_scene_event_type, state_gameplay);
                    
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

    return true;
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

    status = SDL_SetRenderViewport(game->renderer, NULL);
    if (status == 0) {
        SDL_Log("SDL_SetRenderViewport error: %s\n", SDL_GetError());
        return status;
    }
    
    status = SDL_RenderTexture(game->renderer, g_background, NULL, NULL);
    if (status == 0) {
        SDL_Log("SDL_RenderTexture error: %s\n", SDL_GetError());
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
        (SDL_Point){(width - text_width) / 2, height / 4}
    );
    if (status == 0) {
        return status;
    }

    int text_retry_width, text_retry_height;
    const char* retry_text = "restart";
    status = get_text_size(game->font, retry_text, (int)game->field.relative_size, &text_retry_width, &text_retry_height);
    if (status == 0) {
        return status;
    }

    for (int i = 0; i < game->game_over_menu->count; i++) {
        status = print_font_to_renderer(
            game->font,
            game->renderer,
            game->game_over_menu->items[i].text,
            (int)game->field.relative_size,
            (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255},
            (SDL_Point){(width - text_retry_width) / 2, height / 2 + text_retry_height}
        );
        if (status == 0) {
            return status;
        }
        if (game->game_over_menu->items[i].state == item_state_hover) {
            status = print_font_to_renderer(
                game->font,
                game->renderer,
                ">",
                (int)game->field.relative_size,
                (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255},
                (SDL_Point){(width - game->field.relative_size - text_retry_width) / 2, height / 2 + text_retry_height}
            );
            if (status == 0) {
                return 0;
            }
        }
        text_retry_height += game->field.relative_size;

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

// * JOIN LOBBY SCENE
int
join_lobby_handle_events(GAME *game, const SDL_Event *event) {

    int status;
    if (game == NULL || event == NULL) {
        status = false;
        return status;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_UP: {
                if (game->join_lobby_menu->active_type > game_over_menu_retry) {
                    game->join_lobby_menu->items[game->join_lobby_menu->active_type--].state = item_state_idle;
                    game->join_lobby_menu->items[game->join_lobby_menu->active_type].state = item_state_hover;
                }
            }
            break;
            case SDLK_DOWN: {
                if (game->join_lobby_menu->active_type < start_menu_max - 1) {
                    game->join_lobby_menu->items[game->join_lobby_menu->active_type++].state = item_state_idle;
                    game->join_lobby_menu->items[game->join_lobby_menu->active_type].state = item_state_hover;
                }
            }
            break;
            case SDLK_RETURN: {
                if (game->join_lobby_menu->active_type == join_lobby_join) {
                    push_user_event(g_change_scene_event_type, state_gameplay);
                }
                else if (game->join_lobby_menu->active_type == join_lobby_back) {
                    push_user_event(g_change_scene_event_type, state_menu);
                    
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

    return status;
}

int
join_lobby_update(GAME *game) {
    return 0;
}

int
join_lobby_render(GAME *game) {

    int status;

    if (game == NULL) {
        status = false;
        return status;
    }

    int width, height;
    status = SDL_GetWindowSizeInPixels(game->window, &width, &height);
    if (status == 0) {
        SDL_Log("SDL_GetWindowSize error: %s\n", SDL_GetError());
        return status;
    }

    status = SDL_RenderTexture(game->renderer, g_background, NULL, NULL);
    if (status == 0) {
        SDL_Log("SDL_RenderTexture error: %s\n", SDL_GetError());
        return status;
    }

    status = SDL_RenderPresent(game->renderer);
    if (status == 0) {
        SDL_Log("SDL_RenderPresent error: %s\n", SDL_GetError());
        return status;
    }

    return status;
}

SCENE g_join_lobby_scene = {
    .handle_events = join_lobby_handle_events,
    .update = join_lobby_update,
    .renderer = join_lobby_render
};
