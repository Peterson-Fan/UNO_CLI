#include "interaction.h"

int main(int argc, char* argv[]) {
  int n = 4, c = 5, d = 2, r = 1;  // n players, deal c cards per player, use d decks, play r rounds
  int demo = 0;                    // demo mode
  int opt = 0, option_index = 0;   // CLI parameter
  char* file = "onecard.log";      // log file name
  struct option long_options[] = {{"help", no_argument, NULL, 'h'},
                                  {"log", required_argument, NULL, 'l'},
                                  {"player-number", required_argument, NULL, 'n'},
                                  {"initial-cards", required_argument, NULL, 'c'},
                                  {"decks", required_argument, NULL, 'd'},
                                  {"rounds", required_argument, NULL, 'r'},
                                  {"auto", no_argument, NULL, 'a'},
                                  {NULL, 0, NULL, 0}};
  while ((opt = getopt_long(argc, argv, ":hn:c:d:r:a", long_options, &option_index)) != -1) {
    switch (opt) {
      case 'h': {
        printf("-h|--help \t\tprint this help message\n");
        printf("--log filename \t\twrite the logs in filename (default: onecard.log)\n");
        printf("-n n|--player-number=n \tn players, n must be larger than 2 (default: 4)\n");
        printf("-c c|--initial-cards=c \tdeal c cards per player, c must be at least 2 (default: 5)\n");
        printf("-d d|--decks=d \t\tuse d decks 52 cards each, d must be at least 2 (default: 2)\n");
        printf("-r r|--rounds=r \tplay r rounds, r must be at least 1 (default: 1)\n");
        printf("-a|--auto \t\trun in demo mode\n");
        return 0;
      }
      case 'l': {
        file = (optarg == NULL ? "onecard.log" : optarg);
        break;
      }
      case 'n': {
        n = atoi(optarg);
        printf("Number of players: %s\n", optarg);
        break;
      }
      case 'c': {
        c = atoi(optarg);
        printf("Number of cards dealt to per player: %s\n", optarg);
        break;
      }
      case 'd': {
        d = atoi(optarg);
        printf("Number of decks: %s\n", optarg);
        break;
      }
      case 'r': {
        r = atoi(optarg);
        printf("Number of rounds: %s\n", optarg);
        break;
      }
      case 'a': {
        demo = 1;  // need to be added
        printf("Run in demo mode\n");
        break;
      }
      case '?': {
        printf("Unknown option %c\n", optopt);
        break;
      }
      case ':': {
        printf("Missing option for %c\n", optopt);
        break;
      }
      default:
        printf("%d %d %d %d %d", demo, n, c, d, r);  // for compiling the file successful
    }
  }
  // detect invalid input
  if (n <= 1 || c <= 0 || d <= 0 || r <= 0) {
    printf("Invalid input.\n");
    return 0;
  }
  if (n * c + 5 >= 52 * d) {
    printf("The number of decks is not sufficient.\n");
    return 0;
  }
  // misc
  srand((unsigned)time(NULL));
  FILE* fd = fopen(file, "w+");
  // start the game
  game(n, c, d, r, demo, fd);
  fclose(fd);
  return 0;
}

// game mode
void game(int n, int c, int d, int r, int demo, FILE* fd) {
  // Initialize the whole game
  printf("########################\n#                      #\n# Welcome to One Card! #\n#                      #\n########################\n\n");
  printf("---- Initial setup ----\nNumber of players: %d\nCards dealt to per player: %d\nNumber of decks: %d\nNumber of rounds: %d\n\n", n, c, d, r);
  fprintf(fd, "########################\n#                      #\n#       GAME LOG       #\n#                      #\n########################\n\n");
  fprintf(fd, "---- Initial setup ----\nNumber of players: \t\t\t%d\nCards dealt to per player: %d\nNumber of decks: \t\t\t%d\nNumber of rounds: \t\t\t%d\n\n", n, c, d, r);
  // Initialize players
  player* head = NULL;
  for (int i = 1; i <= n; i++) new_player(&head, i, d);
  // Initialize decks
  deck* card = calloc((size_t)1, sizeof(deck));
  if (card == NULL) {
    printf("Memory allocation failed.\n");
    exit(1);
  }
  deck_init(&card, d);
  // enter each round
  for (int i = 1; i <= r; i++) {
    printf("-----Round %d-----\n", i);
    fprintf(fd, "-----Round %d-----\n", i);
    if (i > 1) {
      printf("\n\n\n");
      fprintf(fd, "\n\n\n");
      reset_player_and_deck(&head, &card, 52 * d);
    }
    // shuffle cards
    printf("Shuffling cards...\n");
    fprintf(fd, "Shuffling cards...\n");
    for (int j = 0; j < card->size_s; j++) card->stock[j] = (((j + 1) % 52 != 0) ? ((j + 1) % 52) : 52);  // add cards
    shuffle(&card);
    show_card_deck_log(card, fd);
    if (demo == 1) {
      printf("Shuffling results:\n");
      show_card_deck(card);
    }
    // deal cards
    fprintf(fd, "\n\nDealing cards...\n");
    printf("\n\nDealing cards...\n");
    deal_card(&head, &card, c);
    // show cards
    show_all_card(&head, demo, fd);
    // determine play order
    printf("\nDetermining the playing order...\n");
    fprintf(fd, "\nDetermining the playing order...\n");
    int* candidate = calloc((size_t)n, sizeof(int));
    if (candidate == NULL) {
      printf("Memory allocation failed.\n");
      exit(1);
    }
    for (int j = 0; j < n; j++) candidate[j] = 1;
    int first = play_order(&candidate, &card, n, fd);
    printf("The game will start with player %d\n\n", first);
    fprintf(fd, "The game will start with player %d\n\n", first);
    free(candidate);
    candidate = NULL;
    if (demo == 0) pause_t();
    // real game start
    game_play(&head, &card, first, demo, fd);
    // calculate the score
    int winner_number = winner(&head);
    printf("\nPlayer %d wins!\n", winner_number);
    printf("\n---- Stats ----\nRound %d result:\n", i);
    fprintf(fd, "\nPlayer %d wins!\n", winner_number);
    fprintf(fd, "\n---- Stats ----\nRound %d result:\n", i);
    print_score(&head, fd);
    printf("Round %d ends.\n", i);
    fprintf(fd, "Round %d ends.\n\n\n", i);
    if (demo == 0) {
      pause_t();
      clear_screen();
    }
  }
  // final score and winner
  printf("\nAll rounds end.\n");
  fprintf(fd, "\nAll rounds end.\n");
  calculate_scores(&head, fd);
  // free memory: player and deck
  free_player(&head);
  head = NULL;
  free_deck(&card);
  card = NULL;
}
