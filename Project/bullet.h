#ifndef BULLET_H
#define BULLET_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "map.h"
#include "tank.h"
#include "address_map.h"
#include "definitions.h"

extern volatile int pixel_buffer_start;  
extern short int Buffer1[240][512]; 
extern short int Buffer2[240][512];

extern volatile int * pixel_ctrl_ptr;
// extern struct obstacle* array[7];





extern struct bullet* tank1_bullets[3]; // new global variables defined in main.c
extern struct bullet* tank2_bullets[3];
extern int gameEnd;




extern int pixel_value[320][240];
extern int spacePress;

extern int tank1Hit;
extern int tank2Hit;

extern const int dir_x[NUM_DIR];
extern const int dir_y[NUM_DIR];

extern int tank1_x;
extern int tank1_y;
extern int tank1_dir_idx;

extern int tank2_x;
extern int tank2_y;
extern int tank2_dir_idx;

struct bullet
{
    int xPos;
    int yPos;
    int old_x;
    int old_y;
    int x_dir;
    int y_dir;
    int bounceNum;
};

void tank1Fire(struct bullet* tank1_bullets[3]);
void tank2Fire(struct bullet* tank2_bullets[3]);
void setBullet(struct bullet* tank_bullets[], int id, int tank_x, int tank_y,  int tank_dir_idx, int dir_x[], int dir_y[]);
void updateBullets(struct bullet*  tank1_bullets[3], struct bullet*  tank2_bullets[3]);
void bounceDetect(struct bullet*  tank_bullets[3], int id, int dir_x[], int dir_y[]);
int hitTank1(int tank1_x, int tank1_y, struct bullet* tank1_bullets[3], int id, int tank_dir_idx, int dir_x[], int dir_y[]);
int hitTank2(int tank2_x, int tank2_y, struct bullet* tank2_bullets[3], int id, int tank_dir_idx, int dir_x[], int dir_y[]);
void tank1Wins();
void tank2Wins();

#endif