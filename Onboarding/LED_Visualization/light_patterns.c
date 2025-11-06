#include <windows.h>
#include <stdio.h>

#define NUM_LEDS 80

// #include <stdint.h> is NOT working for some reason
typedef unsigned char uint8_t;

typedef uint8_t LED_Data[NUM_LEDS][5];

uint8_t led_data[NUM_LEDS][5];

void init_led_data(LED_Data led_data, size_t size) {
    for (int led = 0; led < size; led++) {
        led_data[led][0] = led;
        led_data[led][1] = 0;
        led_data[led][2] = 0;
        led_data[led][3] = 0;
        led_data[led][4] = 0;
    }
}

void print_led_data(const LED_Data data, size_t n) {
    printf("   R     G     B\n");
    for (size_t i = 0; i < n; i++) {
        printf("{ %-3hhu   %-3hhu   %-3hhu },\n", data[i][1], data[i][2], data[i][3]);
    }
}

//led[i][1] = red
//led[i][2] = green
//led[i][3] = blue
//led[i][4] = brightness
void step(LED_Data led, size_t size) {
    
    for (int i = 0; i < size; i++){
        led[i][4] = 50;
        led[i][1] += 10;
        led[i][2] += 10;
        led[i][3] += 10;

        // THIS DOES NOT WORK LOL
        // if (led[i][1] == 255 && led[i][2] < 255 && led[i][3] == 0) {
        //     led[i][2]++;
        // } else if (led[i][2] == 255 && led[i][1] > 0 && led[i][3] == 0) {
        //     led[i][1]--;
        // } else if (led[i][2] == 255 && led[i][3] < 255 && led[i][1] == 0) {
        //     led[i][3]++;
        // } else if (led[i][3] == 255 && led[i][2] > 0 && led[i][1] == 0) {
        //     led[i][2]--;
        // } else if (led[i][3] == 255 && led[i][1] < 255 && led[i][2] == 0) {
        //     led[i][1]++;
        // } else if (led[i][1] == 255 && led[i][3] > 0 && led[i][2] == 0) {
        //     led[i][3]--;
        // }
    }
}


int main(void) {
    init_led_data(led_data, NUM_LEDS);
    
    int delay = 1000;

    while (1) {
        step(led_data, NUM_LEDS);
        print_led_data(led_data, NUM_LEDS);
        Sleep(delay);
    }
}