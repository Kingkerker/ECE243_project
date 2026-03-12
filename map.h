#include <stdint.h>      
#include <stdlib.h>      
#include <string.h>      
#include <math.h>

void plot_pixel(int x, int y, short int line_color);

void draw_line(int startX, int startY, int endX, int endY, short int line_color, int pixel_value[320][240]);

void draw_frame(int pixel_value[320][240]);

void set_obstacle_pos(struct obstacle* array[], int centreX, int centreY, int mode, int num);

void draw_obstacle(struct obstacle* array[]);

void clear_screen();

void wait_for_vsync(volatile int* pixel_ctrl_ptr);