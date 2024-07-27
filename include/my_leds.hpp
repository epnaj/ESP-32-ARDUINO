#pragma once
#include <Arduino.h>

class MyLEDGPIO {
    private:
    const gpio_num_t led_gpio;

    public:
    MyLEDGPIO(const gpio_num_t led_gpio): led_gpio(led_gpio) {
        pinMode(led_gpio, OUTPUT);
    }
    // ~MyLEDGPIO(){
    //     pinMode(led_gpio, OUTPUT);
    // }
    void single_blink(const uint32_t blink_length) const {
        digitalWrite(led_gpio, LOW); // 0 LED is ON
        delay(blink_length);          //
        digitalWrite(led_gpio, HIGH); // 1 LED is OFF
    }
    // delay in miliseconds
    void blink_led_n_times(const uint32_t blink_length, const uint32_t delay_between_blinks, const unsigned n) const {
        if(!n){
            return;
        }
        single_blink(blink_length);
        for (int i = 1; i < n; ++i){
            delay(delay_between_blinks);
            single_blink(blink_length);
        }
    }
};