#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "map.h"
#include "tank.h"
#include "address_map.h"
#include "definitions.h"
#include "audio.h"


// void sound_effect()
// {
//     int amplitude = 0x007FFFFF;
//     int sampling_freq = 8000;
//     int audio_freq = 1000;
//     int half_cycle_sample = 4;
//     int counter = 0;
//     int sample = amplitude;
//     while(counter< (half_cycle_sample* 2))
//     {
//         int fifospace = *(audio_base+1);
//         int WSRC = (fifospace >> 24) & 0xFF;
//         int WSLC = (fifospace >> 16) & 0xFF; 
//         if(WSLC>0 && WSLC>0)
//         {
//             *(audio_base+2) = sample;
//             *(audio_base+3) = sample;
//             if (counter >= half_cycle_sample) 
//             {
//                 counter = 0;           // Reset counter for next half period
//                 sample = -1 * sample;  // Flip sample polarity to generate square wave
//             }
//         }
//         counter++;
//     }
// }

void sound_effect(void)
{
    int amplitude = 0x007FFFFF;              // 16-bit amplitude (safe range -32768 to 32767)
    int sampling_freq = 8000;           // Sampling frequency in Hz
    int audio_freq = 200;              // Desired audio frequency in Hz
    double duration_seconds = 0.3;           // Duration of sound in seconds
    
    // Calculate how many samples to generate
    int total_samples = sampling_freq * duration_seconds;
    int samples_per_cycle = sampling_freq / audio_freq;   // Samples per cycle of the waveform
    int half_cycle_samples = samples_per_cycle / 2;       // Samples per half cycle
    
    // Initialize square wave state
    int sample_value = amplitude;
    int samples_in_half_cycle = 0;
    int sample_count = 0;
    
    while (sample_count < total_samples)  // Loop until we've generated all samples
    {
        // Check if write FIFO has space available
        int fifospace = *(audio_base + 1);  // Fifospace register (offset 4)
        int WSLC = (fifospace >> 16) & 0xFF;   // Left channel write space
        int WSRC = (fifospace >> 24) & 0xFF;   // Right channel write space
        
        // Wait until there's space in both FIFOs
        if (WSLC > 0 && WSRC > 0)
        {
            // Write same sample to both left and right channels
            *(audio_base + 2) = sample_value;  // Left data (offset 8)
            *(audio_base + 3) = sample_value;  // Right data (offset 12)
            
            // Increment sample counter
            sample_count++;
            
            // Generate square wave
            samples_in_half_cycle++;
            if (samples_in_half_cycle >= half_cycle_samples)
            {
                samples_in_half_cycle = 0;
                sample_value = -sample_value;  // Toggle polarity
            }
        }
    }
}

// int main(void)
// {
//     sound_effect();
//     while(1)
//     {

//     }
// }