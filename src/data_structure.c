#include "data_structure.h"
// Initialize players
void new_player(player** head, int num, int d) {
  player* newplayer = calloc((size_t)1, sizeof(player));
  if (newplayer == NULL) {
    printf("Error: out of memory\n");
    exit(0);
  }
  int* newcard = calloc((size_t)(d * 52), sizeof(int));
  if (newcard == NULL) {
    printf("Error: out of memory\n");
    exit(0);
  }
  newplayer->player_card = newcard;
  newplayer->size_p = 0;
  newplayer->number = num;
  newplayer->score = 0;
  newplayer->next = NULL;
  newplayer->previous = NULL;
  if (*head == NULL) {
    newplayer->next = newplayer;
    newplayer->previous = newplayer;
    *head = newplayer;
  } else {
    player* tail = (*head)->previous;
    newplayer->next = *head;
    newplayer->previous = tail;
    tail->next = newplayer;
    (*head)->previous = newplayer;
  }
}
// Initialize the deck
void deck_init(deck** card, int d) {
  (*card)->stock = calloc((size_t)(d * 52), sizeof(int));
  if ((*card)->stock == NULL) {
    printf("Memory allocation failed.\n");
    exit(1);
  }
  (*card)->discard = calloc((size_t)(d * 52), sizeof(int));
  if ((*card)->discard == NULL) {
    printf("Memory allocation failed.\n");
    exit(1);
  }
  (*card)->size_s = d * 52;
  (*card)->size_d = 0;
  (*card)->top = 0;
}
// free players
void free_player(player** head) {
  if (*head == NULL) return;

  player* current = *head;
  do {
    player* temp = current;
    current = current->next;
    free(temp->player_card);
    temp->player_card = NULL;
    free(temp);
  } while (current != *head && current != NULL);

  *head = NULL;
}
// free decks
void free_deck(deck** card) {
  if (*card != NULL) {
    if ((*card)->stock != NULL) {
      free((*card)->stock);
      (*card)->stock = NULL;
    }
    if ((*card)->discard != NULL) {
      free((*card)->discard);
      (*card)->discard = NULL;
    }
    free(*card);
    *card = NULL;
  }
}
// shuffle cards
void shuffle(deck** card) {
  for (int i = (*card)->size_s - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int temp = (*card)->stock[i];
    (*card)->stock[i] = (*card)->stock[j];
    (*card)->stock[j] = temp;
  }
}
// convert number to card
void num2card(int num, char* card_i) {
  if (num < 1 || num > 52) {
    sprintf(card_i, "Invalid Card");
    return;
  }
  const char* suits[] = {"Spade", "Heart", "Diamond", "Club"};
  const char* ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
  int suit = (num - 1) / 13;
  int rank = (num - 1) % 13;
  sprintf(card_i, "%s %s", suits[suit], ranks[rank]);
}
// convert card to number
int card2num(char* str) {
  const char* suits[] = {"Spade", "Heart", "Diamond", "Club"};
  const char* ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
  char suit[10] = {0}, rank[3] = {0};
  // deal with invalid input str
  if (sscanf(str, "%9s %2s", suit, rank) != 2) {
    return 0;
  }

  int suit_index = -1, rank_index = -1;
  for (int i = 0; i < 4; i++) {
    if (strcmp(suits[i], suit) == 0) {
      suit_index = i;
      break;
    }
  }
  for (int i = 0; i < 13; i++) {
    if (strcmp(ranks[i], rank) == 0) {
      rank_index = i;
      break;
    }
  }
  // deal with invalid input str
  if (suit_index == -1 || rank_index == -1) {
    return 0;
  }

  return suit_index * 13 + rank_index + 1;
}
// quick sort-compare
int compare(const void* a, const void* b) {
  int num1 = *(int*)a;
  int num2 = *(int*)b;

  if (num1 < num2) {
    return -1;
  } else if (num1 > num2) {
    return 1;
  } else {
    return 0;
  }
}
// delete index [n] card in player deck
void delete_player_card(player** current, int player_card_index) {
  for (int i = player_card_index; i < (*current)->size_p - 1; i++) (*current)->player_card[i] = (*current)->player_card[i + 1];
  (*current)->player_card[(*current)->size_p - 1] = 0;
  (*current)->size_p--;
}
// delete index [n] card in player deck, then put it into discard pile
void discard_exchange(player** current, deck** card, int player_card_index, int next_card) {
  (*card)->discard[(*card)->size_d] = next_card;
  (*card)->size_d++;
  delete_player_card(current, player_card_index);
}
// pointer to the next player
void next_player(player** current, int is_clockwise) {
  if (is_clockwise == 1)
    *current = (*current)->next;
  else
    *current = (*current)->previous;
}
// put discarded cards into stock pile then shuffle them
void reshuffle(deck** card, int size_0) {
  (*card)->stock = realloc((*card)->stock, (size_t)size_0 * sizeof(int));
  if ((*card)->stock == NULL) printf("Reshuffle memory allocation failed.\n");

  for (int i = 0; i < size_0; i++) {
    (*card)->stock[i] = (*card)->discard[i];
    (*card)->discard[i] = 0;
  }
  (*card)->size_d = 0;
  for (int i = 0; i < size_0; i++) {
    if ((*card)->stock[i] == 0) {
      (*card)->size_s = i;
      break;
    }
  }
  (*card)->top = 0;
  (*card)->stock = realloc((*card)->stock, (size_t)(*card)->size_s * sizeof(int));
  if ((*card)->stock != NULL) {
    // shuffle cards again
    shuffle(card);
  } else {
    printf("Reshuffle memory allocation failed.\n");
  }
}

