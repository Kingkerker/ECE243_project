#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "map.h"
#include "tank.h"
#include "address_map.h"
#include "definitions.h"
#include "green_wins.h"
#include "blue_wins.h"
#include "audio.h"
#include "shield.h"
#include "bullet.h"

#define SHIELD_RADIUS 5
/**
 * Tank 1 fires a bullet
 * Searches for an empty slot in tank1's bullet array (max 3 bullets)
 * Creates a new bullet at the tip of tank1's barrel
 * 
 * @param tank1_bullets Array of 3 bullet pointers for tank 1
 */
void tank1Fire(struct bullet* tank1_bullets[])
{
    int valid = 0;  // Flag to track if bullet was successfully created
    
    // Search for first empty slot in the bullet array
    for(int i=0; i<3; i++)
    {
        if(tank1_bullets[i] == NULL)  // Found an empty slot
        {
            // Allocate memory for new bullet structure
            tank1_bullets[i] = malloc(sizeof(struct bullet));
            
            // Initialize bullet position and direction based on tank's current state
            setBullet(tank1_bullets, i, tank1_x, tank1_y, tank1_dir_idx, dir_x, dir_y);
            
            // Draw the bullet on screen (white color 0xffff)
            plot_pixel(tank1_bullets[i]->xPos, tank1_bullets[i]->yPos, 0xffff);
            
            valid = 1;  // Successfully fired
            sound_effect();
            break;      // Exit loop once bullet is created
        }
    }
    
    // If no empty slot found (already 3 bullets active), silently fail
    if(valid == 0)
    {
        return;  // Cannot fire more bullets
    }
}

/**
 * Tank 2 fires a bullet
 * Identical to tank1Fire but uses tank2's position and direction
 * 
 * @param tank2_bullets Array of 3 bullet pointers for tank 2
 */
void tank2Fire(struct bullet* tank2_bullets[])
{
    int valid = 0;
    for(int i=0; i<3; i++)
    {
        if(tank2_bullets[i] == NULL)
        {
            tank2_bullets[i] = malloc(sizeof(struct bullet));
            setBullet(tank2_bullets, i, tank2_x, tank2_y, tank2_dir_idx, dir_x, dir_y);
            plot_pixel(tank2_bullets[i]->xPos, tank2_bullets[i]->yPos, 0xffff);
            valid = 1;
            sound_effect();
            break;
        }
    }
    if(valid == 0)
    {
        return;
    }
}

/**
 * Initializes a new bullet's properties
 * Sets bullet direction to match tank's facing direction
 * Calculates barrel tip position based on tank center and direction
 * 
 * @param tank_bullets Array of bullet pointers for the firing tank
 * @param id Index in bullet array where new bullet will be stored
 * @param tank_x X-coordinate of tank center
 * @param tank_y Y-coordinate of tank center
 * @param tank_dir_idx Current direction index of the tank (0-7)
 * @param dir_x Array of X direction components for each direction
 * @param dir_y Array of Y direction components for each direction
 */
void setBullet(struct bullet* tank_bullets[], int id, int tank_x, int tank_y,  int tank_dir_idx, int dir_x[], int dir_y[])
{
    int barrel_len;  // Distance from tank center to barrel tip
    
    // Set bullet's movement direction to match tank's facing direction
    tank_bullets[id]->x_dir = dir_x[tank_dir_idx];
    tank_bullets[id]->y_dir = dir_y[tank_dir_idx];
    
    // Determine barrel length based on whether direction is diagonal or straight
    if( tank_bullets[id]->x_dir != 0 &&  tank_bullets[id]->y_dir != 0)
    {
        // Diagonal barrel is shorter for visual appeal
        barrel_len = BARREL_LEN_DIAGONAL + 3; 
    }
    else
    {
        // Straight barrel is longer
        barrel_len = BARREL_LEN_STRAIGHT + 3;
    }
    
    // Calculate bullet position: tank center + (direction vector × barrel length)
    tank_bullets[id]->xPos = tank_x + (tank_bullets[id]->x_dir * barrel_len);
    tank_bullets[id]->yPos = tank_y + (tank_bullets[id]->y_dir * barrel_len);
    
    // Store initial position as "old" position for frame differencing (erasing later)
    tank_bullets[id]->old_x = tank_bullets[id]->xPos;
    tank_bullets[id]->old_y = tank_bullets[id]->yPos;
    
    // Initialize bounce counter (bullets can bounce off walls up to 2 times)
    tank_bullets[id]->bounceNum = 0;
}

