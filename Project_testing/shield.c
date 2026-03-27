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

void setShield()
{
    int x = 159;
    int y = 119;
    // while(pixel_value[x][y] == 1 || pixel_value[x][y+1] == 1 || pixel_value[x][y-1] == 1 || pixel_value[x-1][y] || pixel_value[x+1][y])
    // {
    //     y = (rand() % 220) + 10;
    // }
    shield.x = x;
    shield.y = y;
    plot_pixel(shield.x, shield.y, 0xFE60);
    plot_pixel(shield.x, shield.y + 1, 0xFE60);
    plot_pixel(shield.x, shield.y - 1, 0xFE60);
    plot_pixel(shield.x - 1, shield.y, 0xFE60);
    plot_pixel(shield.x + 1, shield.y, 0xFE60);

    shieldExist = 1;
}

void tank1GetShield(int tank1_x, int tank1_y, int tank_dir_idx, int dir_x[], int dir_y[], struct shield shield)
{
    int tank_x_dir = dir_x[tank_dir_idx];
    int tank_y_dir = dir_y[tank_dir_idx];

    if(point_in_tank(shield.x, shield.y, tank1_x, tank1_y, tank_x_dir, tank_y_dir) == 1)
    {
        tank1Shield = 1;
        removeShield();
        return;
    }
    else if(point_in_tank(shield.x, shield.y + 1, tank1_x, tank1_y, tank_x_dir, tank_y_dir) == 1)
    {
        tank1Shield = 1;
        removeShield();
        return;
    }
    else if(point_in_tank(shield.x, shield.y - 1, tank1_x, tank1_y, tank_x_dir, tank_y_dir) == 1)
    {
        tank1Shield = 1;
        removeShield();
        return;
    }
    else if(point_in_tank(shield.x - 1, shield.y,  tank1_x, tank1_y, tank_x_dir, tank_y_dir) == 1)
    {
        tank1Shield = 1;
        removeShield();
        return;
    }
    else if(point_in_tank(shield.x + 1, shield.y, tank1_x, tank1_y, tank_x_dir, tank_y_dir) == 1)
    {
        tank1Shield = 1;
        removeShield();
        return;
    }
    else
    {
        return;
    }
}

void tank2GetShield(int tank2_x, int tank2_y, int tank_dir_idx, int dir_x[], int dir_y[], struct shield shield)
{
    int tank_x_dir = dir_x[tank_dir_idx];
    int tank_y_dir = dir_y[tank_dir_idx];

    if(point_in_tank(shield.x, shield.y, tank2_x, tank2_y, tank_x_dir, tank_y_dir) == 1)
    {
        tank2Shield = 1;
        removeShield();
        return;
    }
    else if(point_in_tank(shield.x, shield.y + 1, tank2_x, tank2_y, tank_x_dir, tank_y_dir) == 1)
    {
        tank2Shield = 1;
        removeShield();
        return;
    }
    else if(point_in_tank(shield.x, shield.y - 1, tank2_x, tank2_y, tank_x_dir, tank_y_dir) == 1)
    {
        tank2Shield = 1;
        removeShield();
        return;
    }
    else if(point_in_tank(shield.x - 1, shield.y,  tank2_x, tank2_y, tank_x_dir, tank_y_dir) == 1)
    {
        tank2Shield = 1;
        removeShield();
        return;
    }
    else if(point_in_tank(shield.x + 1, shield.y, tank2_x, tank2_y, tank_x_dir, tank_y_dir) == 1)
    {
        tank2Shield = 1;
        removeShield();
        return;
    }
    else
    {
        return;
    }
}

void removeShield()
{
    restore_pixel_from_map(shield.x, shield.y);
    restore_pixel_from_map(shield.x+1, shield.y);
    restore_pixel_from_map(shield.x-1, shield.y);
    restore_pixel_from_map(shield.x, shield.y+1);
    restore_pixel_from_map(shield.x, shield.y-1);
    shieldExist = 0;
}



