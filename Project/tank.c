#include <stdint.h> 
#include <stdlib.h>  
#include <string.h>    
#include <math.h>
#include <time.h>
#include "tank.h"
#include "map.h"
#include "definitions.h"


void read_all_ps2_bytes(void)
{
    int PS2_data; // Raw PS/2 data
    int valid; // Valid bit

    while (1)
    {
        PS2_data = *(PS2_ptr); // Read PS/2 register
        valid = PS2_data & 0x8000; // Check RVALID bit

        if (!valid)
        break; // Stop if FIFO is empty

        byte = PS2_data & 0xFF; // Extract scan code

        if (byte == 0xE0)
        {
            extended_code = 1; // Next code is extended
        }
        else if (byte == 0xF0)
        {
            break_code = 1; // Next code is release
        }
        else if(byte == 0x29) // reset key
        {
            spacePress =1;
        }
        else
        {
            if (!extended_code)
            {
                if (!break_code)
                {
                    if (byte == 0x1D) w_down = 1; // W pressed
                    else if (byte == 0x1C) a_down = 1; // A pressed
                    else if (byte == 0x1B) s_down = 1; // S pressed
                    else if (byte == 0x23) d_down = 1; // D pressed
                    else if (byte == 0x15) tank1_Fire = 1;
                    else if (byte == 0x3A) tank2_Fire  = 1;
                }
                else
                {
                    if (byte == 0x1D) w_down = 0; // W released
                    else if (byte == 0x1C) a_down = 0; // A released
                    else if (byte == 0x1B) s_down = 0; // S released
                    else if (byte == 0x23) d_down = 0; // D released

                    break_code = 0; // Clear break flag
                }
            }
            else
            {
                if (!break_code)
                {
                    if (byte == 0x75) up_down = 1; // Up pressed
                    else if (byte == 0x6B) left_down = 1; // Left pressed
                    else if (byte == 0x72) down_down = 1; // Down pressed
                    else if (byte == 0x74) right_down = 1;// Right pressed
                    //else if (byte == 0x71) tank2_Fire = 1;
                }
                else
                {
                    if (byte == 0x75) up_down = 0; // Up released
                    else if (byte == 0x6B) left_down = 0; // Left released
                    else if (byte == 0x72) down_down = 0; // Down released
                    else if (byte == 0x74) right_down = 0;// Right released
                    break_code = 0; // Clear break flag
                }
                extended_code = 0; // Clear extended flag
            }
        }
    }
}

int point_in_tank(int px, int py, int cx, int cy, int dir_xv, int dir_yv)
{
int start_x = cx; // Barrel start x
int start_y = cy; // Barrel start y
int barrel_len; // Barrel length
int i; // Loop index

if (px >= cx - TANK_HALF && px <= cx + TANK_HALF &&
py >= cy - TANK_HALF && py <= cy + TANK_HALF)
{
return 1; // Inside tank body
}

if (dir_xv == 0 && dir_yv == 0)
return 0; // Invalid direction

if (dir_xv > 0) start_x = cx + TANK_HALF; // Right edge
if (dir_xv < 0) start_x = cx - TANK_HALF; // Left edge
if (dir_yv > 0) start_y = cy + TANK_HALF; // Bottom edge
if (dir_yv < 0) start_y = cy - TANK_HALF; // Top edge

if (dir_xv != 0 && dir_yv != 0)
barrel_len = BARREL_LEN_DIAGONAL; // Shorter diagonal barrel
else
barrel_len = BARREL_LEN_STRAIGHT; // Longer straight barrel

for (i = 1; i <= barrel_len; i++)
{
int bx = start_x + dir_xv * i; // Barrel pixel x
int by = start_y + dir_yv * i; // Barrel pixel y

if (px == bx && py == by)
return 1; // Inside barrel
}

return 0; // Not part of tank
}

void draw_tank(int cx, int cy, int dir_xv, int dir_yv, short int color)
{
int x, y; // Pixel coordinates

for (y = cy - TANK_BOX_HALF; y <= cy + TANK_BOX_HALF; y++)
{
for (x = cx - TANK_BOX_HALF; x <= cx + TANK_BOX_HALF; x++)
{
if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H)
{
if (point_in_tank(x, y, cx, cy, dir_xv, dir_yv))
{
plot_pixel(x, y, color); // Draw tank pixel
}
}
}
}
}

