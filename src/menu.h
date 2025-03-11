#ifndef MENU_H
#define MENU_H

enum START_MENU_ITEM_TYPE {
    start_menu_start,
    start_menu_exit,
    start_menu_max
};

enum GAME_OVER_MENU_ITEM_TYPE {
    game_over_menu_retry,
    game_over_menu_exit,
    game_over_menu_max
};

enum MENU_ITEM_STATE {
    item_state_idle, //0
    item_state_hover
};

typedef struct _MENU_ITEM {
    const char *text;
    int type;
    int state;
} MENU_ITEM;

typedef struct _MENU {
    int count;
    int active_type;
    MENU_ITEM items[];
} MENU;

MENU* create_menu(int count, const char **strings);
void destroy_menu(MENU* menu);

#endif //MENU_H