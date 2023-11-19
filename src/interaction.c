#include "interaction.h"

// play the game until it ends
void game_play(player** head, deck** card, int first, int demo, FILE* fd) {
  int size_0 = (*card)->size_s;  // the original size
  int current_card = (*card)->stock[(*card)->top];
  int attack_sum = 0;
  int is_clockwise = 1;  // control the clockwise;
  char card_i[20] = {0};
  num2card(current_card, card_i);
  (*card)->discard[(*card)->size_d] = (*card)->stock[(*card)->top];
  (*card)->top++;
  (*card)->size_d++;
  printf("---- Game start ----\nFirst card: %s\n", card_i);
  fprintf(fd, "---- Game start ----\nFirst card: %s\n", card_i);
  // to the first player
  player* current = *head;
  while (current->number != first) current = current->next;
  // circle
  while (check_size_p(head) == 0) {
    int status = 0;
    if (demo == 1) {
      status = play_card_demo(&current, card, &current_card, &attack_sum, fd);
    } else {
      printf("Player %d\n", current->number);
      show_card(&current, fd, 0);
      status = play_card(&current, card, &current_card, &attack_sum, fd);
    }

    if (status == -1) {
      current_card = 0;
      draw_card(&current, card, &attack_sum, size_0, demo, fd);
      next_player(&current, is_clockwise);
    } else if (status == 0) {
      next_player(&current, is_clockwise);
    } else if (status == 1) {
      is_clockwise = -is_clockwise;
      next_player(&current, is_clockwise);
    } else if (status == 2) {
      next_player(&current, is_clockwise);
      next_player(&current, is_clockwise);
    }
  }
}

// play a card
int play_card(player** current, deck** card, int* current_card, int* attack_sum, FILE* fd) {  // return value: regular-0, reverse-1, skip-2, draw card--1
  // print previous card
  char card_pre[20] = {0};
  num2card(*current_card, card_pre);

  int next_card = 0, next_card_suit = 0, next_card_rank = 0, current_card_suit = 0, current_card_rank = 0;
  int status = 0, normal = 0;  // normal input: will turn to 1
  int index = 0;

  // input the card
  while (normal == 0) {
    printf("Previous card: %s\n", card_pre);
    printf("Cumulative attacks: %d\n", *attack_sum);

    // detect what cards can player play
    auto_detect(current, current_card, attack_sum);

    printf("\nPlayer %d plays: ", (*current)->number);
    char inputStr[100] = {0};
    if (fgets(inputStr, sizeof(inputStr), stdin) != NULL) {
      char str[100] = {0};
      sscanf(inputStr, "%99[^\n]s", str);
      // draw_card situation
      if (strcmp(str, "draw") == 0) {
        return -1;
      }

      next_card = card2num(str);
      // deal with invalid input str
      if (next_card == 0) {
        printf("Invalid Input. Try again.\n");
        continue;
      }
      // find the index of the player card array
      index = find_index(current, next_card);
      if (index == -1) {
        printf("No such card in your deck. Try again.\n");
        continue;
      }
      // calculate suits and ranks
      next_card_suit = (next_card - 1) / 13 + 1;
      next_card_rank = (next_card - 1) % 13 + 1;
      current_card_suit = (*current_card - 1) / 13 + 1;
      current_card_rank = (*current_card - 1) % 13 + 1;

      // judge whether the card is valid
      if (*current_card == 0) {
        normal = 1;
      } else if (next_card_rank == 10 || next_card_rank == 11) {
        normal = 1;
      } else if (*attack_sum > 0 && (next_card_rank == 1 || next_card_rank == 2 || next_card_rank == 6)) {
        normal = 1;
      } else if (*attack_sum == 0 && (next_card_suit == current_card_suit || next_card_rank == current_card_rank)) {
        normal = 1;
      } else {
        printf("Can't match the card with the formal player. Try again.\n\n");
      }
    } else {
      printf("Invalid Input. Try again.\n\n");
    }
  }

  // deal with the played card
  if (next_card_rank == 10) {  // "J"
    discard_exchange(current, card, index, next_card);
    status = 2;
  } else if (next_card_rank == 11) {  // "Q"
    discard_exchange(current, card, index, next_card);
    status = 1;
  } else if (next_card_rank == 6) {  // defense card
    if (*attack_sum > 0) *current_card = 0;
    *attack_sum = 0;
    discard_exchange(current, card, index, next_card);
    status = 0;
  } else {
    // attack_card situation
    if (next_card_rank == 1)
      *attack_sum += 2;
    else if (next_card_rank == 2)
      *attack_sum += 3;
    // regular_card situation
    *current_card = next_card;
    discard_exchange(current, card, index, next_card);
    status = 0;
  }

  // print player information
  char card_i[20] = {0};
  num2card(next_card, card_i);
  fprintf(fd, "Player %d plays: %s\n", (*current)->number, card_i);
  show_card(current, fd, 1);
  // pause and clean the screen
  pause_t();
  clear_screen();
  return status;
}

