#ifndef INTERACTION
#define INTERACTION
// Layer 3

#include "game.h"

void game(int n, int c, int d, int r, int demo, FILE* fd);

void game_play(player** head, deck** card, int first, int demo, FILE* fd);
int play_card(player** current, deck** card, int* current_card, int* attack_sum, FILE* fd);
int play_card_demo(player** current, deck** card, int* current_card, int* attack_sum, FILE* fd);

void draw_card(player** current, deck** card, int* attack_sum, int size_0, int demo, FILE* fd);

#endif
