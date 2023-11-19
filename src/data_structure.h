#ifndef DATA_STRUCTURE
#define DATA_STRUCTURE
// Layer 1

#include <getopt.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// data structures
typedef struct _player {
  int number;                // player number, e.g. 1, 2...
  int score;                 // player score
  int* player_card;          // player card array
  int size_p;                // size of player card
  struct _player* next;      // pointer to the next player
  struct _player* previous;  // pointer to the previous player
} player;

typedef struct _deck {
  int* stock;    // stock pile array
  int* discard;  // discard pile array
  int size_s;    // size of the stock pile
  int size_d;    // size of the discard pile
  int top;       // top of the stock pile
} deck;

// functions
void new_player(player** head, int num, int d);
void free_player(player** head);
void free_deck(deck** card);
void delete_player_card(player** current, int player_card_index);
void discard_exchange(player** current, deck** card, int player_card_index, int next_card);
int check_size_p(player** head);
int find_index(player** current, int next_card);

void deck_init(deck** card, int d);
void num2card(int num, char* card_i);
int card2num(char* str);
void reshuffle(deck** card, int size_0);

void shuffle(deck** card);
int compare(const void* a, const void* b);
void next_player(player** current, int is_clockwise);

void show_card_deck(deck* card);
void show_card_deck_log(deck* card, FILE* fd);
void show_card(player** current, FILE* fd, int times);

void clear_screen(void);
void pause_t(void);

#endif
