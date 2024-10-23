#ifndef __ENCODER_H__
#define __ENCODER_H__

class Axis;
class Encoder;

#include "board.h"
#include "utils.hpp"
#include "component.hpp"
#include <cmath>

class Encoder
{
public:

    static constexpr uint32_t MODE_FLAG_MagnTek = 0x100;
    typedef enum
    {
        MODE_SPI_ABS_MT6816 = 256,
        MODE_SPI_ABS_MT6825 = 257,
    }mode_e;
    typedef struct
    {
        mode_e mode = MODE_SPI_ABS_MT6825;
        float calib_range = 0.02f; // Accuracy required to pass encoder cpr check
        float calib_scan_distance = 16.0f * M_PI; // rad electrical
        float calib_scan_omega = 4.0f * M_PI; // rad/s electrical
        float bandwith = 1000.0f;
        int32_t phase_offset = 0;
        float phase_offset_float = 0.0f;
        int32_t cpr = (262144);
        int32_t direction = 1;
        bool enable_phase_interpolation = true;
        Encoder *parent = nullptr;
    }config_t;

    Encoder(STM32_SPI *spi);
    void init(void);
    bool apply_config();
    void abs_spi_cs_pin_init(void);
    void update_pll_gains();


    bool update();

    bool run_offset_calibration();


    int32_t pos_abs_ = 0;

    void sample_now();
    bool abs_spi_start_transaction();
    void abs_spi_cb();



    bool abs_spi_pos_updated_ = false;

    STM32_SPI *spi_;
    Axis *axis_ = nullptr;

    config_t config_;

    bool is_ready_ = false;
    OutputPort<float> phase_ = 0.0f;     // [rad]
    OutputPort<float> phase_vel_ = 0.0f; // [rad/s]

    float interpolation_ = 0.0f;
    float pos_estimate_counts_ = 0.0f;  // [count]
    float pos_cpr_counts_ = 0.0f;  // [count]
    float delta_pos_cpr_counts_ = 0.0f;  // [count] phase detector result for debug
    float vel_estimate_counts_ = 0.0f;  // [count/s]


    int32_t shadow_count_ = 0;
    int32_t count_in_cpr_ = 0;
    float pll_kp_ = 0.0f;   // [count/s / count]
    float pll_ki_ = 0.0f;   // [(count/s^2) / count]
    float calib_scan_response_ = 0.0f;
    OutputPort<float> pos_estimate_ = 0.0f; // [turn]
    OutputPort<float> vel_estimate_ = 0.0f; // [turn/s]
    OutputPort<float> pos_circular_ = 0.0f; // [turn]


    mode_e mode_;


    uint16_t abs_spi_dma_tx_[2] = {};
    uint16_t abs_spi_dma_rx_[2] = {};

    STM32_GPIO spi_abs_cs_gpio_{GPIOA,GPIO_PIN_4};
    STM32_SPI::spi_task spi_task_;
};



#endif

