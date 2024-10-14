#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "board.h"
#include "component.h"
#include "utils.h"
#include <cmath>

class Axis;

class Encoder
{
public:

    static constexpr uint32_t MODE_FLAG_ABS = 0x100;
    typedef enum
    {
        MODE_SPI_ABS_MT6816 = 256,
        MODE_SPI_ABS_MT6825 = 257,
        MODE_SPI_ABS_AS5047 = 258,
    }mode_e;
    typedef struct
    {
        mode_e mode = MODE_SPI_ABS_MT6825;
        float bandwith = 1000.0f;
        int32_t phase_offset = 0;
        float phase_offset_float = 0.0f;
        int32_t cpr = (2048*4);
        int32_t direction = 0;
        bool enable_phase_interpolation = true;
        Encoder *parent = nullptr;
    }config_t;

    Encoder(STM32_SPI *enc_spi);
    void init(void);
    void abs_spi_cs_pin_init(void);
    void update_pll_gains();


    bool update();



    int32_t pos_abs_ = 0;

    void sample_now();
    bool abs_spi_start_transaction();
    void abs_spi_cb();



    bool abs_spi_pos_updated_ = false;

    Axis *axis_ = nullptr;
    STM32_SPI *spi_;
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
    OutputPort<float> pos_estimate_ = 0.0f; // [turn]
    OutputPort<float> vel_estimate_ = 0.0f; // [turn/s]
    OutputPort<float> pos_circular_ = 0.0f; // [turn]


    mode_e mode_;


    uint16_t abs_spi_dma_tx_[1] = {0xFFFF};
    uint16_t abs_spi_dma_rx_[1]{};
    STM32_GPIO spi_abs_cs_gpio_;
    STM32_SPI::spi_task spi_task_;
};



#endif

