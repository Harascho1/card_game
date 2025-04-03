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
                    // TODO - napravi connection
                    SDL_StartTextInput(game->window);
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
                if (game->game_over_menu->active_type < game_over_menu_max - 1) {
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
            break;
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
                (SDL_Point){(width - text_retry_width) / 2 - game->field.relative_size, height / 2 + text_retry_height}
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


int validation_ip_and_port(CONNECTION *connection) {
    if (connection == NULL) {
        return 0;
    }

    if (connection->ip_addr.buffer == NULL || connection->port.buffer == NULL) {
        return 0;
    }

    if (strlen(connection->ip_addr.buffer) == 0 || strlen(connection->port.buffer) == 0) {
        return 0;
    }

    int port = atoi(connection->port.buffer);
    if (port < 1 || port > 65535) {
        return 0;
    }

    return 1;
}


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
            case SDLK_TAB: {
                if (game->connection->ip_addr.status == SELECTED) {
                    game->connection->ip_addr.status = NOT_SELECTED;
                    game->connection->port.status = SELECTED;
                } else {
                    game->connection->ip_addr.status = SELECTED;
                    game->connection->port.status = NOT_SELECTED;
                }
            }
            break;
            case SDLK_BACKSPACE: {
                if (game->connection->ip_addr.status == SELECTED) {
                    if (strlen(game->connection->ip_addr.buffer) > 0) {
                        game->connection->ip_addr.buffer[strlen(game->connection->ip_addr.buffer) - 1] = '\0';
                    }
                } else if (game->connection->port.status == SELECTED) {
                    if (strlen(game->connection->port.buffer) > 0) {
                        game->connection->port.buffer[strlen(game->connection->port.buffer) - 1] = '\0';
                    }
                }

            }
            break;
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
                    if (validation_ip_and_port(game->connection) == 0) {
                        SDL_Log("Invalid ip or port\n");
                        break;
                    }
                    pthread_t thread;
                    pthread_create(&thread, NULL, (void*)connect_to_server, game->connection);
                    pthread_detach(thread);
                    push_user_event(g_change_scene_event_type, state_waiting_lobby);
                }
                else if (game->join_lobby_menu->active_type == join_lobby_back) {
                    push_user_event(g_change_scene_event_type, state_menu);
                    
                }
            }
            break;
            case SDLK_ESCAPE: {
                SDL_StopTextInput(game->window);
                memset(game->connection->ip_addr.buffer, 0, game->connection->ip_addr.max_length);
                memset(game->connection->port.buffer, 0, game->connection->port.max_length);
                push_user_event(g_change_scene_event_type, state_menu);
            }
            break;
        default:
            break;
        }
    } else if (event->type == SDL_EVENT_TEXT_INPUT) {
        if (strlen(game->connection->ip_addr.buffer) < game->connection->ip_addr.max_length && game->connection->ip_addr.status == SELECTED) {
            strcat(game->connection->ip_addr.buffer, event->text.text);
            // prov radimo samo da li radi upis ip_addrese
        } else if (strlen(game->connection->port.buffer) < game->connection->port.max_length && game->connection->port.status == SELECTED) {
            strcat(game->connection->port.buffer, event->text.text);
        }
    }

    return status;
}

int
join_lobby_update(GAME *game) {
    return 0;
}

