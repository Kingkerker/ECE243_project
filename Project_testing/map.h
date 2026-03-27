#ifndef MAP_H
#define MAP_H
#include <stdint.h>      
#include <stdlib.h>      
#include <string.h>      
#include <math.h>
#include "address_map.h"
#include "definitions.h"

struct obstacle
{
    int startX;  // Starting X coordinate of the line
    int startY;  // Starting Y coordinate of the line
    int endX;    // Ending X coordinate of the line
    int endY;    // Ending Y coordinate of the line
    int minX;    // Minimum X of bounding box (for overlap detection)
    int maxX;    // Maximum X of bounding box
    int minY;    // Minimum Y of bounding box
    int maxY;    // Maximum Y of bounding box
};

extern volatile int pixel_buffer_start;  
extern short int Buffer1[240][512]; 
extern short int Buffer2[240][512];

extern volatile int * pixel_ctrl_ptr;
extern struct obstacle* array[7];
extern int pixel_value[320][240];
extern int spacePress;

void plot_pixel(int x, int y, short int line_color);
void draw_line(int startX, int startY, int endX, int endY, short int line_color, int pixel_value[320][240]);
void draw_frame(int pixel_value[320][240]);
void set_obstacle_pos(struct obstacle* array[], int centreX, int centreY, int mode, int num);
void draw_obstacle(struct obstacle* array[], int pixel_value[320][240]);
void clear_screen();
void wait_for_vsync(volatile int* pixel_ctrl_ptr);

#endif