/**
 * Updates all active bullets for both tanks each frame
 * - Restores background at old bullet positions (erases bullets)
 * - Moves bullets in their current direction
 * - Checks for hits on tanks (both friendly and enemy)
 * - Handles wall bounces if no hit occurred
 * 
 * @param tank1_bullets Array of bullet pointers for tank 1
 * @param tank2_bullets Array of bullet pointers for tank 2
 */
void updateBullets(struct bullet*  tank1_bullets[3], struct bullet*  tank2_bullets[3])
{
    // Update tank 1's bullets
    for(int i=0; i<3; i++)
    {
        if(tank1_bullets[i] != NULL)  // Only process active bullets
        {
            // ERASE STEP: Restore background at bullet's previous position
            // This removes the bullet from its old location
            restore_pixel_from_map(tank1_bullets[i]->old_x, tank1_bullets[i]->old_y);
            
            // Save current position before moving (will be used for next frame's erase)
            tank1_bullets[i]->old_x = tank1_bullets[i]->xPos;
            tank1_bullets[i]->old_y = tank1_bullets[i]->yPos;
            
            // MOVE STEP: Update bullet position by adding direction vector
            tank1_bullets[i]->xPos += tank1_bullets[i]->x_dir;  
            tank1_bullets[i]->yPos += tank1_bullets[i]->y_dir; 
            
            // Check for hits with both tanks
            // First check if bullet hit its own tank (tank1) - friendly fire
            if(hitTank1(tank1_x, tank1_y, tank1_bullets, i, tank1_dir_idx, dir_x, dir_y) == 1)
            {
                tank1Hit = 0;
                tank2Wins();  // Tank 1 hit itself Tank 2 wins
                return;
            }
            // Then check if bullet hit enemy tank (tank2)
            else if(hitTank2(tank2_x, tank2_y, tank1_bullets, i, tank2_dir_idx, dir_x, dir_y) == 1)
            {
                tank2Hit = 0;
                tank1Wins();  // Tank 1's bullet hit tank 2 - tank 1 wins
                return;
            }
            else if(hitTank2(tank2_x, tank2_y, tank1_bullets, i, tank2_dir_idx, dir_x, dir_y) == 2)
            {
                // destroy_bullet(tank1_bullets, i);
                bounceDetect(tank1_bullets, i,  dir_x, dir_y);
            }
            else if(hitTank1(tank1_x, tank1_y, tank1_bullets, i, tank1_dir_idx, dir_x, dir_y) == 2)
            {
                // destroy_bullet(tank1_bullets, i);
                bounceDetect(tank1_bullets, i,  dir_x, dir_y);
            }
            // If no tank hit, check for wall collisions
            else 
            {
                bounceDetect(tank1_bullets, i,  dir_x, dir_y);
            }
            plot_pixel(tank1_bullets[i]->xPos, tank1_bullets[i]->yPos, 0xffff);
        }
        else
        {
            continue;
        }
    }
    
    // Update tank 2's bullets (symmetrical logic)
    for(int i=0; i<3; i++)
    {
        if(tank2_bullets[i] != NULL)
        {
            // Erase bullet from old position
            restore_pixel_from_map(tank2_bullets[i]->old_x, tank2_bullets[i]->old_y);
            
            // Save current position for next erase
            tank2_bullets[i]->old_x = tank2_bullets[i]->xPos;
            tank2_bullets[i]->old_y = tank2_bullets[i]->yPos;
            
            // Move bullet
            tank2_bullets[i]->xPos += tank2_bullets[i]->x_dir;  
            tank2_bullets[i]->yPos += tank2_bullets[i]->y_dir;  
            
            // Check hits
            if(hitTank2(tank2_x, tank2_y, tank2_bullets, i, tank2_dir_idx, dir_x, dir_y) == 1)
            {
                tank2Hit = 0;
                tank1Wins();  // Tank 2 hit itself Tank 1 wins
                return;
            }
            else if(hitTank1(tank1_x, tank1_y, tank2_bullets, i, tank1_dir_idx, dir_x, dir_y) == 1)
            {
                tank1Hit = 0;
                tank2Wins();  // Tank 2's bullet hit tank 1 - tank 2 wins
                return;
            }
            else if(hitTank1(tank1_x, tank1_y, tank2_bullets, i, tank1_dir_idx, dir_x, dir_y) == 2)
            {
                // destroy_bullet(tank2_bullets, i);
                bounceDetect(tank2_bullets, i,  dir_x, dir_y);
            }
            else if(hitTank2(tank2_x, tank2_y, tank2_bullets, i, tank2_dir_idx, dir_x, dir_y) == 2)
            {
                // destroy_bullet(tank2_bullets, i);
                bounceDetect(tank2_bullets, i,  dir_x, dir_y);
            }
            else 
            {
                bounceDetect(tank2_bullets, i,  dir_x, dir_y);
            }
            plot_pixel(tank2_bullets[i]->xPos, tank2_bullets[i]->yPos, 0xffff);
        }
        else
        {
            continue;
        }      
    }
}

