#include "game_util.h"
#include "print_util.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO:
void playGame(state *currentState) { printf("Playing!\n"); }

int main(void) {

  int choice = NONE;
  char menuChoice[100];

  printf("Welcome to the ICL Text Adventure!\n");
  printf("What would you like to do?\n");
  printf("    1. Quit\n");
  printf("    2. Start a new game\n");
  printf("    3. Load a previously saved game\n");

  while (choice != NEW_GAME && choice != LOAD_GAME && choice != QUIT) {

    printf("Please enter a 1 (quit), 2 (new game) or 3 (load game)!\n");
    printf("   >> ");

    fgets(menuChoice, sizeof(menuChoice), stdin);
    choice = atoi(menuChoice);

    switch (choice) {
    case QUIT:
      quit();

    case NEW_GAME:
      printf("Prepare for a new adventure...\n");
      // TODO: validate huxley ptr
      building_t *huxley = initialiseBuilding();
      state *playerState = initialiseState(huxley->start_room);
      playGame(playerState);

      printBuildingDetails(huxley);
      printf("\n");
      printStateDetails(playerState);

      freeState(playerState);
      freeBuilding(huxley);

      break;

    case LOAD_GAME:
      printf("Enter the load file name: ");

      char fileName[100];
      fgets(fileName, sizeof(fileName), stdin);

      printf("File loaded: %s\n", fileName);

      int result = -1; // loadGame(fileName); // to be checked
      if (result == -1) {
        printf("File loading failed!\n");
        printf("You have been taken back to the menu.\n");
        choice = NONE;
      }
      break;

    default:
      printf("Invalid choice entered!\n");
      break;
    }
  }
  printf("Done\n");

  exit(EXIT_SUCCESS);
}