// check whether a player has played all his or her cards
int check_size_p(player** head) {
  player* current = *head;
  int end = 0;
  do {
    if (current->size_p == 0) {
      end = 1;
      break;
    }
    current = current->next;
  } while (current->number != 1);
  return end;
}
// find the index of a card
int find_index(player** current, int next_card) {
  int player_card_index = -1;
  for (int i = 0; i < (*current)->size_p; i++) {
    if (next_card == (*current)->player_card[i]) {
      player_card_index = i;
      break;
    }
  }
  return player_card_index;
}
// print player's cards
void show_card(player** current, FILE* fd, int times) {
  if ((*current)->size_p == 0) {
    printf("Player %d have played all cards!", (*current)->number);
    if (times == 1) {
      fprintf(fd, "Player %d have played all cards!", (*current)->number);
    }
  } else {
    qsort((*current)->player_card, (size_t)(*current)->size_p, sizeof(int), compare);  // quick sort
    char card_i[20];
    printf("Player %d cards: ", (*current)->number);
    if (times == 1) {
      fprintf(fd, "Player %d cards: ", (*current)->number);
    }
    for (int i = 0; i < (*current)->size_p; i++) {
      memset(card_i, 0, (size_t)20 * sizeof(char));
      num2card((*current)->player_card[i], card_i);
      printf("%s", card_i);
      if (times == 1) {
        fprintf(fd, "%s", card_i);
      }
      if (i < (*current)->size_p - 1) {
        printf(", ");
      }
      if (times == 1) {
        if (((*current)->player_card[i] >= 27 && (*current)->player_card[i] <= 39) || (*current)->player_card[i] == 9 || (*current)->player_card[i] == 22)
          fprintf(fd, "\t");
        else
          fprintf(fd, "\t\t");
      }
    }
    printf("\n");
    if (times == 1) {
      fprintf(fd, "\n");
    }
  }
}
// print cards in stock pile
void show_card_deck(deck* card) {
  char card_i[20];
  for (int i = 0; i < card->size_s; i++) {
    memset(card_i, 0, (size_t)20 * sizeof(char));
    num2card(card->stock[i], card_i);
    printf("%s", card_i);
    if (i < card->size_s - 1) printf(", ");
  }
}
// print cards in stock pile(in log file)
void show_card_deck_log(deck* card, FILE* fd) {
  fprintf(fd, "Shuffling results:");
  char card_i[20];
  for (int i = 0; i < card->size_s; i++) {
    memset(card_i, 0, (size_t)20 * sizeof(char));
    num2card(card->stock[i], card_i);
    if (i % 10 == 0) fprintf(fd, "\n");
    fprintf(fd, "%s", card_i);
    if ((card->stock[i] >= 27 && card->stock[i] <= 39) || card->stock[i] == 9 || card->stock[i] == 22)
      fprintf(fd, "\t");
    else
      fprintf(fd, "\t\t");
  }
}
// clean the screen
void clear_screen(void) {
#if defined(_WIN32) || defined(_WIN64)
  // Windows system
  int value = system("cls");
#else
  // Unix-like system
  int value = system("clear");
#endif

  if (value != 0) printf("Screen clean failed.\n");
}
// pause the program
void pause_t(void) {
  printf("Press Enter to continue...\n");
  getchar();
}