/**
 * Detects and handles bullet collisions with walls/obstacles
 * Bullets can bounce up to 2 times before disappearing
 * When bouncing, direction reverses and bounce counter increments
 * 
 * @param tank_bullets Array of bullet pointers for the current tank
 * @param id Index of the bullet to check
 */
void bounceDetect(struct bullet*  tank_bullets[3], int id, int dir_x[], int dir_y[])
{
    // Check if bullet has hit an obstacle (pixel_value == 1 means wall or obstacle)
    if(pixel_value[tank_bullets[id]->xPos][tank_bullets[id]->yPos] == 1)
    {
        // If bullet has already bounced twice, destroy it
        if(tank_bullets[id]->bounceNum == 2)
        {
            // Free memory and mark array slot as empty
            restore_pixel_from_map(tank_bullets[id]->old_x, tank_bullets[id]->old_y);
            free(tank_bullets[id]);
            tank_bullets[id] = NULL;
            return;
        }
        
        // Reverse direction for bounce (multiply by -1)
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<8; j++)
            {
                if(dir_x[i] == tank_bullets[id]->x_dir && dir_y[j] == tank_bullets[id]->y_dir)
                {
                    if(tank_bullets[id]->x_dir == 0 || tank_bullets[id]->y_dir == 0)
                    {
                        tank_bullets[id]->x_dir *= -1;
                        tank_bullets[id]->y_dir *= -1;
                        tank_bullets[id]->bounceNum += 1;
                        return;
                    }
                    else
                    {
                        if((pixel_value[tank_bullets[id]->xPos+7][tank_bullets[id]->yPos] == 1 || pixel_value[tank_bullets[id]->xPos-7][tank_bullets[id]->yPos] == 1)) // horizontal obstacle
                        {
                            tank_bullets[id]->y_dir *=-1;
                            tank_bullets[id]->bounceNum +=1;
                            return;
                        }
                        else
                        {
                            tank_bullets[id]->x_dir *=-1;
                            tank_bullets[id]->bounceNum +=1;
                            return;
                        }
                    }
                }
            }
        }
    }
    return;
}

/**
 * Checks if a bullet has hit tank 1
 * Uses point_in_tank function to determine if bullet coordinates
 * are within tank 1's shape (body or barrel)
 * 
 * @param tank1_x X-coordinate of tank 1 center
 * @param tank1_y Y-coordinate of tank 1 center
 * @param tank_bullets Array of bullet pointers (could be from either tank)
 * @param id Index of bullet to check
 * @param tank_dir_idx Tank 1's current direction (for accurate hit detection)
 * @param dir_x Direction X components array
 * @param dir_y Direction Y components array
 * @return Current value of tank1Hit flag (1 if hit, 0 otherwise)
 */
