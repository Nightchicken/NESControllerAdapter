#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "bsp/board_api.h"
#include "tusb.h"

#define USBV 0 //temp var
#define USBDM 0 //temp var
#define USBDP 0 //temp var
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
void init(){
    //init generic pins
    board_init();
    tusb_init();
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
short getNESControllerInput(){
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
    short controllerData = 0;
    gpio_put(LATCH,1);
    sleep_us(12);
    gpio_put(LATCH,0);
    for (int i = 0; i < 8; i++){
        sleep_us(6);
        gpio_put(CLOCK,0);
        sleep_us(6);
        controllerData = (controllerData | (!gpio_get(DATA) << i));
        gpio_put(CLOCK,1);
    }
    return controllerData;
}
//Interupts
void consoleSwitch_callback(uint pin,uint32_t events){
    if (pin == BUTTON){
        isNES = !isNES;
    }
}
void emulateKeyboardNES(short controllerInput){
    uint8_t modifier = 0;
    uint8_t keycodes[6] = {0};
    int keyIndex = 0;

    if (controllerInput & 0x01) {
        // A Button -> X
        keycodes[keyIndex++] = HID_KEY_X;
    }
    if (controllerInput & 0x02) {
        // B Button -> Z
        keycodes[keyIndex++] = HID_KEY_Z;
    }
    if (controllerInput & 0x04) {
        // Select -> Left Shift
        modifier |= KEYBOARD_MODIFIER_LEFTSHIFT;
    }
    if (controllerInput & 0x08) {
        // Start -> Enter
        keycodes[keyIndex++] = HID_KEY_ENTER;
    }
    if (controllerInput & 0x10) {
        // Up
        keycodes[keyIndex++] = HID_KEY_ARROW_UP;
    }
    if (controllerInput & 0x20) {
        // Down
        keycodes[keyIndex++] = HID_KEY_ARROW_DOWN;
    }
    if (controllerInput & 0x40) {
        // Left
        keycodes[keyIndex++] = HID_KEY_ARROW_LEFT;
    }
    if (controllerInput & 0x80) {
        // Right
        keycodes[keyIndex++] = HID_KEY_ARROW_RIGHT;
    }

    tud_hid_keyboard_report(0, modifier, keycodes);
}


int main(){
    init();
    short controllerData = 0;
    gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_RISE, true, consoleSwitch_callback);

    while(1){
        tud_task();  // Process USB events

        if (isNES && tud_hid_ready()){
            controllerData = getNESControllerInput();
            emulateKeyboardNES(controllerData);
        }
        sleep_ms(10);  // 10ms polling interval
    }
    return 0;
}
