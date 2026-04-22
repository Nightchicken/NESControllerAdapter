#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "bsp/board_api.h"
#include "tusb.h"

//Part assignements
#define BUTTON 18
#define BUTTON2 19
#define GPIO_IN false
#define GPIO_OUT true
#define ISNESLED 22
#define ISSNESLED 20
#define ISGAMECUBELED 26
#define ISCONTROLLERLED 21
#define DATA 2
#define CLOCK 4
#define LATCH 3
#define GAMECUBE_DATA 5

enum ControllerType {
    CONTROLLER_NES = 0,
    CONTROLLER_SNES = 1,
    CONTROLLER_GAMECUBE = 2
};

volatile enum ControllerType controllerType = CONTROLLER_NES;
volatile bool isController = false;

void button_callback(uint pin, uint32_t events){
    if (pin == BUTTON){
        controllerType = (controllerType + 1) % 3;
    }
    if (pin == BUTTON2){
        isController = !isController;
    }
}

void init(){
    board_init();
    tusb_init();
    gpio_init(CLOCK);
    gpio_init(LATCH);
    gpio_init(DATA);
    gpio_init(GAMECUBE_DATA);
    if (BUTTON != -1) {
        gpio_init(BUTTON);
        gpio_set_dir(BUTTON,GPIO_IN);
        gpio_pull_down(BUTTON);
        gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_RISE, true, &button_callback);
    }
    if (BUTTON2 != -1) {
		gpio_init(BUTTON2);
        gpio_set_dir(BUTTON2,GPIO_IN);
		gpio_pull_down(BUTTON2);
		gpio_set_irq_enabled(BUTTON2, GPIO_IRQ_EDGE_RISE, true);
	}
    gpio_set_dir(DATA,GPIO_IN);
    gpio_set_dir(LATCH,GPIO_OUT);
    gpio_set_dir(CLOCK,GPIO_OUT);
    gpio_set_dir(GAMECUBE_DATA, GPIO_OUT);
    gpio_pull_up(DATA);
    gpio_pull_up(GAMECUBE_DATA);
	gpio_init(ISCONTROLLERLED);
    gpio_set_dir(ISCONTROLLERLED,GPIO_OUT);
    if (ISNESLED != -1) {
        gpio_init(ISNESLED);
        gpio_set_dir(ISNESLED,GPIO_OUT);
    }
    if (ISSNESLED != -1) {
        gpio_init(ISSNESLED);
        gpio_set_dir(ISSNESLED,GPIO_OUT);
    }
    if (ISGAMECUBELED != -1) {
        gpio_init(ISGAMECUBELED);
        gpio_set_dir(ISGAMECUBELED,GPIO_OUT);
    }
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
short getSNESControllerInput(){
 /*
    0 - B
    1 - Y
    2 - Select
    3 - Start
    4 - Up
    5 - Down
    6 - Left
    7 - Right
    8 - A
    9 - X
    10 - Left Bumper
    11 - Right Bumper
*/
    short controllerData = 0;
    gpio_put(LATCH,1);
    sleep_us(12);
    gpio_put(LATCH,0);
    for (int i = 0; i < 12; i++){
        sleep_us(6);
        gpio_put(CLOCK,0);
        sleep_us(6);
        controllerData = (controllerData | (!gpio_get(DATA) << i));
        gpio_put(CLOCK,1);
    }
    return controllerData;
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
    if (controllerInput& 0x08) {
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

    tud_hid_keyboard_report(1, modifier, keycodes);  // Report ID 1 for keyboard
}
void emulateControllerNES(short controllerInput){
    uint8_t hat = GAMEPAD_HAT_CENTERED;
    uint32_t buttons = 0;
    // Map D-pad to hat switch (8 directions) Hat switches are 8 directional dpad
    bool up = controllerInput & 0x10;
    bool down = controllerInput & 0x20;
    bool left = controllerInput & 0x40;
    bool right = controllerInput & 0x80;

    if (up && right) {
      hat = GAMEPAD_HAT_UP_RIGHT;
    } else if (up && left) {
      hat = GAMEPAD_HAT_UP_LEFT;
    } else if (down && right) {
      hat = GAMEPAD_HAT_DOWN_RIGHT;
    } else if (down && left) {
      hat = GAMEPAD_HAT_DOWN_LEFT;
    } else if (up) {
      hat = GAMEPAD_HAT_UP;
    } else if (down) {
      hat = GAMEPAD_HAT_DOWN;
    } else if (left) {
      hat = GAMEPAD_HAT_LEFT;
    } else if (right) {
      hat = GAMEPAD_HAT_RIGHT;
    }

    // Map buttons to generic buttons
    if (controllerInput & 0x01) {
      buttons |= GAMEPAD_BUTTON_A;
    } // A
    if (controllerInput & 0x02) {
      buttons |= GAMEPAD_BUTTON_B;
    } // B
    if (controllerInput & 0x04) {
      buttons |= GAMEPAD_BUTTON_SELECT;
    } // Select
    if (controllerInput & 0x08) {
      buttons |= GAMEPAD_BUTTON_START;
    } // Start

    tud_hid_gamepad_report(2, 0, 0, 0, 0, 0, 0, hat, buttons);  // Report ID 2 for gamepad
}
void emulateKeyboardSNES(short controllerInput){
    uint8_t modifier = 0;
    uint8_t keycodes[6] = {0};
    int keyIndex = 0;

    if (controllerInput & 0x001) {
        // B -> Z
        keycodes[keyIndex++] = HID_KEY_Z;
    }
    if (controllerInput & 0x002) {
        // Y -> A
        keycodes[keyIndex++] = HID_KEY_A;
    }
    if (controllerInput & 0x004) {
        // Select -> Left Shift
        modifier |= KEYBOARD_MODIFIER_LEFTSHIFT;
    }
    if (controllerInput & 0x008) {
        // Start -> Enter
        keycodes[keyIndex++] = HID_KEY_ENTER;
    }
    if (controllerInput & 0x010) {
        // Up
        keycodes[keyIndex++] = HID_KEY_ARROW_UP;
    }
    if (controllerInput & 0x020) {
        // Down
        keycodes[keyIndex++] = HID_KEY_ARROW_DOWN;
    }
    if (controllerInput & 0x040) {
        // Left
        keycodes[keyIndex++] = HID_KEY_ARROW_LEFT;
    }
    if (controllerInput & 0x080) {
        // Right
        keycodes[keyIndex++] = HID_KEY_ARROW_RIGHT;
    }
    if (controllerInput & 0x100) {
        // A -> X
        keycodes[keyIndex++] = HID_KEY_X;
    }
    if (controllerInput & 0x200) {
        // X -> S
        keycodes[keyIndex++] = HID_KEY_S;
    }
    if (controllerInput & 0x400) {
        // Left Bumper -> Q
        keycodes[keyIndex++] = HID_KEY_Q;
    }
    if (controllerInput & 0x800) {
        // Right Bumper -> W
        keycodes[keyIndex++] = HID_KEY_W;
    }

    tud_hid_keyboard_report(1, modifier, keycodes);  // Report ID 1 for keyboard
}
void emulateControllerSNES(short controllerInput){
    uint8_t hat = GAMEPAD_HAT_CENTERED;
    uint32_t buttons = 0;
    // Map D-pad to hat switch (8 directions)
    bool up = controllerInput & 0x010;
    bool down = controllerInput & 0x020;
    bool left = controllerInput & 0x040;
    bool right = controllerInput & 0x080;
    if (up && right)        hat = GAMEPAD_HAT_UP_RIGHT;
    else if (up && left)    hat = GAMEPAD_HAT_UP_LEFT;
    else if (down && right) hat = GAMEPAD_HAT_DOWN_RIGHT;
    else if (down && left)  hat = GAMEPAD_HAT_DOWN_LEFT;
    else if (up)            hat = GAMEPAD_HAT_UP;
    else if (down)          hat = GAMEPAD_HAT_DOWN;
    else if (left)          hat = GAMEPAD_HAT_LEFT;
    else if (right)         hat = GAMEPAD_HAT_RIGHT;
    if (controllerInput & 0x001) {
      buttons |= GAMEPAD_BUTTON_B;
    } // B
    if (controllerInput & 0x002) {
      buttons |= GAMEPAD_BUTTON_Y;
    } // Y
    if (controllerInput & 0x004) {
      buttons |= GAMEPAD_BUTTON_SELECT;
    } // Select
    if (controllerInput & 0x008) {
      buttons |= GAMEPAD_BUTTON_START;
    } // Start
    if (controllerInput & 0x100) {
      buttons |= GAMEPAD_BUTTON_A;
    } // A
    if (controllerInput & 0x200) {
      buttons |= GAMEPAD_BUTTON_X;
    } // X
    if (controllerInput & 0x400) {
      buttons |= GAMEPAD_BUTTON_TL;
    } // Left Bumper
    if (controllerInput & 0x800) {
      buttons |= GAMEPAD_BUTTON_TR;
    } // Right Bumper

    tud_hid_gamepad_report(2, 0, 0, 0, 0, 0, 0, hat, buttons);  // Report ID 2 for gamepad
}

struct GameCubeData {
    uint8_t buttonHigh;
    uint8_t buttonLow;
    uint8_t stickX;
    uint8_t stickY;
    uint8_t triggerL;
    uint8_t triggerR;
};

void gamecube_write_byte(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        uint8_t bit = (byte >> i) & 1;
        if (bit) {
            gpio_put(GAMECUBE_DATA, 0);
            sleep_us(1);
            gpio_put(GAMECUBE_DATA, 1);
            sleep_us(3);
        } else {
            gpio_put(GAMECUBE_DATA, 0);
            sleep_us(3);
            gpio_put(GAMECUBE_DATA, 1);
            sleep_us(1);
        }
    }
}

