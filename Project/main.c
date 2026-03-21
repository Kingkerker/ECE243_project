#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "map.h"
#include "tank.h"
#include "bullet.h"
#include "address_map.h"
#include "definitions.h"

volatile int pixel_buffer_start; // Current drawing buffer
short int Buffer1[240][512]; // First pixel buffer
short int Buffer2[240][512]; // Second pixel buffer
volatile int *pixel_ctrl_ptr = (int *)0xFF203020; // VGA buffer controller
volatile int *PS2_ptr = (int *)0xFF200100; // PS/2 port address

int spacePress = 0;
int tank1Hit = 0;
int tank2Hit = 0;
int tank1_Fire = 0;
int tank2_Fire = 0;
int gameEnd = 0;


int pixel_value[320][240]; // Logical map data
unsigned char byte; // Last PS/2 byte

struct obstacle *array[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL}; // Obstacle list
struct bullet* tank1_bullets[3] = {NULL, NULL, NULL};
struct bullet* tank2_bullets[3] = {NULL,NULL,NULL};


// Direction x component
const int dir_x[NUM_DIR] = { 1, 1, 0,-1,-1,-1, 0, 1 };

// Direction y component
const int dir_y[NUM_DIR] = { 0, 1, 1, 1, 0,-1,-1,-1 };

int tank1_x = 15; // Tank 1 x position
int tank1_y = 120; // Tank 1 y position
int tank1_dir_idx = 0; // Tank 1 starts facing right

int tank2_x = 304; // Tank 2 x position
int tank2_y = 120; // Tank 2 y position
int tank2_dir_idx = 4; // Tank 2 starts facing left

int w_down = 0; // W key state
int a_down = 0; // A key state
int s_down = 0; // S key state
int d_down = 0; // D key state

int up_down = 0; // Up arrow state
int left_down = 0; // Left arrow state
int down_down = 0; // Down arrow state
int right_down = 0; // Right arrow state

int break_code = 0; // Key release flag
int extended_code = 0; // Extended key flag

int tank1_rotate_counter = 0; // Rotation counter for tank 1
int tank2_rotate_counter = 0; // Rotation counter for tank 2

int tank1_move_acc = 0; // Movement accumulator for tank 1
int tank2_move_acc = 0; // Movement accumulator for tank 2

int main(void)
{
    int front_buffer; // Buffer currently shown
    int back_buffer; // Buffer currently drawn

    int old1_x, old1_y, old2_x, old2_y; // Old positions
    int old1_dir_idx, old2_dir_idx; // Old directions

    pixel_buffer_start = (int)&Buffer1; // Give a valid start buffer

    srand((unsigned int)time(NULL)); // Seed random generator
    initialize(); // Build the map

    *(pixel_ctrl_ptr + 1) = (int)&Buffer1; // Set front buffer
    wait_for_vsync(pixel_ctrl_ptr); // Wait for sync

    pixel_buffer_start = (int)&Buffer1; // Draw on Buffer1
    draw_tank(tank1_x, tank1_y, dir_x[tank1_dir_idx], dir_y[tank1_dir_idx], GREEN);
    draw_tank(tank2_x, tank2_y, dir_x[tank2_dir_idx], dir_y[tank2_dir_idx], CYAN);

    pixel_buffer_start = (int)&Buffer2; // Prepare Buffer2
    draw_static_map(); // Draw map background
    draw_tank(tank1_x, tank1_y, dir_x[tank1_dir_idx], dir_y[tank1_dir_idx], GREEN);
    draw_tank(tank2_x, tank2_y, dir_x[tank2_dir_idx], dir_y[tank2_dir_idx], CYAN);

    front_buffer = (int)&Buffer1; // Current front
    back_buffer = (int)&Buffer2; // Current back

    while (1)
    {
        read_all_ps2_bytes(); // move the read key function to the very beginning, prevent skipping 
        if(spacePress == 1)
        {
            reset();
            gameEnd = 0;
            spacePress = 0;
        }
        if(gameEnd == 0)
        {
            old1_x = tank1_x; // Save tank 1 x
            old1_y = tank1_y; // Save tank 1 y
            old2_x = tank2_x; // Save tank 2 x
            old2_y = tank2_y; // Save tank 2 y
            old1_dir_idx = tank1_dir_idx; // Save tank 1 direction
            old2_dir_idx = tank2_dir_idx; // Save tank 2 direction

            // read_all_ps2_bytes(); // Read keyboard input

            update_tank1(); // Update tank 1
            update_tank2(); // Update tank 2




            if(tank1_Fire == 1)
            {
                tank1Fire(tank1_bullets);
                tank1_Fire = 0;
            }
            if(tank2_Fire == 1)
            {
                tank2Fire(tank2_bullets);
                tank2_Fire = 0;
            }
            updateBullets(tank1_bullets, tank2_bullets);
            // if(gameEnd == 1)
            // {
            //     while(gameEnd == 1)
            //     {
            //         reset();
            //     }
            // }




            apply_frame_delta_to_buffer(
            back_buffer,
            old1_x, old1_y,
            old2_x, old2_y,
            tank1_x, tank1_y, tank1_dir_idx,
            tank2_x, tank2_y, tank2_dir_idx
            );
        }

        *(pixel_ctrl_ptr + 1) = back_buffer; // Show new frame
        wait_for_vsync(pixel_ctrl_ptr); // Wait for next sync

        int temp = front_buffer; // Swap buffers
        front_buffer = back_buffer;
        back_buffer = temp;
        
    }

    return 0;
}

