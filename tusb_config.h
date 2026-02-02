#ifndef TUSB_CONFIG_H
#define TUSB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CFG_TUSB_MCU
#define CFG_TUSB_MCU          OPT_MCU_RP2040
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS           OPT_OS_PICO
#endif

#define CFG_TUSB_DEBUG        0

// RHPort configuration
#define CFG_TUSB_RHPORT0_MODE OPT_MODE_DEVICE

// Memory section and alignment
#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN    __attribute__((aligned(4)))

//--------------------------------------------------------------------
// Device Configuration
//--------------------------------------------------------------------
#define CFG_TUD_ENABLED       1
#define CFG_TUD_ENDPOINT0_SIZE 64

// Class drivers
#define CFG_TUD_HID           1
#define CFG_TUD_CDC           0
#define CFG_TUD_MSC           0
#define CFG_TUD_MIDI          0
#define CFG_TUD_VENDOR        0

// HID buffer size
#define CFG_TUD_HID_EP_BUFSIZE 8

#ifdef __cplusplus
}
#endif

#endif