uint8_t gamecube_read_byte(void) {
    uint8_t byte = 0;
    for (int i = 7; i >= 0; i--) {
        uint32_t start = time_us_32();
        while (!gpio_get(GAMECUBE_DATA) && (time_us_32() - start) < 10);
        start = time_us_32();
        while (gpio_get(GAMECUBE_DATA) && (time_us_32() - start) < 10);
        uint32_t duration = time_us_32() - start;
        if (duration > 1) {
            byte |= (1 << i);
        }
    }
    return byte;
}

struct GameCubeData getGameCubeControllerInput(void) {
    struct GameCubeData data = {0};
    gpio_set_dir(GAMECUBE_DATA, GPIO_OUT);
    gamecube_write_byte(0x40);
    sleep_us(100);
    gpio_set_dir(GAMECUBE_DATA, GPIO_IN);
    data.buttonHigh = gamecube_read_byte();
    data.buttonLow = gamecube_read_byte();
    data.stickX = gamecube_read_byte();
    data.stickY = gamecube_read_byte();
    data.triggerL = gamecube_read_byte();
    data.triggerR = gamecube_read_byte();
    return data;
}

void emulateKeyboardGameCube(struct GameCubeData data) {
    uint8_t modifier = 0;
    uint8_t keycodes[6] = {0};
    int keyIndex = 0;

    if (data.buttonHigh & 0x80) {
        keycodes[keyIndex++] = HID_KEY_X;
    }
    if (data.buttonHigh & 0x40) {
        keycodes[keyIndex++] = HID_KEY_Z;
    }
    if (data.buttonHigh & 0x20) {
        keycodes[keyIndex++] = HID_KEY_A;
    }
    if (data.buttonHigh & 0x10) {
        keycodes[keyIndex++] = HID_KEY_S;
    }
    if (data.buttonHigh & 0x04) {
        keycodes[keyIndex++] = HID_KEY_C;
    }
    if (data.buttonHigh & 0x02) {
        modifier |= KEYBOARD_MODIFIER_LEFTSHIFT;
    }
    if (data.buttonLow & 0x80) {
        keycodes[keyIndex++] = HID_KEY_ARROW_UP;
    }
    if (data.buttonLow & 0x40) {
        keycodes[keyIndex++] = HID_KEY_ARROW_DOWN;
    }
    if (data.buttonLow & 0x20) {
        keycodes[keyIndex++] = HID_KEY_ARROW_LEFT;
    }
    if (data.buttonLow & 0x10) {
        keycodes[keyIndex++] = HID_KEY_ARROW_RIGHT;
    }

    tud_hid_keyboard_report(1, modifier, keycodes);
}

