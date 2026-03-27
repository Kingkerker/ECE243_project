#ifndef GREEN_WINS_H
#define GREEN_WINS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "map.h"
#include "tank.h"
#include "address_map.h"
#include "definitions.h"
#include "bullet.h"

typedef struct {
    int x1, y1;
    int x2, y2;
} gLine;

extern volatile int pixel_buffer_start;
extern short int Buffer1[240][512];
extern short int Buffer2[240][512];
extern volatile int * pixel_ctrl_ptr;

void draw_thick_gline(int x1, int y1, int x2, int y2, short int color, int thickness);
void draw_t1_w();

#endif 