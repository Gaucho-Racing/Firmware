#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#define NUM_LEDS 80

typedef uint8_t LED_Data[NUM_LEDS][5];

LED_Data led_arr;

void init_led_data() {
    // initialize every RGB as a solid BLUE with 10 brightness
    for (int led = 0; led < NUM_LEDS; led++) {
        led_arr[led][0] = led;
        led_arr[led][1] = 0;
        led_arr[led][2] = 255;
        led_arr[led][3] = 0;
        led_arr[led][4] = 10;
    }
}

void set_pattern(uint8_t pattern[][3], size_t pattern_length) {
    int pattern_index = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        led_arr[i][1] = pattern[pattern_index][0];
        led_arr[i][2] = pattern[pattern_index][1];
        led_arr[i][3] = pattern[pattern_index][2];

        pattern_index += 1;
        pattern_index %= pattern_length;
    }
}

void print_led_data() {
    printf("   R     G     B\n");
    for (size_t i = 0; i < NUM_LEDS; i++) {
        printf("{ %-3hhu   %-3hhu   %-3hhu },\n", led_arr[i][1], led_arr[i][2], led_arr[i][3]);
    }
}

void random_color(uint8_t led[5]) {

    int max_color_index = (rand() % 3) + 1;
    
    for (int i = 1; i <= 3; i++) {
        if (i == max_color_index) {
            led[i] = 255;
            continue;
        }

        led[i] = rand() % 255;
    }
}

// PRECONDITION: led has one 255 value and 0 for everything else
void color_shift(uint8_t led[5]) {
    int color_index = 0;

    for (int i = 1; i <= 3; i++) {
        if (led[i] == 255) {
            color_index = i;
            break;
        }
    }
    if (color_index == 0) return;

    int next_color_index = color_index % 3 + 1; 

    led[color_index] = 0;
    led[next_color_index] = 255;
}

//led[i][1] = red
//led[i][2] = green
//led[i][3] = blue
//led[i][4] = brightness
void step() {
    const size_t size = NUM_LEDS;
    
    for (int i = 0; i < size; i++) {

        // random_color(led_arr[i]);
        
        color_shift(led_arr[i]);
    }
}

// for(int i=0;i<size;i++){
//     for(int red=0;red<=255;red++){
//         led_arr[i][1]+=5;
//     }
//      for(int blue=0;blue<=255;blue++){
//         led_arr[i][2]+=5;
//         led_arr[i][1]-=5;
//     }
//      for(int green=0;red<=255;red++){
//         led_arr[i][3]+=5;
//         led_arr[i][2]-=5;
//     }
    
// }


int main(void) {
    const int delay = 333;

    srand(time(NULL));

    init_led_data();

    uint8_t rgb_pattern[][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}};
    set_pattern(rgb_pattern, 3);

    print_led_data();
    Sleep(delay);

    while (1) {
        step();
        print_led_data();
        Sleep(delay);
    }
}