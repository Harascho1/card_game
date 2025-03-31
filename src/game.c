#include "game.h"

enum {
    minimum_cell_size = 16,
    field_width_in_cells = 14,
    field_height_in_cells = 10
};

enum {
    card_width = 60,
    card_height = 84
};

Uint32 g_change_scene_event_type = (Uint32)-1;

static const char *g_start_menu_items[] = {
    "create lobby",
    "join lobby",
    "exit"
};

static const char *g_game_over_menu_items[] = {
    "restart",
    "menu"
};

static const char *g_join_lobby_menu_items[] = {
    "join",
    "back"
};

int init_game(GAME *game, const RESOLUTION *resolution) {
    int status = 0;

    game->field.relative_size = 48;
    if (game == NULL) {
        return status;
    }

    game->start_menu = create_menu(3, g_start_menu_items);
    if (game->start_menu == NULL) {
        return status;
    }

    game->game_over_menu = create_menu(2, g_game_over_menu_items);
    if (game->game_over_menu == NULL) {
        return status;
    }

    game->join_lobby_menu = create_menu(2, g_join_lobby_menu_items);
    if (game->join_lobby_menu == NULL) {
        return status;
    }

    game->window = SDL_CreateWindow(
            "Game", 
            resolution->width, 
            resolution->height, 
            0
    );
    if (game->window == NULL) {
        SDL_Log("create_window error: %s\n", SDL_GetError());
        return status;
    }

    game->renderer = SDL_CreateRenderer(game->window, NULL);
    if (game->renderer == NULL) {
        SDL_Log("create_renderer error: %s\n", SDL_GetError());
        return status;
    }

    game->font = create_font("../assets/font.ttf", game->field.relative_size, (SDL_Color){.r = 100, .g = 100, .b = 100, .a = 255});
    if (game->font == NULL) {
        return status;
    }

    game->deck = create_deck();
    if (game->deck == NULL) {
        return status;
    }

    Uint32 change_scene_event_type = SDL_RegisterEvents(1);
    if (change_scene_event_type == (Uint32)-1) {
        exit_game(game);
        return status;
    }

    g_change_scene_event_type = change_scene_event_type;
    return true;

}

void exit_game(GAME *game) {
    if (game != NULL) {
        if (game->renderer) {
            SDL_DestroyRenderer(game->renderer);
        }
        if (game->window) {
            SDL_DestroyWindow(game->window);
        }
        if (game->start_menu) {
            destroy_menu(game->start_menu);
        }
        if (game->game_over_menu) {
            destroy_menu(game->game_over_menu);
        }
        if (game->join_lobby_menu) {
            destroy_menu(game->join_lobby_menu);
        }
        if (game->font) {
            destroy_font(game->font);
        }
    }
}

int
gameplay_handle_events(GAME *game, const SDL_Event *event) {
    int status = 0;
    if (game == NULL && event == NULL) {
        return status;
    }

    if (event->key.key == SDLK_ESCAPE) {
        return push_user_event(g_change_scene_event_type, state_game_over);
    }

    return 0;
}

int
gameplay_update(GAME *game) {
    return 0;
}

int
push_user_event(Uint32 type, Sint32 code) {
    SDL_Event event;
    SDL_memset(&event, 0, sizeof(event));

    printf("code je: %d\n",code);

    event.type = type;
    event.user.code = code;

    int status = SDL_PushEvent(&event);
    if (status != 1) {
        status = -1;
    } else {
        status = 0;
    }
    return status;
}

int
gameplay_render(GAME *game) {
    int status = 0;

    if (game == NULL) {
        return status;
    }

    int width, height;
    status = SDL_GetWindowSizeInPixels(game->window, &width, &height);
    if (status == 0) {
        SDL_Log("SDL_GetWindowSizeInPixels error: %s\n", SDL_GetError());
        return status;
    }
    SDL_Surface *surface = IMG_Load("../assets/background.png");
    if (surface == NULL) {
        SDL_Log("IMG_Load error: %s\n", SDL_GetError());
        return status;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_DestroySurface(surface);
    if (texture == NULL) {
        SDL_Log("texture nije inicializovan\n");
        status = 0;
        return status;
    }

    SDL_RenderTexture(game->renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);

    //* Card size (60, 84) 5x(5, 7)
    HAND hand; 
    hand.count = 3;
    hand.cards = SDL_malloc(sizeof(CARD) * hand.count);
    if (hand.cards == NULL) {
        SDL_Log("SDL_malloc error: %s\n", SDL_GetError());
        return status;
    }
    hand.cards[0] = game->deck->cards[0];
    hand.cards[1] = game->deck->cards[1];
    hand.cards[2] = game->deck->cards[2];
    int rect_width = (width - card_width * hand.count) / 2;
    int rect_height = (height - card_height * 2);

    SDL_FRect rect = {
        .x = rect_width,
        .y = rect_height,
        .w = card_width,
        .h = card_height 
    };
    
    //* Render hand
    status = render_hand(game->renderer, &hand, &rect);
    if (status == 0) {
        SDL_Log("render_card error...\n");
        return status;
    }

    status = SDL_RenderPresent(game->renderer);
    if (status == 0) {
        SDL_Log("SDL_RenderPresent error: %s\n", SDL_GetError());
        return status;
    }
    SDL_free(hand.cards);
    return status;

}

SCENE g_gameplay_scene = {
    .handle_events = gameplay_handle_events,
    .update = gameplay_update,
    .renderer = gameplay_render
};