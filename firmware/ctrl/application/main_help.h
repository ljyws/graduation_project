#ifndef __MAIN_HELP_H__
#define __MAIN_HELP_H__

#include "board.h"

#ifdef __cplusplus

extern "C"{
#endif

#include "cmsis_os.h"

#ifdef __cplusplus
}

class Axis;
class Motor;

#include "utils.hpp"
#include "low_level.h"
#include "encoder.hpp"
#include "axis.hpp"

extern "C" {
    extern const unsigned char fw_version_major_;
    extern const unsigned char fw_version_minor_;
    extern const unsigned char fw_version_revision_;
    extern const unsigned char fw_version_unreleased_;
}

class Ctrl
{
public:
    bool save_configuration();
    void erase_configuration();
    void reboot() { NVIC_SystemReset(); }
    void enter_dfu_mode();
    bool any_error();
    void clear_errors();

    void sampling_cb();

    void control_loop_cb();

    float& vbus_voltage_ = ::vbus_voltage; // TODO: make this the actual variable
    float& ibus_ = ::ibus_; // TODO: make this the actual variable
    float ibus_report_filter_k_ = 1.0f;





};

extern Ctrl ctrl;

#endif


#endif