TEXT_BOX text_boxes[2];

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


    int text_width = 0, text_height;
    status = get_text_size(game->font, game->join_lobby_menu->items[0].text, game->field.relative_size, &text_width, &text_height);
    if (status == 0) {
        return 0;
    }

    status = print_font_to_renderer(
        game->font,
        game->renderer,
        game->join_lobby_menu->items[0].text,
        game->field.relative_size,
        (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255},
        (SDL_Point){(width - text_width) / 2, (height / 10 - text_height)}
    );
    if (status == 0) {
        return 0;
    }

    SDL_Texture *backgroung_text = CreateTextureFromImage(game->renderer, "../assets/Day/1.png");
    if (backgroung_text == NULL) {
        SDL_Log("Error CreateTextureFromImage\n");
        status = 0;
        return status;
    }

    SDL_Surface *rect_surface = SDL_CreateSurface(
        game->connection->ip_addr.rect.w,
        game->connection->ip_addr.rect.h,
        SDL_PIXELFORMAT_RGBA8888
    );
    const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888);
    if (format == NULL) {
        SDL_Log("SDL_AllocFormat error: %s\n", SDL_GetError());
        return 0;
    }
    Uint32 color = SDL_MapRGBA(format, 0, 0, 255, 255, 255);
    if (rect_surface == NULL) {
        SDL_Log("SDL_CreateSurface error: %s\n", SDL_GetError());
        return 0;
    }
    SDL_Rect rect = {
        .x = 0,
        .y = 0,
        //.y = height / 4  + text_height * 2 + game->field.relative_size - 5,
        .w = game->connection->ip_addr.rect.w + 10,
        .h = game->connection->ip_addr.rect.h + 10
    };
    SDL_FillSurfaceRect(rect_surface, &rect, color);
    SDL_Texture *rect_texture = SDL_CreateTextureFromSurface(game->renderer, rect_surface);
    if (rect_texture == NULL) {
        SDL_Log("SDL_CreateTextureFromSurface error: %s\n", SDL_GetError());
        return 0;
    }

    if (game->connection->ip_addr.status == SELECTED) {

        SDL_FRect frect = {
            .x = game->connection->ip_addr.rect.x - 5,
            .y = game->connection->ip_addr.rect.y - 5,
            .w = rect.w,
            .h = rect.h
        };
        status = SDL_RenderTexture(game->renderer, rect_texture, NULL, &frect);
        if (status == 0) {
            SDL_Log("SDL_RenderText error %s\n", SDL_GetError());
            return 0;
        }

    } else if (game->connection->port.status == SELECTED) {

        SDL_FRect frect = {
            .x = game->connection->port.rect.x - 5,
            .y = game->connection->port.rect.y - 5,
            .w = rect.w,
            .h = rect.h
        };
        status = SDL_RenderTexture(game->renderer, rect_texture, NULL, &frect);
        if (status == 0) {
            SDL_Log("SDL_RenderText error %s\n", SDL_GetError());
            return 0;
        }

    }

    SDL_DestroySurface(rect_surface);
    SDL_DestroyTexture(rect_texture);

    status = SDL_RenderTexture(game->renderer, backgroung_text, NULL, &game->connection->ip_addr.rect);
    if (status == 0) {
        SDL_Log("SDL_RenderText error %s\n", SDL_GetError());
        return status;
    }

    status = SDL_RenderTexture(game->renderer, backgroung_text, NULL, &game->connection->port.rect);
    if (status == 0) {
        SDL_Log("SDL_RenderText error %s\n", SDL_GetError());
        return status;
    }

    for (int i = 1; i < 3; i++) {
        int text_width = 0, text_height;
        status = get_text_size(game->font, game->join_lobby_menu->items[i].text, game->field.relative_size, &text_width, &text_height);
        if (status == 0) {
            return 0;
        }

        status = print_font_to_renderer(
            game->font,
            game->renderer,
            game->join_lobby_menu->items[i].text,
            game->field.relative_size,
            (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255},
            (SDL_Point){width / 10, (height / 4 + (text_height * 2 * i))}
        );
        if (status == 0) {
            return 0;
        }

    }
    //ip addr
    if (game->connection->ip_addr.buffer[0] != '\0')  {
        status = print_font_to_renderer(
            game->font,
            game->renderer,
            game->connection->ip_addr.buffer,
            game->field.relative_size,
            (SDL_Color){.r = 0, .g = 255, .b = 255, .a = 255},
            (SDL_Point){width / 10 + 5, (height / 4 + (text_height * 2 * 1)) + game->field.relative_size}
        );
        if (status == 0) {
            SDL_Log("ip addr: %s\n", game->connection->ip_addr.buffer);
            return 0;
        }
    }
    //port
    if (game->connection->port.buffer[0] != '\0')  {
        status = print_font_to_renderer(
            game->font,
            game->renderer,
            game->connection->port.buffer,
            game->field.relative_size,
            (SDL_Color){.r = 0, .g = 255, .b = 255, .a = 255},
            (SDL_Point){width / 10 + 5, (height / 4 + (text_height * 2 * 2)) + game->field.relative_size}
        );
        if (status == 0) {
            SDL_Log("port: %s\n", game->connection->port.buffer);
            return 0;
        }
    }

    status = get_text_size(game->font, game->join_lobby_menu->items[3].text, game->field.relative_size, &text_width, &text_height);
    if (status == 0) {
        return 0;
    }

    for (int i = 3; i < 5; i++) {

        status = print_font_to_renderer(
            game->font,
            game->renderer,
            game->join_lobby_menu->items[i].text,
            game->field.relative_size,
            (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255},
            (SDL_Point){(width - text_width) / 2, (height - text_height - (i - 3) * game->field.relative_size - game->field.relative_size)}
        );
        if (status == 0) {
            return 0;
        }
    }

    SDL_DestroyTexture(backgroung_text);
    
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

int
waiting_menu_handle_events(GAME *game, const SDL_Event *event) {
    if (event == NULL || game == NULL) {
        return -1;
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
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
waiting_menu_update(GAME *game) {
    return 0;
}

int
waiting_menu_render(GAME *game) {
    int status;

    if (game == NULL) {
        return 0;
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

SCENE g_waiting_scene = {
    .handle_events = waiting_menu_handle_events,
    .update = waiting_menu_update,
    .renderer = waiting_menu_render
};
