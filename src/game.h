#ifndef GAME_H
#define GAME_H

#include "font.h"
#include "menu.h"
#include "card.h"
#include "connection.h"

extern Uint32 g_change_scene_event_type;

typedef enum _GAME_STATE {
    state_menu = 0,
    state_gameplay,
    state_join_lobby,
    state_game_over,
    state_waiting_lobby,

} GAME_STATE;

typedef struct _GAME_FIELD {
    int relative_size;
    int width;
    int height;
} GAME_FIELD;

typedef struct _RESOLUTION {
    int width;
    int height;
} RESOLUTION;

typedef struct _GAME {
    SDL_Renderer    *renderer;
    SDL_Window      *window;
    FONT            *font;
    MENU            *start_menu;
    MENU            *game_over_menu;
    MENU            *join_lobby_menu;
    MENU            *waiting_menu;
    CONNECTION      *connection;
    DECK            *deck;
    GAME_FIELD      field;

} GAME;

typedef int (*SCENE_HANDLE_EVENT)(
    GAME*           game,
    const SDL_Event *event
);
typedef int (*SCENE_UPDATE)(GAME *game);
typedef int (*SCENE_RENDER)(GAME* game);

typedef struct _SCENE {
    SCENE_HANDLE_EVENT  handle_events;
    SCENE_UPDATE        update;
    SCENE_RENDER        renderer;
} SCENE;

extern SCENE g_menu_scene;
extern SCENE g_gameplay_scene;
extern SCENE g_game_over_scene;
extern SCENE g_join_lobby_scene;
extern SCENE g_waiting_scene;

int init_game(GAME *game, const RESOLUTION *resolution);
void exit_game(GAME *game);

int
push_user_event(Uint32 type, Sint32 code);

#endif //GAME_H