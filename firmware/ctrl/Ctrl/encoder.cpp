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
    update_pll_gains();
    is_ready_ = true;

}

bool Encoder::apply_config()
{
    update_pll_gains();
    return true;
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
        spi_task_.cs_gpio = spi_abs_cs_gpio_;
        spi_task_.tx_buf = (uint8_t*)&abs_spi_dma_tx_;
        spi_task_.rx_buf = (uint8_t*)&abs_spi_dma_rx_;
        abs_spi_cb();
        spi_task_.len = 2;
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
            abs_spi_start_transaction();
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

    float delta_pos_counts     = (float)(shadow_count_ - std::floor(pos_estimate_counts_));
    float delta_pos_cpr_counts = (float)(count_in_cpr_ - std::floor(pos_cpr_counts_));


    delta_pos_cpr_counts = wrap_pm(delta_pos_cpr_counts,(float)(config_.cpr));
    delta_pos_cpr_counts_ += 0.1f * (delta_pos_cpr_counts - delta_pos_cpr_counts_);
    //pll feedback
    pos_estimate_counts_ += current_meas_period * pll_kp_ * delta_pos_cpr_counts;
    pos_cpr_counts_ += current_meas_period * pll_kp_ * delta_pos_cpr_counts;
    pos_cpr_counts_ = fmodf_pos(pos_cpr_counts_, (float)(config_.cpr));
    vel_estimate_counts_ += current_meas_period * pll_ki_ * delta_pos_cpr_counts;
    bool snap_to_zero_vel = false;

    if(std::abs(vel_estimate_counts_) < 0.5f * current_meas_period * pll_ki_)
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

    float elec_rad_per_enc = axis_->motor_.config_.pole_pairs * 2 * M_PI * (1.0f / (float)(config_.cpr));
    float ph = elec_rad_per_enc * (interpolated_enc - config_.phase_offset_float);

    if (is_ready_)
    {
        phase_ = wrap_pm_pi(ph) * config_.direction;
        phase_vel_ = (2*M_PI) * *vel_estimate_.present() * axis_->motor_.config_.pole_pairs * config_.direction;
    }
    return true;
}


void Encoder::abs_spi_cs_pin_init()
{
    spi_abs_cs_gpio_.config(GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_LOW);
    spi_abs_cs_gpio_.write(true);
}