void draw_static_map(void)
{
int x, y; // Map coordinates

for (y = 0; y < SCREEN_H; y++)
{
for (x = 0; x < SCREEN_W; x++)
{
if (pixel_value[x][y] == 0)
{
plot_pixel(x, y, BLACK); // Empty space
}
else
{
if (x <= 3 || x >= 316 || y <= 3 || y >= 236)
plot_pixel(x, y, RED); // Border wall
else
plot_pixel(x, y, WHITE); // Obstacle wall
}
}
}
}

void restore_pixel_from_map(int x, int y)
{
if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H)
return; // Ignore out of range

if (pixel_value[x][y] == 0)
{
plot_pixel(x, y, BLACK); // Restore empty pixel
}
else
{
if (x <= 3 || x >= 316 || y <= 3 || y >= 236)
plot_pixel(x, y, RED); // Restore border
else
plot_pixel(x, y, WHITE); // Restore obstacle
}
}

void restore_box(int cx, int cy)
{
int x, y; // Box coordinates

for (y = cy - TANK_BOX_HALF; y <= cy + TANK_BOX_HALF; y++)
{
for (x = cx - TANK_BOX_HALF; x <= cx + TANK_BOX_HALF; x++)
{
restore_pixel_from_map(x, y); // Restore one pixel
}
}
}

void apply_frame_delta_to_buffer(
int buffer_addr,
int old1_x, int old1_y,
int old2_x, int old2_y,
int new1_x, int new1_y, int new1_dir_idx,
int new2_x, int new2_y, int new2_dir_idx
)
{
pixel_buffer_start = buffer_addr; // Select target buffer

restore_box(old1_x, old1_y); // Erase old tank 1
restore_box(new1_x, new1_y); // Clear new tank 1 area
restore_box(old2_x, old2_y); // Erase old tank 2
restore_box(new2_x, new2_y); // Clear new tank 2 area

draw_tank(new1_x, new1_y, dir_x[new1_dir_idx], dir_y[new1_dir_idx], GREEN); // Draw tank 1
draw_tank(new2_x, new2_y, dir_x[new2_dir_idx], dir_y[new2_dir_idx], CYAN); // Draw tank 2
}

int can_place_tank(int new_x, int new_y, int dir_xv, int dir_yv)
{
int x, y; // Check coordinates

for (y = new_y - TANK_BOX_HALF; y <= new_y + TANK_BOX_HALF; y++)
{
for (x = new_x - TANK_BOX_HALF; x <= new_x + TANK_BOX_HALF; x++)
{
if (point_in_tank(x, y, new_x, new_y, dir_xv, dir_yv))
{
if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H)
return 0; // Outside screen

if (pixel_value[x][y] == 1)
return 0; // Hit obstacle
}
}
}

return 1; // Position is valid
}

void rotate_tank1(int step)
{
int new_dir_idx = (tank1_dir_idx + step + NUM_DIR) % NUM_DIR; // New direction

if (can_place_tank(tank1_x, tank1_y, dir_x[new_dir_idx], dir_y[new_dir_idx]))
{
tank1_dir_idx = new_dir_idx; // Apply rotation
}
}

void rotate_tank2(int step)
{
int new_dir_idx = (tank2_dir_idx + step + NUM_DIR) % NUM_DIR; // New direction

if (can_place_tank(tank2_x, tank2_y, dir_x[new_dir_idx], dir_y[new_dir_idx]))
{
tank2_dir_idx = new_dir_idx; // Apply rotation
}
}

int get_move_rate(int dir_idx)
{
if (dir_x[dir_idx] != 0 && dir_y[dir_idx] != 0)
return DIAGONAL_MOVE_RATE; // Diagonal speed

return STRAIGHT_MOVE_RATE; // Straight speed
}

