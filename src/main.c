#include "game.h"
#include "menu.h"
#include "font.h"

bool g_input_handled = false;

static SCENE* 
g_scene[] = {
    &g_menu_scene,
    &g_gameplay_scene,
    &g_join_lobby_scene,
    &g_game_over_scene
};

static GAME_STATE g_current_scene = state_menu;

static int
sdl_init() {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 0) {
        SDL_Log("SDL_InitSubSystem *VIDEO error: %s", SDL_GetError());
        return 0;
    }
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Log("SDL_Init *VIDEO error: %s", SDL_GetError());
        return 0;
    }
    if (SDL_Init(SDL_INIT_EVENTS) == 0) {
        SDL_Log("SDL_Init *EVENTS error: %s", SDL_GetError());
        return 0;
    }
    return 1;
}

static bool
is_period_passed(int period, int last_update_time) {
    int current_time = SDL_GetTicks();
    if (current_time - last_update_time >= period) {
        return true;
    }
    return false;
}

int
main(int argc, const char * argv[]) {

    if (sdl_init() == 0) {
        SDL_Quit();
        return -1;
    }
    
    GAME game;
    if (init_game(&game, &(RESOLUTION){800, 600}) == 0) {
        exit_game(&game);
        SDL_Quit();
    }


    SDL_Event event;
    bool is_running = true;
    int last_frame_time = 0;

    while (is_running) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            } else if (event.type == g_change_scene_event_type) {
                g_current_scene = event.user.code;
                if (g_current_scene == state_game_over) {
                }
            }
        }
        g_scene[g_current_scene]->handle_events(&game, &event);
        if (is_period_passed(1000/60, last_frame_time)) {
            g_scene[g_current_scene]->update(&game);
            g_scene[g_current_scene]->renderer(&game);
            g_input_handled = false;
            last_frame_time = SDL_GetTicks();
        }
    }

    exit_game(&game);
    SDL_Quit();
    return 0;
}