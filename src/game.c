#include "game.h"

// deal cards to every player
void deal_card(player** head, deck** card, int c) {
  player* current = *head;
  do {
    for (int i = 0; i < c; i++) {
      if ((*card)->top >= (*card)->size_s) {
        reshuffle(card, (*card)->size_s + 1);
      }
      current->player_card[i] = (*card)->stock[i + (*card)->top];
    }
    current->size_p = c;
    (*card)->top += c;
    current = current->next;
  } while (current->number != (*head)->number);
}

// decide the first player
int play_order(int** candidate, deck** card, int n, FILE* fd) {
  const int max_attempts = 10;
  int attempt = 0;

  while (attempt < max_attempts) {
    int* canaidate_card = calloc((size_t)n, sizeof(int));
    if (canaidate_card == NULL) {
      printf("Error: out of memory\n");
      exit(0);
    }

    char card_i[20];
    for (int i = 0; i < n; i++) {
      if ((*candidate)[i] == 1) {
        if ((*card)->top >= (*card)->size_s) {
          reshuffle(card, (*card)->size_s + 1);
        }
        canaidate_card[i] = (*card)->stock[i + (*card)->top];
        (*card)->discard[(*card)->size_d] = canaidate_card[i];
        (*card)->top++;
        (*card)->size_d++;
        num2card(canaidate_card[i], card_i);
        printf("Player %d: %s\n", i + 1, card_i);
        fprintf(fd, "Player %d: %s\n", i + 1, card_i);
      } else if ((*candidate)[i] == 0) {
        canaidate_card[i] = 14;
      }
    }
    // select the lowest rank
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (canaidate_card[i] > canaidate_card[j]) (*candidate)[i] = 0;
      }
    }

    // find the first player
    int count = 0, first = 0;
    for (int i = 0; i < n; i++) {
      if ((*candidate)[i] == 1) {
        count++;
        first = i + 1;
      }
    }
    free(canaidate_card);

    if (count == 1) {
      return first;
    } else {
      printf("\nThere exists same ranks! Another extra card round again.\n\n");
      fprintf(fd, "\nThere exists same ranks! Another extra card round again.\n\n");

      // reset candidate
      for (int i = 0; i < n; i++) {
        (*candidate)[i] = 1;
      }

      attempt++;
    }
  }
  return rand() % n + 1;
}

// help player know what cards can be played
void auto_detect(player** current, const int* current_card, int* attack_sum) {
  if (*current_card == 0) {
    printf("You can play: ");
    char card_i[20];
    for (int i = 0; i < (*current)->size_p; i++) {
      memset(card_i, 0, (size_t)20 * sizeof(char));
      num2card((*current)->player_card[i], card_i);
      printf("%s", card_i);
      if (i < (*current)->size_p - 1) printf(", ");
    }
    printf("\n");
  } else if (*attack_sum == 0) {
    int count = 0;
    printf("You can play: ");
    for (int i = 0; i < (*current)->size_p; i++) {
      int next_card = (*current)->player_card[i];
      int current_card_suit = (*current_card - 1) / 13 + 1;
      int current_card_rank = (*current_card - 1) % 13 + 1;
      int next_card_suit = (next_card - 1) / 13 + 1;
      int next_card_rank = (next_card - 1) % 13 + 1;
      char card_i[20] = {0};
      num2card(next_card, card_i);
      if (next_card_rank == 10 || next_card_rank == 11 || next_card_suit == current_card_suit || next_card_rank == current_card_rank) {
        if (count == 0) {
          printf("%s", card_i);
        } else {
          printf(", %s", card_i);
        }
        count++;
      }
    }
    if (count == 0) printf("None. Type \"draw\" to draw a card.");
  } else {
    int count = 0;
    printf("You can play: ");
    for (int i = 0; i < (*current)->size_p; i++) {
      int next_card = (*current)->player_card[i];
      int next_card_rank = (next_card - 1) % 13 + 1;
      char card_i[20] = {0};
      num2card(next_card, card_i);
      if (next_card_rank == 1 || next_card_rank == 2 || next_card_rank == 6 || next_card_rank == 10 || next_card_rank == 11) {
        if (count == 0) {
          printf("%s", card_i);
        } else {
          printf(", %s", card_i);
        }
        count++;
      }
    }
    if (count == 0) printf("None. Type \"draw\" to draw %d cards.", *attack_sum);
  }

  printf("\n");
}

