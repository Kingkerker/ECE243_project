#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


// You also need to define your pins (CS, DC, RST etc.)
#define TFT_CS   10
#define TFT_DC   9

void tank1Wins()
{
    tft.fillScreen(0x0000);             // Black background
    tft.setTextColor(0x07E0);           // Green color (0x07E0 is pure Green in RGB565)
    tft.setTextSize(3);                 // Make it big and "pixelated"
    
    // Center the text roughly for 320x240
    tft.setCursor(40, 100); 
    tft.print("GREEN TANK WINS !");
    
    gameEnd = 1;
}


