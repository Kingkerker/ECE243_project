#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "map.h"
#include "tank.h"
#include "address_map.h"
#include "definitions.h"

extern volatile int pixel_buffer_start;  
extern short int Buffer1[240][512]; 
extern short int Buffer2[240][512];
extern volatile int * pixel_ctrl_ptr;

extern volatile int *audio_base;

void sound_effect(void);