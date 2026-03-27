#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define SCREEN_W 320
#define SCREEN_H 240

#define TANK_HALF 3
#define BARREL_LEN_STRAIGHT 3
#define BARREL_LEN_DIAGONAL 2
#define TANK_BOX_HALF 8

#define GREEN 0x07E0
#define CYAN 0x07FF
#define BLACK 0x0000
#define RED 0xF800
#define WHITE 0xFFFF

#define NUM_DIR 8

// Bigger value means slower rotation
#define ROTATE_DELAY 8

// Movement threshold
#define MOVE_SCALE 100

// Straight movement speed
#define STRAIGHT_MOVE_RATE 69

// Diagonal movement speed
#define DIAGONAL_MOVE_RATE 45

// Limit movement steps in one frame
#define MAX_MOVE_STEPS_PER_FRAME 4