void emulateControllerGameCube(struct GameCubeData data) {
    uint8_t hat = GAMEPAD_HAT_CENTERED;
    uint32_t buttons = 0;
    int8_t x = (int8_t)(data.stickX - 128);
    int8_t y = (int8_t)(data.stickY - 128);

    bool up = data.buttonLow & 0x80;
    bool down = data.buttonLow & 0x40;
    bool left = data.buttonLow & 0x20;
    bool right = data.buttonLow & 0x10;

    if (up && right)        hat = GAMEPAD_HAT_UP_RIGHT;
    else if (up && left)    hat = GAMEPAD_HAT_UP_LEFT;
    else if (down && right) hat = GAMEPAD_HAT_DOWN_RIGHT;
    else if (down && left)  hat = GAMEPAD_HAT_DOWN_LEFT;
    else if (up)            hat = GAMEPAD_HAT_UP;
    else if (down)          hat = GAMEPAD_HAT_DOWN;
    else if (left)          hat = GAMEPAD_HAT_LEFT;
    else if (right)         hat = GAMEPAD_HAT_RIGHT;

    if (data.buttonHigh & 0x80) buttons |= GAMEPAD_BUTTON_A;
    if (data.buttonHigh & 0x40) buttons |= GAMEPAD_BUTTON_B;
    if (data.buttonHigh & 0x20) buttons |= GAMEPAD_BUTTON_X;
    if (data.buttonHigh & 0x10) buttons |= GAMEPAD_BUTTON_Y;
    if (data.buttonHigh & 0x04) buttons |= GAMEPAD_BUTTON_TL;
    if (data.buttonHigh & 0x02) buttons |= GAMEPAD_BUTTON_START;
    if (data.triggerL > 100) buttons |= GAMEPAD_BUTTON_TL2;
    if (data.triggerR > 100) buttons |= GAMEPAD_BUTTON_TR2;

    tud_hid_gamepad_report(2, x, y, 0, 0, data.triggerL, data.triggerR, hat, buttons);
}

