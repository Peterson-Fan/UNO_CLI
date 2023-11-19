#ifndef GAME
#define GAME
// Layer 2

#include "data_structure.h"

void deal_card(player** head, deck** card, int c);
int play_order(int** candidate, deck** card, int n, FILE* fd);
void show_all_card(player** head, int demo, FILE* fd);
void auto_detect(player** current, const int* current_card, int* attack_sum);
int winner(player** head);
void print_score(player** head, FILE* fd);
void calculate_scores(player** head, FILE* fd);
void sort_players_by_score(player** head);
void print_leaderboard(player** head, FILE* fd);
void find_winners_or_tie(player** head, FILE* fd);
void reset_player_and_deck(player** head, deck** card, int size_0);

#endif