// demo mode: randomly pick cards
int play_card_demo(player** current, deck** card, int* current_card, int* attack_sum, FILE* fd) {
  char card_i[20] = {0};
  int next_card = 0, index = 0, status = 0;
  // decide which card to play
  if (*current_card == 0) {
    index = rand() % (*current)->size_p;
    next_card = (*current)->player_card[index];
  } else if (*attack_sum == 0) {
    for (int i = 0; i < (*current)->size_p; i++) {
      int candidate = (*current)->player_card[i];
      int current_card_suit = (*current_card - 1) / 13 + 1;
      int current_card_rank = (*current_card - 1) % 13 + 1;
      int candidate_card_suit = (candidate - 1) / 13 + 1;
      int candidate_card_rank = (candidate - 1) % 13 + 1;
      if (candidate_card_rank == 10 || candidate_card_rank == 11 || candidate_card_suit == current_card_suit || candidate_card_rank == current_card_rank) {
        next_card = (*current)->player_card[i];
        index = i;
        break;
      }
    }
  } else {
    for (int i = 0; i < (*current)->size_p; i++) {
      int candidate = (*current)->player_card[i];
      int candidate_card_rank = (candidate - 1) % 13 + 1;
      num2card(next_card, card_i);
      if (candidate_card_rank == 1 || candidate_card_rank == 2 || candidate_card_rank == 6 || candidate_card_rank == 10 || candidate_card_rank == 11) {
        next_card = (*current)->player_card[i];
        index = i;
        break;
      }
    }
  }

  // print card information
  if (next_card != 0) {
    num2card(next_card, card_i);
    printf("Player %d plays: %s\n", (*current)->number, card_i);
    fprintf(fd, "Player %d plays: %s\n", (*current)->number, card_i);
  }

  // deal with the played card
  int next_card_rank = (next_card - 1) % 13 + 1;
  if (next_card == 0) {
    return -1;
  } else if (next_card_rank == 10) {  // "J"
    discard_exchange(current, card, index, next_card);
    status = 2;
  } else if (next_card_rank == 11) {  // "Q"
    discard_exchange(current, card, index, next_card);
    status = 1;
  } else if (next_card_rank == 6) {  // defense card
    if (*attack_sum > 0) *current_card = 0;
    *attack_sum = 0;
    discard_exchange(current, card, index, next_card);
    status = 0;
  } else {
    // attack_card situation
    if (next_card_rank == 1)
      *attack_sum += 2;
    else if (next_card_rank == 2)
      *attack_sum += 3;
    // regular_card situation
    *current_card = next_card;
    discard_exchange(current, card, index, next_card);
    status = 0;
  }
  show_card(current, fd, 1);
  return status;
}

// draw cards from stock pile
void draw_card(player** current, deck** card, int* attack_sum, int size_0, int demo, FILE* fd) {
  int reshuffle_count = 0;
  if (*attack_sum == 0) *attack_sum = 1;
  printf("Player %d draws: ", (*current)->number);
  fprintf(fd, "Player %d draws: ", (*current)->number);
  for (int i = 0; i < *attack_sum; i++) {
    // when the stock pile is running out
    if ((*card)->top >= (*card)->size_s) {
      reshuffle(card, size_0);
      reshuffle_count = 1;
    }
    // draw cards
    int drawn_card = (*card)->stock[(*card)->top];
    (*card)->top++;
    (*current)->player_card[(*current)->size_p] = drawn_card;
    (*current)->size_p++;
    // print information
    char card_i[20] = {0};
    num2card(drawn_card, card_i);
    if (i == 0) {
      printf("%s", card_i);
      fprintf(fd, "%s", card_i);
    } else {
      printf(", %s", card_i);
      fprintf(fd, "%s", card_i);
    }
    if ((drawn_card >= 27 && drawn_card <= 39) || drawn_card == 9 || drawn_card == 22)
      fprintf(fd, "\t");
    else
      fprintf(fd, "\t\t");
  }
  printf("\n");
  fprintf(fd, "\n");
  if (reshuffle_count == 1) {
    printf("Stock pile exhausted. Shuffling the discard pile and restore the stock pile\n");
    fprintf(fd, "Stock pile exhausted. Shuffling the discard pile and restore the stock pile\n");
  }
  show_card(current, fd, 1);
  *attack_sum = 0;
  if (demo == 0) {
    pause_t();
    clear_screen();
  }
}
