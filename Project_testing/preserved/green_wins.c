#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Structure to store line segment coordinates
typedef struct {
    int x1, y1;
    int x2, y2;
} Line;

volatile int pixel_buffer_start;
short int Buffer1[240][512];
short int Buffer2[240][512];
volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

// Function to wait for vertical sync
void wait_for_vsync(volatile int *pixel_ctrl_ptr)
{
    // Wait for the current frame to finish
    *pixel_ctrl_ptr = 1;
    while (*(pixel_ctrl_ptr + 2) & 0x01) {
        // Wait for vsync
    }
}

void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
    *one_pixel_address = line_color;
}

void clear_screen()
{
    int y, x;
    for (x = 0; x < 320; x++)
        for (y = 0; y < 240; y++)
            plot_pixel(x, y, 0);
}

void draw_thick_line(int x1, int y1, int x2, int y2, short int color, int thickness)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int abs_dx = (dx >= 0) ? dx : -dx;
    int abs_dy = (dy >= 0) ? dy : -dy;
    int stepX = (dx > 0) ? 1 : -1;
    int stepY = (dy > 0) ? 1 : -1;
    int x = x1, y = y1;
    int half = thickness / 2;

    if (abs_dy <= abs_dx) {
        int D = 2 * abs_dy - abs_dx;
        for (int i = 0; i <= abs_dx; i++) {
            for (int off_x = -half; off_x <= half; off_x++) {
                for (int off_y = -half; off_y <= half; off_y++) {
                    int px = x + off_x;
                    int py = y + off_y;
                    if (px >= 0 && px < 320 && py >= 0 && py < 240)
                        plot_pixel(px, py, color);
                }
            }
            if (D >= 0) {
                y += stepY;
                D += 2 * (abs_dy - abs_dx);
            } else {
                D += 2 * abs_dy;
            }
            x += stepX;
        }
    } else {
        int D = 2 * abs_dx - abs_dy;
        for (int i = 0; i <= abs_dy; i++) {
            for (int off_x = -half; off_x <= half; off_x++) {
                for (int off_y = -half; off_y <= half; off_y++) {
                    int px = x + off_x;
                    int py = y + off_y;
                    if (px >= 0 && px < 320 && py >= 0 && py < 240)
                        plot_pixel(px, py, color);
                }
            }
            if (D >= 0) {
                x += stepX;
                D += 2 * (abs_dx - abs_dy);
            } else {
                D += 2 * abs_dx;
            }
            y += stepY;
        }
    }
}

void draw_t1_w(void)
{
    clear_screen();

    const int CHAR_W = 20;
    const int CHAR_H = 30;
    const int SPACING = 5;

    const char text[] = "T1 W";
    int total_chars = 4;
    int total_width = total_chars * CHAR_W + (total_chars - 1) * SPACING;

    int start_x = (320 - total_width) / 2;
    int top_y = (240 - CHAR_H) / 2;

    int current_x = start_x;
    int current_y_top = top_y;

    // ----- Character definitions (top‑origin) -----

    // T (fixed)
    const Line T_lines[] = {
        {0,30,20,30},    // horizontal bar at bottom (will become top after inversion)
        {10,0,10,30}     // vertical stem (will become bottom-to-top, but connects correctly)
    };
    int T_count = sizeof(T_lines) / sizeof(Line);

    // 1
    const Line One_lines[] = {
        {10,0,10,30},
        {5,0,15,0}
    };
    int One_count = sizeof(One_lines) / sizeof(Line);

    // W
    const Line W_lines[] = {
        {0,30,5,0},
        {5,0,10,30},
        {10,30,15,0},
        {15,0,20,30}
    };
    int W_count = sizeof(W_lines) / sizeof(Line);

    // Iterate over characters
    for (int i = 0; text[i] != '\0'; i++) {
        const Line *segments = NULL;
        int seg_count = 0;

        switch (text[i]) {
            case 'T':
                segments = T_lines;
                seg_count = T_count;
                break;
            case '1':
                segments = One_lines;
                seg_count = One_count;
                break;
            case 'W':
                segments = W_lines;
                seg_count = W_count;
                break;
            case ' ':
                current_x += CHAR_W + SPACING;
                continue;
            default:
                continue;
        }

        for (int s = 0; s < seg_count; s++) {
            int x1_rel = segments[s].x1;
            int y1_rel = segments[s].y1;
            int x2_rel = segments[s].x2;
            int y2_rel = segments[s].y2;

            int x1 = current_x + x1_rel;
            int y1_top = current_y_top + y1_rel;
            int x2 = current_x + x2_rel;
            int y2_top = current_y_top + y2_rel;

            // Invert Y for bottom‑origin hardware
            int y1 = 239 - y1_top;
            int y2 = 239 - y2_top;

            draw_thick_line(x1, y1, x2, y2, 0x07E0, 3);
        }

        current_x += CHAR_W + SPACING;
    }
}
int main(void)
{
    pixel_buffer_start = (int)&Buffer1;
    
    // Draw to back buffer (Buffer2)
    pixel_buffer_start = (int)&Buffer2;
    draw_t1_w();  // Changed from draw_blue_wins() to draw_b_wins()
    
    // Set front buffer and wait for vsync
    *(pixel_ctrl_ptr + 1) = (int)&Buffer1;
    wait_for_vsync(pixel_ctrl_ptr);
    
    // Switch to show the drawn buffer
    *(pixel_ctrl_ptr + 1) = (int)&Buffer2;
    
    while(1)
    {
        // Keep displaying
    }
}