int hitTank1(int tank1_x, int tank1_y, struct bullet* tank_bullets[3], int id, int tank_dir_idx, int dir_x[], int dir_y[])
{
    // Get direction components for tank 1's current facing
    int tank_x_dir = dir_x[tank_dir_idx];
    int tank_y_dir = dir_y[tank_dir_idx];
    int bullet_x = tank_bullets[id]->xPos;
    int bullet_y = tank_bullets[id]->yPos;

    // Check if bullet position is inside tank 1's shape
    if(tank1Shield == 1)
    {
        int dx = bullet_x - tank1_x;
        int dy = bullet_y - tank1_y;
        if((dx*dx + dy * dy) <= 64)
        {
            tank1Shield = 0;
            destroy_bullet(tank_bullets, id);
            return 2;
        }
    }
    if(point_in_tank(tank_bullets[id]->xPos, tank_bullets[id]->yPos, 
                    tank1_x, tank1_y, tank_x_dir, tank_y_dir) == 1)
    {
    
        tank1Hit = 1;  // Set global hit flag
        sound_effect();
        return 1;
        
    }
    
    return 0;  // Return current hit state
}

/**
 * Checks if a bullet has hit tank 2
 * Symmetrical to hitTank1 but for tank 2
 * 
 * @param tank2_x X-coordinate of tank 2 center
 * @param tank2_y Y-coordinate of tank 2 center
 * @param tank_bullets Array of bullet pointers
 * @param id Index of bullet to check
 * @param tank_dir_idx Tank 2's current direction
 * @param dir_x Direction X components array
 * @param dir_y Direction Y components array
 * @return Current value of tank2Hit flag (1 if hit, 0 otherwise)
 */
int hitTank2(int tank2_x, int tank2_y, struct bullet* tank_bullets[3], int id, int tank_dir_idx, int dir_x[], int dir_y[])
{
    int tank_x_dir = dir_x[tank_dir_idx];
    int tank_y_dir = dir_y[tank_dir_idx];
    int bullet_x = tank_bullets[id]->xPos;
    int bullet_y = tank_bullets[id]->yPos;

    if(tank2Shield == 1)
    {
        int dx = bullet_x - tank2_x;
        int dy = bullet_y - tank2_y;
        if((dx*dx + dy * dy) <= 64)
        {
            tank2Shield = 0;
            destroy_bullet(tank_bullets, id);
            return 2;
        }
    }
    
    if(point_in_tank(tank_bullets[id]->xPos, tank_bullets[id]->yPos, 
                    tank2_x, tank2_y, tank_x_dir, tank_y_dir) == 1)
    {        
        tank2Hit = 1;  // Set global hit flag
        sound_effect();
        return 1;   
    }
    return 0;
}

/**
 * Called when tank 1 wins the game
 * Displays victory message to console
 * Enters infinite loop to halt program execution (game over)
 */
void tank1Wins()
{
    tank1Score +=1;
    gameEnd = 1;
    pixel_buffer_start = (int)&Buffer1;
    clear_screen();
    pixel_buffer_start = (int)&Buffer2;
    clear_screen();

    pixel_buffer_start = (int)&Buffer1;
    draw_t1_w();

    pixel_buffer_start = (int)&Buffer2;
    draw_t1_w();

    *(pixel_ctrl_ptr + 1) = (int)&Buffer1;
    wait_for_vsync(pixel_ctrl_ptr);
}

/**
 * Called when tank 2 wins the game
 * Displays victory message to console
 * Enters infinite loop to halt program execution (game over)
 */
void tank2Wins()
{
    tank2Score+= 1;
    gameEnd = 1;
    pixel_buffer_start = (int)&Buffer1;
    clear_screen();
    pixel_buffer_start = (int)&Buffer2;
    clear_screen();

    pixel_buffer_start = (int)&Buffer1;
    draw_t2_w();

    pixel_buffer_start = (int)&Buffer2;
    draw_t2_w();

    *(pixel_ctrl_ptr + 1) = (int)&Buffer1;
    wait_for_vsync(pixel_ctrl_ptr);
}

void destroy_bullet(struct bullet* tank_bullets[3], int id)
{
    if(tank_bullets[id] != NULL)
    {
        restore_pixel_from_map(tank_bullets[id]->xPos, tank_bullets[id]->yPos);
        free(tank_bullets[id]);
        tank_bullets[id] = NULL;
    }
}