void move_tank1_one_pixel(int forward)
{
int step_x = dir_x[tank1_dir_idx]; // Direction x
int step_y = dir_y[tank1_dir_idx]; // Direction y
int new_x, new_y; // Next position

if (!forward)
{
step_x = -step_x; // Reverse x
step_y = -step_y; // Reverse y
}

new_x = tank1_x + step_x; // Candidate x
new_y = tank1_y + step_y; // Candidate y

if (can_place_tank(new_x, new_y, dir_x[tank1_dir_idx], dir_y[tank1_dir_idx]))
{
tank1_x = new_x; // Update x
tank1_y = new_y; // Update y
}
}

void move_tank2_one_pixel(int forward)
{
int step_x = dir_x[tank2_dir_idx]; // Direction x
int step_y = dir_y[tank2_dir_idx]; // Direction y
int new_x, new_y; // Next position

if (!forward)
{
step_x = -step_x; // Reverse x
step_y = -step_y; // Reverse y
}

new_x = tank2_x + step_x; // Candidate x
new_y = tank2_y + step_y; // Candidate y

if (can_place_tank(new_x, new_y, dir_x[tank2_dir_idx], dir_y[tank2_dir_idx]))
{
tank2_x = new_x; // Update x
tank2_y = new_y; // Update y
}
}

void update_tank1(void)
{
int move_rate = get_move_rate(tank1_dir_idx); // Current movement rate
int steps = 0; // Steps used this frame

if (a_down && !d_down)
{
tank1_rotate_counter++; // Count rotate frames
if (tank1_rotate_counter >= ROTATE_DELAY)
{
rotate_tank1(-1); // Rotate left
tank1_rotate_counter = 0; // Reset counter
}
}
else if (d_down && !a_down)
{
tank1_rotate_counter++; // Count rotate frames
if (tank1_rotate_counter >= ROTATE_DELAY)
{
rotate_tank1(1); // Rotate right
tank1_rotate_counter = 0; // Reset counter
}
}
else
{
tank1_rotate_counter = 0; // No rotation input
}

if (w_down && !s_down)
{
tank1_move_acc += move_rate; // Add forward movement

while (tank1_move_acc >= MOVE_SCALE && steps < MAX_MOVE_STEPS_PER_FRAME)
{
move_tank1_one_pixel(1); // Move forward
tank1_move_acc -= MOVE_SCALE; // Remove one step worth
steps++; // Count step
}
}
else if (s_down && !w_down)
{
tank1_move_acc += move_rate; // Add backward movement

while (tank1_move_acc >= MOVE_SCALE && steps < MAX_MOVE_STEPS_PER_FRAME)
{
move_tank1_one_pixel(0); // Move backward
tank1_move_acc -= MOVE_SCALE; // Remove one step worth
steps++; // Count step
}
}
else
{
tank1_move_acc = 0; // Reset accumulator
}
}

void update_tank2(void)
{
int move_rate = get_move_rate(tank2_dir_idx); // Current movement rate
int steps = 0; // Steps used this frame

if (left_down && !right_down)
{
tank2_rotate_counter++; // Count rotate frames
if (tank2_rotate_counter >= ROTATE_DELAY)
{
rotate_tank2(-1); // Rotate left
tank2_rotate_counter = 0; // Reset counter
}
}
else if (right_down && !left_down)
{
tank2_rotate_counter++; // Count rotate frames
if (tank2_rotate_counter >= ROTATE_DELAY)
{
rotate_tank2(1); // Rotate right
tank2_rotate_counter = 0; // Reset counter
}
}
else
{
tank2_rotate_counter = 0; // No rotation input
}

if (up_down && !down_down)
{
tank2_move_acc += move_rate; // Add forward movement

while (tank2_move_acc >= MOVE_SCALE && steps < MAX_MOVE_STEPS_PER_FRAME)
{
move_tank2_one_pixel(1); // Move forward
tank2_move_acc -= MOVE_SCALE; // Remove one step worth
steps++; // Count step
}
}
else if (down_down && !up_down)
{
tank2_move_acc += move_rate; // Add backward movement

while (tank2_move_acc >= MOVE_SCALE && steps < MAX_MOVE_STEPS_PER_FRAME)
{
move_tank2_one_pixel(0); // Move backward
tank2_move_acc -= MOVE_SCALE; // Remove one step worth
steps++; // Count step
}
}
else
{
tank2_move_acc = 0; // Reset accumulator
}
}


