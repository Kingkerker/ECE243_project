#include <stdint.h>      // Include standard integer types (uint8_t, int32_t, etc.)
#include <stdlib.h>      // Include standard library functions (malloc, rand, etc.)
#include <string.h>      // Include string manipulation functions (memcpy, memset, etc.)
#include <math.h>        // Include mathematical functions (abs, sqrt, etc.)
#include <time.h>        // Include time functions (time() for random seed)

volatile int pixel_buffer_start;  // Global pointer to the current pixel buffer (volatile prevents compiler optimization)

// Two frame buffers for double buffering - 240 rows, 512 columns (extra space for alignment)
short int Buffer1[240][512]; 
short int Buffer2[240][512];

// Structure to define an obstacle with its position and bounding box for collision detection
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

// Function to plot a single pixel on the screen at coordinates (x, y) with specified color
void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;    // Pointer to the pixel's location in memory (volatile for hardware access)
    
    // Calculate memory address: base + (y * 1024) + (x * 2)
    // y << 10 = y * 1024 (each row is 1024 bytes for 512 pixels at 2 bytes each)
    // x << 1 = x * 2 (each pixel is 2 bytes)
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1); 
    
    *one_pixel_address = line_color;  // Write the color value to the calculated memory address
}

// Function to draw a line using Bresenham's line algorithm
void draw_line(int startX, int startY, int endX, int endY, short int line_color, int pixel_value[320][240]) 
{
    // Calculate the differences between end and start points
    int dx = endX - startX;  // Change in X
    int dy = endY - startY;  // Change in Y
    int abs_dx = abs(dx);    // Absolute change in X (always positive)
    int abs_dy = abs(dy);    // Absolute change in Y (always positive)
    int stepX = (dx > 0) ? 1 : -1;  // Direction to step in X (1 = right, -1 = left)
    int stepY = (dy > 0) ? 1 : -1;  // Direction to step in Y (1 = down, -1 = up)

    // Check if line is more horizontal than vertical
    if (abs_dy <= abs_dx) 
    {
        // More horizontal line - iterate over X coordinates
        int D = 2 * abs_dy - abs_dx;  // Decision parameter for Bresenham's algorithm
        
        int y = startY;  // Start at the initial Y coordinate
        
        // Loop from startX to endX, stepping by stepX each iteration
        for (int x = startX; x != endX; x += stepX) 
        {
            plot_pixel(x, y, line_color);  // Plot the current pixel
            pixel_value[x][y] = 1;         // Mark this pixel as occupied in our tracking array
    
            if (D >= 0)  // Need to step in Y direction
            {
                y += stepY;                 // Move to next Y coordinate
                D += 2 * (abs_dy - abs_dx); // Update decision parameter
            } 
            else  // No need to step in Y direction
            {
                D += 2 * abs_dy;             // Update decision parameter
            }
        }
    } 
    else 
    {
        // More vertical line - iterate over Y coordinates
        int D = 2 * abs_dx - abs_dy;  // Decision parameter for Bresenham's algorithm
        
        int x = startX;  // Start at the initial X coordinate
        
        // Loop from startY to endY, stepping by stepY each iteration
        for (int y = startY; y != endY; y += stepY) 
        {
            plot_pixel(x, y, line_color);  // Plot the current pixel
            pixel_value[x][y] = 1;         // Mark this pixel as occupied in our tracking array
            
            if (D >= 0)  // Need to step in X direction
            {
                x += stepX;                 // Move to next X coordinate
                D += 2 * (abs_dx - abs_dy); // Update decision parameter
            } 
            else  // No need to step in X direction
            {
                D += 2 * abs_dx;             // Update decision parameter
            }
        }
    }
}

// Function to clear the entire screen by setting all pixels to black (color 0)
void clear_screen()                                  
{
    int y, x;                             
    // Loop through all X coordinates from 0 to 319
    for (x = 0; x < 320; x++)                       
        // Loop through all Y coordinates from 0 to 239
        for (y = 0; y < 240; y++)                      
            plot_pixel (x, y, 0);  // Set each pixel to black (0)                    
}

// Function to draw the game frame/border on the screen
void draw_frame(int pixel_value[320][240])
{
    int length = 320, width = 240;  // Store screen dimensions (unused variables)
    
    // Loop through all pixels on the screen
    for(int i=0; i<320; i++)        // i represents X coordinate
    {
        for(int j=0; j<240; j++)    // j represents Y coordinate
        {
            // Check if pixel is in top border (j<=3) or bottom border (j>=236)
            if(j<=3 || j>=236)
            {
                pixel_value[i][j] = 1;     // Mark this pixel as occupied in tracking array
                plot_pixel(i, j, 0xF800);  // Draw red pixel (RGB 5-6-5 format: 0xF800 = red)
            }
            // Check if pixel is in left border (i<=3) or right border (i>=316)
            if(i<=3 || i>=316)
            {
                pixel_value[i][j] = 1;     // Mark this pixel as occupied in tracking array
                plot_pixel(i, j, 0xF800);  // Draw red pixel
            }
        }
    }
}