// detect who is the winner
int winner(player** head) {
  int winner_number = 0;
  player* current = *head;
  while (winner_number == 0) {
    if (current->size_p == 0) winner_number = current->number;
    current = current->next;
  }
  return winner_number;
}

// get the score and print it out
void print_score(player** head, FILE* fd) {
  player* current = *head;
  do {
    printf("Player %d: %d, ", current->number, -current->size_p);
    fprintf(fd, "Player %d: %d,\t ", current->number, -current->size_p);
    current->score -= current->size_p;
    printf("total: %d\n", current->score);
    fprintf(fd, "total: %d\n", current->score);
    current = current->next;
  } while (current != *head);
}

// calcualte the score and print the leaderboard and winner when the game ends
void calculate_scores(player** head, FILE* fd) {
  sort_players_by_score(head);
  print_leaderboard(head, fd);
  find_winners_or_tie(head, fd);
}

// sort players by his or her score
void sort_players_by_score(player** head) {
  if (*head == NULL || (*head)->next == *head) return;

  int swapped = 0;
  do {
    swapped = 0;
    player* current = *head;

    while (current->next != *head) {
      if (current->score < current->next->score) {
        // swap
        int temp_score = current->score;
        current->score = current->next->score;
        current->next->score = temp_score;

        int temp_number = current->number;
        current->number = current->next->number;
        current->next->number = temp_number;

        swapped = 1;
      }
      current = current->next;
    }
  } while (swapped);
}

// print leaderboard
void print_leaderboard(player** head, FILE* fd) {
  printf("\n----Leaderboard----\n");
  fprintf(fd, "\n----Leaderboard----\n");
  player* current = *head;
  int rank = 1;
  do {
    printf("Rank %d: Player %d - Score: %d\n", rank, current->number, current->score);
    fprintf(fd, "Rank %d: Player %d - Score: %d\n", rank, current->number, current->score);
    current = current->next;
    rank++;
  } while (current != *head);
}

// print the result of the game
void find_winners_or_tie(player** head, FILE* fd) {
  int high_score = (*head)->score;
  player* current = (*head)->next;
  int tie = 0;

  // check whether it is a tie
  do {
    if (current->score == high_score && current != *head) {
      tie = 1;
      break;
    }
    current = current->next;
  } while (current != *head);

  if (tie == 1) {
    printf("It's a tie between the following players with a score of %d:\n", high_score);
    fprintf(fd, "It's a tie between the following players with a score of %d:\n", high_score);
    current = *head;
    do {
      if (current->score == high_score) {
        printf("Player %d\n", current->number);
        fprintf(fd, "Player %d\n", current->number);
      }
      current = current->next;
    } while (current != *head);
  } else {
    printf("\nThe winner is Player %d!\n", (*head)->number);
    fprintf(fd, "\nThe winner is Player %d!\n", (*head)->number);
  }
}

// reset the game for next round
void reset_player_and_deck(player** head, deck** card, int size_0) {
  // player card
  player* current = *head;
  do {
    memset(current->player_card, 0, (size_t)current->size_p * sizeof(int));
    current->size_p = 0;
    current = current->next;
  } while (current != *head);

  // discard pile
  memset((*card)->discard, 0, (size_t)(*card)->size_d * sizeof(int));
  (*card)->size_d = 0;

  // stock pile
  int* new_stock = realloc((*card)->stock, (size_t)size_0 * sizeof(int));
  if (new_stock == NULL) {
    printf("Reshuffle memory allocation failed.\n");
    exit(1);
  }
  (*card)->stock = new_stock;
  for (int i = 0; i < size_0; i++) (*card)->stock[i] = 0;
  (*card)->top = 0;
  (*card)->size_s = size_0;
}

// print all players' cards
void show_all_card(player** head, int demo, FILE* fd) {
  player* current = *head;
  do {
    show_card(&current, fd, 1);
    current = current->next;
    if (demo == 0) {
      pause_t();
      clear_screen();
    }
  } while (current->number != (*head)->number);
}