bool Encoder::run_offset_calibration()
{
    const float start_lock_duration = 1.0f;

    shadow_count_ = count_in_cpr_;

    // Reset state variables
    axis_->open_loop_controller_.Idq_setpoint_ = {0.0f, 0.0f};
    axis_->open_loop_controller_.Vdq_setpoint_ = {0.0f, 0.0f};
    axis_->open_loop_controller_.phase_ = 0.0f;
    axis_->open_loop_controller_.phase_vel_ = 0.0f;

    float max_current_ramp = axis_->motor_.config_.calibration_current / start_lock_duration * 2.0f;
    axis_->open_loop_controller_.max_current_ramp_ = max_current_ramp;
    axis_->open_loop_controller_.max_voltage_ramp_ = max_current_ramp;
    axis_->open_loop_controller_.max_phase_vel_ramp_ = INFINITY;
    axis_->open_loop_controller_.target_current_ =  axis_->motor_.config_.calibration_current;
    axis_->open_loop_controller_.target_voltage_ =  0.0f ;
    axis_->open_loop_controller_.target_vel_ = 0.0f;
    axis_->open_loop_controller_.total_distance_ = 0.0f;
    axis_->open_loop_controller_.phase_ = axis_->open_loop_controller_.initial_phase_ = wrap_pm_pi(0 - config_.calib_scan_distance / 2.0f);

    // axis_->motor_.current_control_.enable_current_control_src_ = true;
    // axis_->motor_.current_control_.Idq_setpoint_src_.connect_to(&axis_->open_loop_controller_.Idq_setpoint_);
    // axis_->motor_.current_control_.Vdq_setpoint_src_.connect_to(&axis_->open_loop_controller_.Vdq_setpoint_);
    //
    // axis_->motor_.current_control_.phase_src_.connect_to(&axis_->open_loop_controller_.phase_);


    axis_->motor_.phase_vel_src_.connect_to(&axis_->open_loop_controller_.phase_vel_);
    // axis_->motor_.current_control_.phase_vel_src_.connect_to(&axis_->open_loop_controller_.phase_vel_);

    // axis_->motor_.arm(&axis_->motor_.current_control_);

    // // go to start position of forward scan for start_lock_duration to get ready to scan
    // for (size_t i = 0; i < (size_t)(start_lock_duration * 1000.0f); ++i)
    // {
    //     if (!axis_->motor_.is_armed_)
    //     {
    //         return false;
    //     }
    //     if (axis_->requested_state_ != Axis::AXIS_STATE_UNDEFINED)
    //     {
    //         axis_->motor_.disarm();
    //         return false;
    //     }
    //     osDelay(1);
    // }
    //
    //
    // int32_t init_enc_val = shadow_count_;
    // uint32_t num_steps = 0;
    // int64_t encvaluesum = 0;
    //
    axis_->open_loop_controller_.target_vel_ = config_.calib_scan_omega;
    axis_->open_loop_controller_.total_distance_ = 0.0f;
    //
    // while ((axis_->requested_state_ == Axis::AXIS_STATE_UNDEFINED) && axis_->motor_.is_armed_)
    // {
    //     bool reached_target_dist = axis_->open_loop_controller_.total_distance_.any().value_or(-INFINITY) >= config_.calib_scan_distance;
    //     if (reached_target_dist)
    //     {
    //         break;
    //     }
    //     encvaluesum += shadow_count_;
    //     num_steps++;
    //     osDelay(1);
    // }
    //
    // // Check response and direction
    // if (shadow_count_ > init_enc_val + 8)
    // {
    //     // motor same dir as encoder
    //     config_.direction = 1;
    // } else if (shadow_count_ < init_enc_val - 8)
    // {
    //     // motor opposite dir as encoder
    //     config_.direction = -1;
    // } else
    // {
    //     axis_->motor_.disarm();
    //     return false;
    // }
    //
    // float elec_rad_per_enc = axis_->motor_.config_.pole_pairs * 2 * M_PI * (1.0f / (float)(config_.cpr));
    // float expected_encoder_delta = config_.calib_scan_distance / elec_rad_per_enc;
    // calib_scan_response_ = std::abs(shadow_count_ - init_enc_val);
    // if (std::abs(calib_scan_response_ - expected_encoder_delta) / expected_encoder_delta > config_.calib_range)
    // {
    //     axis_->motor_.disarm();
    //     return false;
    // }
    //
    // axis_->open_loop_controller_.target_vel_ = -config_.calib_scan_omega;
    //
    // while ((axis_->requested_state_ == Axis::AXIS_STATE_UNDEFINED) && axis_->motor_.is_armed_)
    // {
    //     bool reached_target_dist = axis_->open_loop_controller_.total_distance_.any().value_or(INFINITY) <= 0.0f;
    //     if (reached_target_dist)
    //     {
    //         break;
    //     }
    //     encvaluesum += shadow_count_;
    //     num_steps++;
    //     osDelay(1);
    // }
    //
    // if (!axis_->motor_.is_armed_)
    // {
    //     return false;
    // }
    //
    // axis_->motor_.disarm();
    //
    // config_.phase_offset = encvaluesum / num_steps;
    // int32_t residual = encvaluesum - ((int64_t)config_.phase_offset * (int64_t)num_steps);
    // config_.phase_offset_float = (float)residual / (float)num_steps + 0.5f;  // add 0.5 to center-align state to phase
    //
    // is_ready_ = true;
    return true;
}
