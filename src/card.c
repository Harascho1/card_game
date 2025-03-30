#include "card.h"

void shuffle_deck(DECK *deck) {
    if (deck == NULL) {
        SDL_Log("Deck is NULL and therefore cannot be shuffled");
        return;
    }
    for (int i = 0; i < deck->count; i++) {
        int j = rand() % deck->count;
        CARD temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
}

DECK* create_deck() {
    DECK *deck = NULL;
    deck = SDL_malloc(sizeof(DECK));
    if (deck == NULL) {
        return deck;
    }
    deck->count = 52;
    int i = 0;
    for (int suit = suit_clubs; suit <= suit_spades; suit++) {
        for (int value = value_ace; value <= value_king; value++) {
            deck->cards[i].suit = suit;
            deck->cards[i].value = value;
            i++;
        }
    }

    shuffle_deck(deck);
    return deck;
}

void destroy_deck(DECK *deck) {
    if (deck != NULL) {
        SDL_free(deck);
    }
}

char* find_path(CARD *card) {
    char suit[10];
    switch (card->suit) {
        case suit_clubs:
            strncpy(suit, "clubs", 6);
            break;
        case suit_diamonds:
            strncpy(suit, "diamonds", 9);
            break;
        case suit_hearts:
            strncpy(suit, "hearts", 7);
            break;
        case suit_spades:
            strncpy(suit, "spades", 7);
            break;
        default:
            SDL_Log("Unknown suit");
            return NULL;
    }

    char value[5];
    switch (card->value)
    {
        case 1:
            strncpy(value, "ace", 3);
            break;
        case 11:
            strncpy(value, "jack", 4);
            break;
        case 12:
            strncpy(value, "queen", 5);
            break;
        case 13:
            strncpy(value, "king", 4);
            break;
        default:
            SDL_itoa(card->value, value, 10);
            break;
    }

    char *path;
    path = SDL_malloc(sizeof(char) * 50);
    sprintf(path, "../assets/cards/%s_%s_white.png", value, suit);

    return path;
}

int render_hand(SDL_Renderer *renderer, HAND *hand, SDL_FRect *rect) {
    int status = 0;

    if (renderer == NULL || hand == NULL) {
        SDL_Log("Renderer or card is NULL");
        return status;
    }
    
    SDL_Surface *surface;
    SDL_Texture *texture;
    char *path;
    for (int i = 0; i < hand->count; i++) {
        path = find_path(&hand->cards[i]);
        if (path == NULL) {
            SDL_Log("Path is NULL");
            return status;
        }
        surface = IMG_Load(path);
        if (surface == NULL) {
            SDL_Log("IMG_Load error: %s", SDL_GetError());
            return status;
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == NULL) {
            SDL_Log("SDL_CreateTextureFromSurface error: %s", SDL_GetError());
            SDL_DestroySurface(surface);
            return status;
        }
        status = SDL_RenderTexture(renderer, texture, NULL, rect);
        if (status == 0) {
            SDL_Log("SDL_RenderTexture error: %s", SDL_GetError());
            return status;
        }
        rect->x += 60;
    }
    SDL_free(path);
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
    return status;
}