// Function to draw all 5 obstacles ensuring they don't overlap
void draw_obstacle(struct obstacle* array[], int pixel_value[320][240])
{
    int num = 0;  // Counter for number of obstacles placed (starts at 0)
    
    // Continue until we have placed 5 obstacles
    while(num != 5)
    {
        // Randomly select obstacle orientation: 0 = horizontal, 1 = vertical
        int mode = (rand() % 2); // select vertical or horizontal
        
        if(num == 0)  // First obstacle - no overlap checking needed
        {
            // Generate random center coordinates within allowed range
            int centreX = (rand() % 194) + 63;   // Range: 63 to 256
            int centreY = (rand() % 114) + 63;   // Range: 63 to 176
            
            // Set the obstacle position based on center and orientation
            set_obstacle_pos(array, centreX, centreY, mode, num);
            
            // Draw the line and mark pixels in tracking array
            draw_line(array[num]->startX, array[num]->startY, 
                     array[num]->endX, array[num]->endY, 0xffff, pixel_value);
        }
        else  // Subsequent obstacles - must check for overlaps
        {
            int fit = 0;  // Counter for how many previous obstacles this new one DOES NOT overlap with
            
            // Keep trying until we find a position that doesn't overlap with ANY previous obstacle
            while(fit != num)
            {
                fit = 0;  // Reset counter for each attempt
                
                // Generate new random center coordinates
                int centreX = (rand() % 194) + 63;   // Range: 63 to 256
                int centreY = (rand() % 114) + 63;   // Range: 63 to 176
                
                // Set obstacle position with current center and orientation
                set_obstacle_pos(array, centreX, centreY, mode, num);
                
                // Check against all previously placed obstacles
                for(int i=0; i < num; i++)
                {
                    // Check if bounding boxes DO NOT overlap
                    // Condition is true if they are separated in any direction
                    if(array[num]->minX >= array[i]->maxX ||  // New obstacle is completely to the right
                       array[num]->maxX <= array[i]->minX ||  // New obstacle is completely to the left
                       array[num]->maxY <= array[i]->minY ||  // New obstacle is completely above
                       array[num]->minY >= array[i]->maxY)    // New obstacle is completely below
                    {
                        fit++; // Increment counter for each previous obstacle it doesn't overlap with
                    }
                }
            }
            // Found a valid position - draw the obstacle
            draw_line(array[num]->startX, array[num]->startY, 
                     array[num]->endX, array[num]->endY, 0xffff, pixel_value);
        }
        num++;  // Move to next obstacle number
    }
}

// Function to set the obstacle's start/end points and bounding box based on center and orientation
void set_obstacle_pos(struct obstacle* array[], int centreX, int centreY, int mode, int num)
{
    if(mode == 0) // Horizontal obstacle (line runs left-right)
    {
        // Set line endpoints (50 pixels left and right from center)
        array[num]->startX = centreX - 50;
        array[num]->startY = centreY;
        array[num]->endX = centreX + 50;
        array[num]->endY = centreY;
        
        // Set bounding box (includes 5-pixel margin around the line)
        array[num]->maxX = array[num]->startX + 105;  // 50 + 50 + 5 = 105 from start
        array[num]->maxY = array[num]->startY + 5;    // 5 pixels below the line
        array[num]->minX = array[num]->startX - 5;    // 5 pixels left of start
        array[num]->minY = array[num]->startY - 5;    // 5 pixels above the line
    }
    else // Vertical obstacle (line runs up-down)
    {
        // Set line endpoints (40 pixels above and below center)
        array[num]->startX = centreX;
        array[num]->startY = centreY - 40;
        array[num]->endX = centreX;
        array[num]->endY = centreY + 40;
        
        // Set bounding box (includes 5-pixel margin around the line)
        array[num]->maxX = array[num]->startX + 5;    // 5 pixels right of the line
        array[num]->maxY = array[num]->startY + 85;   // 40 + 40 + 5 = 85 from start
        array[num]->minX = array[num]->startX - 5;    // 5 pixels left of the line
        array[num]->minY = array[num]->startY - 5;    // 5 pixels above the line
    }
}

// Function to wait for vertical sync (VGA timing synchronization)
void wait_for_vsync(volatile int* pixel_ctrl_ptr)        
{
    int status;  // Variable to store the status register value
    
    // Set the buffer swap bit to 1 to request a buffer swap at next VSync
    *pixel_ctrl_ptr = 1;                                
    
    // Read the status register (offset 3 from base)
    status = *(pixel_ctrl_ptr + 3);
    
    // Loop while the VSync bit (bit 0) is set - it clears automatically when VSync occurs
    while ((status & 0x01) != 0)                         
    {
        status = *(pixel_ctrl_ptr + 3);  // Keep reading status until VSync happens
    }
}

// Main program entry point
int main(void)
{
    // Pointer to the pixel buffer controller registers (hardware-specific address for DE1-SoC)
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    
    // Array of pointers to obstacle structures (will hold 5 obstacles)
    struct obstacle* array[5];
    
    // 2D array to track which pixels are occupied (1 = occupied, 0 = free)
    int pixel_value[320][240];
    
    // Initialize all pixels to "free" (0)
    for(int i=0; i<320; i++)
    {
        for(int j=0; j<240; j++)
        {
            pixel_value[i][j] = 0;  // Set each pixel to unoccupied
        }
    }

    // Allocate memory for each obstacle structure on the heap
    for(int i=0; i<5; i++) 
    {
        array[i] = malloc(sizeof(struct obstacle)); // Each obstacle gets its own memory
    }

    // Seed the random number generator with current time for different results each run
    srand(time(NULL));
    
    // Set the back buffer address to Buffer1 (offset 1 from base)
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
    
    // Debug: Print the entire pixel tracking array to console
    // for(int i=0; i<320; i++)
    // {
    //     for(int j=0; j<240; j++)
    //     {
    //         printf("%d ", pixel_value[i][j]); 
    //     }
    // }
}