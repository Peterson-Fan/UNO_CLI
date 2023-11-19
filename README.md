# P2-One Card Game

## Description

The program is about the card game "OneCard". The program provides a real-game mode for multiple players to play and a demo mode for playing the game automatically. A log file will log the game detail.

## Getting started

1. enter the `./src` directory

2. in a bash shell, input the command: `make`

3. run `./onecard` program with different arguments. For example, `./onecard --rounds=3 -n 4 --decks=2 -c 8`to start the game in real-game mode., and `./onecard --rounds=3 -n 4 --decks=2 -c 8 -a` to start the game in demo mode.

   For CLI argument help, input `./onecard --help` to print the help interface.

4. When the game starts, a player can type the card name to play a card, or input "draw" to draw a card. An example:
    ```
    draw
    Diamond 9 
    ```

## Key Data Structure

A circular double linked list is used for players, and dynamic arrays storing int type data are widely used for cards.

```C
typedef struct _player {
  int number;                 // player number, e.g. 1, 2...
  int score;                  // player score
  int* player_card;           // player card array
  int size_p;                 // size of player card
  struct _player* next;       // pointer to the next player
  struct _player* previous;   // pointer to the previous player
} player;

typedef struct _deck {
  int* stock;     // stock pile array
  int* discard;   // discard pile array
  int size_s;     // size of the stock pile
  int size_d;     // size of the discard pile
  int top;        // top of the stock pile
} deck;
```

The card information(suit and rank) is stored as an integer ranging form 1 to 52. The corresponding transformation is as followed:
```C
// card number->suit and rank
suit = (card - 1) / 13 + 1;
rank = (card - 1) % 13 + 1;
// suit and rank->card number
card=suit * 13 + rank -14;
```
Here suit is 1, 2, 3 or 4, representing Spade, Heart, Diamond and Club respectively. And rank from 1 to 13 means card rank from 2 to A.


## Program Function Structure

The program uses layer programming method to brings much flexibility while also saving much rewriting when adjusting the code and allowing faster debugging in case of problem. The functions in lower layer can't access functions in upper layer.

### Layer 1-data structure: Functions dealing with data structures directly

- new_player: \
Initialize players

- deck_init: \
Initialize decks

- free_player: \
Free memory of players when the game ends

- free_deck: \
Free memory of decks when the game ends

- delete_player_card: \
Delete one card in player's deck

- discard_exchange: \
Delete one card in player's deck, then put it into discard pile

- check_size_p: \
Check whether a player has played all his or her cards

- find_index: \
Find the index of a card in a player's cards

- num2card: \
Convert number to card(string)

- card2num: \
Convert card(string) to number

- reshuffle: \
Put discarded cards into stock pile then shuffle them

- shuffle: \
Shuffle cards in stock pile

- compare: \
Compare function for quick sort algorithm

- next_player: \
Return pointer to the "next" player by clockwise/counterclockwise direction

- show_card_deck: \
Print cards in stock pile

- show_card_deck_log: \
Print cards in stock pile(in log file)

- show_card: \
Print player's cards

- clear_screen: \
Clean the screen

- pause_t: \
  Pause the program


### Layer 2-game: Functions needed to play the game

- deal_card: \
Deal cards to every player

- play_order: \
Decide who is the first player

- auto_detect: \
Help player know what cards can be played

- winner: \
Detect who is the winner

- print_score: \
Get the score and print it out

- calculate_scores: \
Print the leaderboard and winner when the game ends

- sort_players_by_score: \
Sort Players by his or her score

- print_leaderboard: \
Print leaderboard

- find_winners_or_tie: \
Print the result of the game

- reset_player_and_deck: \
Reset the game for next round

- show_all_card: \
Print all players' cards

### Layer 3-interaction: Function needed for the user to interact with the computer and play

- game: \
Organize the game process

- game_play: \
Organize the game process in a round

- play_card: \
play a card

- play_card_demo: \
play a card in demo mod

- draw_card: \
Draw cards

## Game Setting

One Card is a rather simple game played by n persons over a pre-decided number of rounds r . A total of d decks
of Poker cards, excluding Jokers, are shuffled and c cards are offered to each player. Before the game starts each
player is offered an extra card to determine the playing order. This game is then discarded. The game will be played
counter-clockwise, starting with the player who received the extra card with lowest rank. Once the playing order has
been decided all those initial n cards are directly put in the discard pile. Finally, the game starts with the first card of
the stock pile being posed face-up, to initiate the rank and suit.

As the game starts each player, following the defined order, plays exactly one card either following the rank or the suit
defined by the previous card. Any played card directly goes into the discard pile, and anyone who is unable to play
should draw a card from the top of the stock pile. If the stock pile is exhausted, the discard pile is shuffled and used
as stock pile.

As soon as a player has no cards left, the rounds stops. All other players receive a penalty equal to the number of cards
left in their hands. The player who won the round initiates the following one, all the other rules remain unchanged.
At the end of the r rounds the final score of the players is determined by summing up their respective penalties. The
person with highest score wins. In case of equality more than one player can be declared winner.

Cards split into four main categories:
- Attack:
   - Cards with rank 2: the next player draws two cards from the stock pile
   - Cards with rank 3: the next player draws three cards from the stock pile
- Defense:
   - Cards with rank 7: cancel an attack, i.e. do not draw any card if a 2 or a 3 was played before
- Action:
   - Queen cards: reverse the playing order from counter-clockwise to clockwise or clockwise to counterclockwise
   - Jack cards: skip the next player
- Regular: any other card has no special effect and is only used to match the previous card's rank or suit

Notes on cards and attacks:
- The effect of the attack cards is cumulative.
- A Queen or a Jack, of same suit as the previous card, can be played to redirect an attack on the previous player,
or the player after the next one, respectively;
- When attacked, a player not playing a special card (2, 3, 7, Q, J) must draw cards from the stock pile, before
ending its turn;

## Bugs

All detected bugs have been fixed.

## Contributor

Fan Yuheng 523370910168

