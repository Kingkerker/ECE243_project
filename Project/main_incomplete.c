#include <stdint.h> 
#include <stdlib.h>  
#include <string.h>
#include <math.h>
#include <time.h>
#include "map.h"
#include "address_map.h"

volatile int pixel_buffer_start;
short int Buffer1[240][512]; 
short int Buffer2[240][512];
volatile int * pixel_ctrl_ptr =(int *)0xFF203020;
volatile int * PS2_ptr = (int *)0xFF200100;
int pixel_value[320][240]; // globally defined variables, may used in obstacle, tank, bullet files
unsigned char byte;

struct obstacle* array[7];

int main(void)
{
    // Seed the random number generator with current time for different results each run
    srand(time(NULL));
    int space= 0;
    initialize();
    int PS2_data, valid;
    // Debug: Print the entire pixel tracking array to console
    // for(int i=0; i<320; i++)
    // {
    //     for(int j=0; j<240; j++)
    //     {
    //         printf("%d ", pixel_value[i][j]); 
    //     }
    // }

    // code to initialize tank's position in the map

    // whole game begins
    while(1)
    {
        PS2_data = *(PS2_ptr);
        valid = PS2_data & 0x8000;
        if(valid)
        {
            byte = PS2_data & 0xFF;
            if (byte == 0x29) // space key is pressed
            {
                space = 1;
            }
            if(space == 1) //key space is pressed
            {
                initialize();
                space == 0;
            }
        }
    }
}

void initialize()
{
    // initialize the game settings, draw the map, set tanks
    // free old obstacles 
     for (int i = 0; i < 7; i++) 
     {
        if (array[i] != NULL) 
        {
            free(array[i]);
            array[i] = NULL;
        }
    }
    for(int i=0; i<320; i++)
    {
        for(int j=0; j<240; j++)
        {
            pixel_value[i][j] = 0;  // Set each pixel to unoccupied
        }
    }
    for(int i=0; i<7; i++) 
    {
        array[i] = malloc(sizeof(struct obstacle)); // Each obstacle gets its own memory
    }
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1;
	
    // Wait for VSync to complete the buffer swap
	wait_for_vsync(pixel_ctrl_ptr); 
	
    // Get the current front buffer address (now Buffer1 becomes the front buffer)
	pixel_buffer_start = *pixel_ctrl_ptr;
    
    // Clear the screen (set all pixels to black)
    clear_screen();
    
    // Draw the red frame border and mark those pixels as occupied
    draw_frame(pixel_value);
    
    // Generate and draw 5 non-overlapping obstacles
    draw_obstacle(array, pixel_value);
}