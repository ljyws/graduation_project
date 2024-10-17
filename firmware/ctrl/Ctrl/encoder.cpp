#include "main_help.h"

Encoder::Encoder(STM32_SPI *spi) : spi_(spi){
}

void Encoder::init()
{
    mode_ = config_.mode;
    spi_task_.config =
    {
        .Mode = SPI_MODE_MASTER,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_16BIT,
        .CLKPolarity = (mode_ == MODE_SPI_ABS_MT6825||mode_ == MODE_SPI_ABS_MT6816)?SPI_POLARITY_HIGH:SPI_POLARITY_LOW,
        .CLKPhase = SPI_PHASE_2EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
        .FirstBit = SPI_FIRSTBIT_MSB,
        .TIMode = SPI_TIMODE_DISABLE,
        .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
        .CRCPolynomial = 10,
    };

    if(mode_ & MODE_FLAG_MagnTek)
    {
        abs_spi_cs_pin_init();
    }

}

void Encoder::update_pll_gains()
{
    pll_kp_ = 2.0f * config_.bandwith;
    pll_ki_ = 0.25f * (pll_kp_ * pll_kp_);
}

void Encoder::sample_now()
{
    switch (mode_)
    {
        case MODE_SPI_ABS_MT6816:
        {
            break;
        }
        case MODE_SPI_ABS_MT6825:
        {
            abs_spi_dma_tx_[0] = 0x83ff;
            abs_spi_dma_tx_[1] = 0xffff;
            abs_spi_start_transaction();
            break;
        }
        default:break;
    }
}
uint16_t rx[2];
bool Encoder::abs_spi_start_transaction()
{
    if(mode_ & MODE_FLAG_MagnTek)
    {
//        uint16_t tx[2] = {0x83ff,0xffff};
//
//        spi_task_.cs_gpio.write(false);
//
//        HAL_SPI_TransmitReceive(&hspi1,(uint8_t*)&tx[0],(uint8_t*)&rx[0],1,1000);
//        HAL_SPI_TransmitReceive(&hspi1,(uint8_t*)&tx[1],(uint8_t*)&rx[1],1,1000);
//
//        spi_task_.cs_gpio.write(true);
        spi_task_.cs_gpio = spi_abs_cs_gpio_;
        spi_task_.tx_buf = (uint8_t*)&abs_spi_dma_tx_[0];
        spi_task_.rx_buf = (uint8_t*)&abs_spi_dma_rx_[0];
        spi_task_.len = 1;
        spi_->transfer_async(&spi_task_);

        spi_task_.tx_buf = (uint8_t*)&abs_spi_dma_tx_[1];
        spi_task_.rx_buf = (uint8_t*)&abs_spi_dma_rx_[1];
        spi_task_.len = 1;
        spi_->transfer_async(&spi_task_);
    }
    return true;
}

void Encoder::abs_spi_cb()
{
    uint32_t pos;
    switch (mode_)
    {
        case MODE_SPI_ABS_MT6816:
        {
            break;
        }
        case MODE_SPI_ABS_MT6825:
        {
            pos = ((abs_spi_dma_rx_[0]&0x00FF)<<10)|((abs_spi_dma_rx_[1]&0xFC00)>>6)|((abs_spi_dma_rx_[1]&0x00F0)>>4);
            break;
        }
        default:break;
    }

    pos_abs_ = pos;
    abs_spi_pos_updated_ = true;
}

bool Encoder::update()
{
    int32_t delta_enc = 0;
    int32_t pos_abs_latched = pos_abs_;

    switch (mode_)
    {
        case MODE_SPI_ABS_MT6816:
        case MODE_SPI_ABS_MT6825:
        {
            // abs_spi_start_transaction();
            break;
        }
        default:
        {
            return false;
            break;
        }
    }

    shadow_count_ += delta_enc;
    count_in_cpr_ += delta_enc;
    count_in_cpr_ = mod(count_in_cpr_,config_.cpr);

    if(mode_ & MODE_FLAG_MagnTek)
        count_in_cpr_ = pos_abs_latched;

    float pos_cpr_counts_last = pos_cpr_counts_;

    //run pll
    pos_estimate_counts_ += current_meas_period * vel_estimate_counts_;
    pos_cpr_counts_      += current_meas_period * vel_estimate_counts_;

    float delta_pos_cpr_counts = (float)(count_in_cpr_ - std::floor(pos_cpr_counts_));
    float delta_pos_counts     = (float)(shadow_count_ - std::floor(pos_estimate_counts_));

    delta_pos_cpr_counts = wrap_pm(delta_pos_cpr_counts,0.5f * (float)(config_.cpr));

    //pll feedback
    pos_estimate_counts_ += current_meas_period * pll_kp_ * delta_pos_cpr_counts;
    pos_cpr_counts_ += current_meas_period * pll_kp_ * delta_pos_cpr_counts;
    pos_cpr_counts_ = fmodf_pos(pos_cpr_counts_, (float)(config_.cpr));
    vel_estimate_counts_ += current_meas_period * pll_ki_ * delta_pos_cpr_counts;
    bool snap_to_zero_vel = false;

    if(abs(vel_estimate_counts_) < 0.5f * current_meas_period * pll_ki_)
    {
        vel_estimate_counts_ = 0.0f;
        snap_to_zero_vel = true;
    }

    //output
    pos_estimate_ = pos_estimate_counts_ / (float)config_.cpr;
    vel_estimate_ = vel_estimate_counts_ / (float)config_.cpr;

    float pos_circular = pos_circular_.any().value_or(0.0f);
    pos_circular += wrap_pm((pos_cpr_counts_ - pos_cpr_counts_last)/(float)config_.cpr,1.0f);
    pos_circular = fmodf_pos(pos_circular, 1.0f);
    pos_circular_ = pos_circular;

    int32_t corrected_enc = count_in_cpr_ - config_.phase_offset;
    if (snap_to_zero_vel || !config_.enable_phase_interpolation)
        interpolation_ = 0.5f;
    else if (delta_enc > 0)
        interpolation_ = 0.0f;
    else if (delta_enc < 0)
        interpolation_ = 1.0f;
    else
    {
        interpolation_ += current_meas_period * vel_estimate_counts_;
        if (interpolation_ > 1.0f) interpolation_ = 1.0f;
        if (interpolation_ < 0.0f) interpolation_ = 0.0f;
    }

    float interpolated_enc = corrected_enc + interpolation_;

    float elec_rad_per_enc = 7.0f * 2 * M_PI * (1.0f / (float)(config_.cpr));
    float ph = elec_rad_per_enc * (interpolated_enc - config_.phase_offset_float);

    if (is_ready_)
    {
        phase_ = wrap_pm_pi(ph) * config_.direction;
        phase_vel_ = (2*M_PI) * *vel_estimate_.present() * 7.0f * config_.direction;
    }
    return true;
}


void Encoder::abs_spi_cs_pin_init()
{
    spi_abs_cs_gpio_.config(GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_LOW);
    spi_abs_cs_gpio_.write(true);
}
