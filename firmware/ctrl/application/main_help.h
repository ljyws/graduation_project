#ifndef __MAIN_HELP_H__
#define __MAIN_HELP_H__

#include "board.h"

#ifdef __cplusplus
// #include <communication/interface_usb.h>
// #include <communication/interface_i2c.h>
// #include <communication/interface_uart.h>
// #include <task_timer.hpp>
extern "C"{
#endif

#include "cmsis_os.h"

    // extern uint32_t _reboot_cookie;
    //
    // extern uint64_t serial_number;
    // extern char serial_number_str[13];
#ifdef __cplusplus
}

class Axis;
class Motor;

#include "utils.hpp"
#include "encoder.hpp"
#include "axis.hpp"

extern "C"{
    // extern const unsigned char fw_version_major_;
    // extern const unsigned char fw_version_minor_;
    // extern const unsigned char fw_version_revision_;
    // extern const unsigned char fw_version_unreleased_;
}


#endif


#endif