void initialize(void)
{
    int i, j; // Loop indices

    for (int i = 0; i < 7; i++)
    {
        if (array[i] != NULL)
        {
            free(array[i]); // Free old obstacle
            array[i] = NULL; // Clear pointer
        }
    }
    for(int i=0; i<3; i++)
    {
        if(tank1_bullets[i] != NULL)
        {
            free(tank1_bullets[i]);
            tank1_bullets[i] = NULL;
        }
        if(tank2_bullets[i] != NULL)
        {
            free(tank2_bullets[i]);
            tank2_bullets[i] = NULL;
        }
    }

    for (int i = 0; i < SCREEN_W; i++)
    {
        for (int j = 0; j < SCREEN_H; j++)
        {
            pixel_value[i][j] = 0; // Clear map
        }
    }

    for (int i = 0; i < 7; i++)
    {
        array[i] = malloc(sizeof(struct obstacle)); // Allocate obstacle
    }

    draw_frame(pixel_value); // Draw border
    draw_obstacle(array, pixel_value); // Draw random obstacles
}

// void reset()
// {
//     if(spacePress == 1)
//     {
//         clear_screen();
//         initialize();
//         draw_tank(15, 120, dir_x[0], dir_y[0], GREEN);
//         draw_tank(304, 120, dir_x[4], dir_y[4], CYAN);
//         spacePress = 0;
//     }
// }

void reset()
{
    if(spacePress == 1)
    {
        // reset tanks position
        tank1_x = 15;
        tank1_y = 120;
        tank1_dir_idx = 0;

        tank2_x = 304;
        tank2_y = 120;
        tank2_dir_idx = 4;

        tank1_rotate_counter = 0;
        tank2_rotate_counter = 0;
        tank1_move_acc =0;
        tank2_move_acc = 0;

        pixel_buffer_start = (int)&Buffer1;
        clear_screen();
        pixel_buffer_start = (int)&Buffer2;
        clear_screen();

        pixel_buffer_start = (int)&Buffer1;
        draw_static_map();
        draw_tank(15, 120, dir_x[0], dir_y[0], GREEN);
        draw_tank(304, 120, dir_x[4], dir_y[4], CYAN);
        for(int i=0; i<3; i++)
        {
            if(tank1_bullets[i] != NULL)
            {
                restore_pixel_from_map(tank1_bullets[i]->old_x, tank1_bullets[i]->old_y);
                restore_pixel_from_map(tank1_bullets[i]->xPos, tank1_bullets[i]->yPos);
            }
            if(tank2_bullets[i] != NULL)
            {
                restore_pixel_from_map(tank2_bullets[i]->old_x, tank2_bullets[i]->old_y);
                restore_pixel_from_map(tank2_bullets[i]->xPos, tank2_bullets[i]->yPos);
            }
        }

        pixel_buffer_start = (int)&Buffer2;
        draw_static_map();
        draw_tank(15, 120, dir_x[0], dir_y[0], GREEN);
        draw_tank(304, 120, dir_x[4], dir_y[4], CYAN);
        for(int i=0; i<3; i++)
        {
            if(tank1_bullets[i] != NULL)
            {
                restore_pixel_from_map(tank1_bullets[i]->old_x, tank1_bullets[i]->old_y);
                restore_pixel_from_map(tank1_bullets[i]->xPos, tank1_bullets[i]->yPos);
            }
            if(tank2_bullets[i] != NULL)
            {
                restore_pixel_from_map(tank2_bullets[i]->old_x, tank2_bullets[i]->old_y);
                restore_pixel_from_map(tank2_bullets[i]->xPos, tank2_bullets[i]->yPos);
            }
        }

        for(int i=0; i<3; i++)
        {
            if(tank1_bullets[i] != NULL)
            {
                free(tank1_bullets[i]);
                tank1_bullets[i] = NULL;
            }
            if(tank2_bullets[i] != NULL)
            {
                free(tank2_bullets[i]);
                tank2_bullets[i] = NULL;
            }
        }

        *(pixel_ctrl_ptr + 1) = (int)&Buffer1;
        wait_for_vsync(pixel_ctrl_ptr);


        spacePress = 0;
        gameEnd = 0;

    }
}