int main(){
    init();
    short controllerData = 0;
    struct GameCubeData gcData = {0};

    gpio_put(ISCONTROLLERLED, isController);
    gpio_put(ISNESLED, (controllerType == CONTROLLER_NES) ? 1 : 0);
    gpio_put(ISSNESLED, (controllerType == CONTROLLER_SNES) ? 1 : 0);
    gpio_put(ISGAMECUBELED, (controllerType == CONTROLLER_GAMECUBE) ? 1 : 0);

    while(1){
        tud_task();
        if (tud_hid_ready()){
            switch(controllerType) {
                case CONTROLLER_NES:
                    controllerData = getNESControllerInput();
                    if (isController) {
                        emulateControllerNES(controllerData);
                    } else {
                        emulateKeyboardNES(controllerData);
                    }
                    break;
                case CONTROLLER_SNES:
                    controllerData = getSNESControllerInput();
                    if (isController) {
                        emulateControllerSNES(controllerData);
                    } else {
                        emulateKeyboardSNES(controllerData);
                    }
                    break;
                case CONTROLLER_GAMECUBE:
                    gcData = getGameCubeControllerInput();
                    if (isController) {
                        emulateControllerGameCube(gcData);
                    } else {
                        emulateKeyboardGameCube(gcData);
                    }
                    break;
            }
            gpio_put(ISCONTROLLERLED, isController);
            gpio_put(ISNESLED, (controllerType == CONTROLLER_NES) ? 1 : 0);
            gpio_put(ISSNESLED, (controllerType == CONTROLLER_SNES) ? 1 : 0);
            gpio_put(ISGAMECUBELED, (controllerType == CONTROLLER_GAMECUBE) ? 1 : 0);
        }
        sleep_ms(10);
    }
    return 0;
}
