#ifndef TANK_H
#define TANK_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "definitions.h"

extern volatile int pixel_buffer_start; // Current drawing buffer
extern short int Buffer1[240][512]; // First pixel buffer
extern short int Buffer2[240][512]; // Second pixel buffer
extern volatile int *pixel_ctrl_ptr;// VGA buffer controller
extern volatile int *PS2_ptr; // PS/2 port address
extern int spacePress;

extern int pixel_value[320][240]; // Logical map data
extern unsigned char byte; // Last PS/2 byte

extern struct obstacle *array[7]; // Obstacle list

extern const int dir_x[NUM_DIR];
extern const int dir_y[NUM_DIR];

extern int tank1_x;
extern int tank1_y;
extern int tank1_dir_idx;

extern int tank2_x;
extern int tank2_y;
extern int tank2_dir_idx;

extern int w_down;
extern int a_down;
extern int s_down; 
extern int d_down; 

extern int up_down; 
extern int left_down; 
extern int down_down; 
extern int right_down; 

extern int break_code; 
extern int extended_code;

extern int tank1_rotate_counter;
extern int tank2_rotate_counter; 

extern int tank1_move_acc;
extern int tank2_move_acc;

extern int tank1_Fire;
extern int tank2_Fire;

extern int tank1Shield;
extern int tank2Shield;
extern int shieldExist;

// void initialize(void);
void draw_static_map(void);
void draw_tank(int cx, int cy, int dir_xv, int dir_yv, short int color);
int point_in_tank(int px, int py, int cx, int cy, int dir_xv, int dir_yv);
int can_place_tank(int new_x, int new_y, int dir_xv, int dir_yv);

void rotate_tank1(int step);
void rotate_tank2(int step);
void move_tank1_one_pixel(int forward);
void move_tank2_one_pixel(int forward);

void update_tank1(void);
void update_tank2(void);

void read_all_ps2_bytes(void);

void restore_pixel_from_map(int x, int y);
void restore_box(int cx, int cy);
void apply_frame_delta_to_buffer(
int buffer_addr,
int old1_x, int old1_y,
int old2_x, int old2_y,
int new1_x, int new1_y, int new1_dir_idx,
int new2_x, int new2_y, int new2_dir_idx
);

int get_move_rate(int dir_idx);

#endif