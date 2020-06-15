#include "player.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ROOM_HISTORY (5)
#define ROOM_COUNT (5)
#define CLEAR ("clear")
#define ITEM_NUM (5)
#define LOOKUP_FAILURE (NULL)
#define INITIAL_CASH (0)
#define MAX_HEALTH (100)
#define FIND_FAIL (-1)
#define REMOVED (NULL)
#define MAX_PROPERTY (8)
#define PROPERTY_NUM (4)

// enum for position in rooms
typedef enum { EAST, WEST, NORTH, SOUTH, CENTRE } RoomPosition;

// enum for Items stored by person in inventory with respective cost
typedef enum { APPLE, KEYBOARD, MOUSE, MONITOR, CASH, PASS } Item;

// enum for properties of products
typedef enum { EDIBLE = 1, THROWABLE = 2, VALUABLE = 4, BUYABLE = 8 } Property;

// enum for room
typedef enum { LOBBY, LAB, LECTURE_HALL, FUSION, HARRODS } Room_name;

// enum for character type
typedef enum { BATMAN, UTA } Character;

// generic (string, enum) struct for lookups
typedef struct {
  char *key;
  int value;
} pair_t;

// struct for Items and their properties
typedef struct {
  char *key;
  Item name;
  uint8_t properties;
  char *description;
  int hash;
} item_t;

// structure for rooms
// valid for non cyclical room structure
typedef struct room_t {
  RoomName current_room;
  struct room_t **adjacent_rooms;
  RoomPosition position;
  int adjacent_room_count;
  char *description;
  item_t **Items;
  int ItemCount;

} room_t;

// structure for building consisting of room_history
typedef struct building_t {
  room_t *start_room;
} building_t;

typedef struct {
  item_t **inventory;
  int itemCount;
  int health;
  int cash;
} player_t;

typedef struct {
  player_t *player;
  room_t *curr_room_node; // struct for room_t structure

  struct {
    char *username;
    Character character;
    int score;
  } profile;

  // Room_name room_history[MAX_ROOM_HISTORY];
} state;

static const pair_t propertyTable[] = {{"edible", EDIBLE},
                                       {"throwable", THROWABLE},
                                       {"valuable", VALUABLE},
                                       {"buyable", BUYABLE}};
// Supported Items
static const item_t gameItems[] = {
    {"apple", APPLE, EDIBLE, "An apple. Increases health by 5 when eaten!"},
    {"keyboard", KEYBOARD, THROWABLE,
     "A keyboard. A programmer's best friend."},
    {"mouse", MOUSE, THROWABLE, "A mouse. Click and scroll for days."},
    {"monitor", MONITOR, THROWABLE,
     "A monitor. Can't see your seg faults without it!"},
    {"cash", CASH, VALUABLE, "Cash. I wonder what I could buy around here..."}};

extern building_t *initialiseBuilding();
extern void freeBuilding(building_t *huxley);
extern state *initialiseState(room_t *initialRoom);
extern void freeState(state *state1);
