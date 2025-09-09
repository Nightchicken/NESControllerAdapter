#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"


#define BUTTON 14
#define GPIO_IN false
#define GPIO_OUT true
#define LEDA 6
#define LEDB 7
#define LEDUP 3
#define LEDDOWN 2
#define LEDLEFT 1
#define LEDRIGHT 0
#define LEDSELECT 5
#define LEDSTART 4
#define DATA 18
#define CLOCK 16
#define LATCH 17
volatile bool isNES = true;
//INIT
void init(){
    //init generic pins
    stdio_init_all();
    gpio_init(LEDA);
    gpio_init(LEDB);
    gpio_init(LEDUP);
    gpio_init(LEDDOWN);
    gpio_init(LEDLEFT);
    gpio_init(LEDRIGHT);
    gpio_init(LEDSELECT);
    gpio_init(LEDSTART);
    gpio_init(CLOCK);
    gpio_init(LATCH);
    gpio_init(DATA);
    gpio_init(BUTTON);
    //Set direction of pins in or out
    gpio_set_dir(BUTTON,GPIO_IN);
    gpio_set_dir(LEDA,GPIO_OUT);
    gpio_set_dir(LEDB,GPIO_OUT);
    gpio_set_dir(LEDUP,GPIO_OUT);
    gpio_set_dir(LEDDOWN,GPIO_OUT);
    gpio_set_dir(LEDLEFT,GPIO_OUT);
    gpio_set_dir(LEDRIGHT,GPIO_OUT);
    gpio_set_dir(LEDSELECT,GPIO_OUT);
    gpio_set_dir(LEDSTART,GPIO_OUT);
    gpio_set_dir(DATA,GPIO_IN);
    gpio_set_dir(LATCH,GPIO_OUT);
    gpio_set_dir(CLOCK,GPIO_OUT);
    gpio_put(CLOCK,1);
}
//GET NES
short getController(){
    //getController
    //
    short data = 0;
    gpio_put(LATCH,1);
    sleep_us(12);
    gpio_put(LATCH,0);
    for (int i = 0; i < 8; i++){
        sleep_us(6);
        gpio_put(CLOCK,0);
        sleep_us(6);
        data = (data | !gpio_get(DATA) << i);
        gpio_put(CLOCK,1);
    }
    return data;
}
//LIGHT DECODING
void decodeTest(short data){
        data = getController();
        if ((data & 1) == 1){
            gpio_put(LEDA,true);
        }
        else{
            gpio_put(LEDA,false);
        }
        if ((data & 2) == 2){
            gpio_put(LEDB,true);
        }
        else{
            gpio_put(LEDB,false);
        }
        if ((data & 4) == 4){
            gpio_put(LEDSELECT,true);
        }
        else{
            gpio_put(LEDSELECT,false);
        }
        if ((data & 8) == 8){
            gpio_put(LEDSTART,true);
        }
        else{
            gpio_put(LEDSTART,false);
        }
        if ((data & 16) == 16){
            gpio_put(LEDUP,true);
        }
        else{
            gpio_put(LEDUP,false);
        }
        if ((data & 32) == 32){
            gpio_put(LEDDOWN,true);
        }
        else{
            gpio_put(LEDDOWN,false);
        }
        if ((data & 64) == 64){
            gpio_put(LEDLEFT,true);
        }
        else{
            gpio_put(LEDLEFT,false);
        }
        if ((data & 128) == 128){
            gpio_put(LEDRIGHT,true);
        }
        else{
            gpio_put(LEDRIGHT,false);
        }
}
//Interupts
void consoleSwitch_callback(uint pin,uint32_t events){
    if (pin == BUTTON){
        isNES = !isNES;
    }
}
void emulateKeyboard(){}


int main(){
/*
    0 - A
    1 - B
    2 - Select
    3 - Start
    4 - Up
    5 - Down
    6 - Left
    7 - Right
*/
    init();
    short data = 0;
    gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_RISE,true, consoleSwitch_callback); 
    while(1){
        if (isNES){
            decodeTest(data);
        }
        else{
        }
    }
    return 0;
}
