#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "bullet.h"
#include "address_map.h"
#include "definitions.h"
#include "blue_tank.h"
#define IMAGE_WIDTH  320  
#define IMAGE_HEIGHT 240

void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;    // Pointer to the pixel's location in memory (volatile for hardware access)
    
    // Calculate memory address: base + (y * 1024) + (x * 2)
    // y << 10 = y * 1024 (each row is 1024 bytes for 512 pixels at 2 bytes each)
    // x << 1 = x * 2 (each pixel is 2 bytes)
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1); 
    
    *one_pixel_address = line_color;  // Write the color value to the calculated memory address
}

void draw_blue_tank(int screen_offset_x, int screen_offset_y) {
    int x, y;
    unsigned short pixel_color;
    int index;

    for (y = 0; y < IMAGE_HEIGHT; y++) {
        for (x = 0; x < IMAGE_WIDTH; x++) {
            
            // 1. Calculate the 1D index from 2D coordinates
            index = (y * IMAGE_WIDTH) + x;

            // 2. Grab the color from your .h array
            pixel_color = blue_tank[index];

            // 3. Call your hardware-specific function
            // We add screen_offsets so you can move the tank around the screen
            plot_pixel(screen_offset_x + x, screen_offset_y + y, pixel_color);
        }
    }
}
int main(void)
{
    draw_blue_tank(0,0);
}