#include "drv8301.hpp"

#include "board.h"
#include "cmsis_os2.h"
#include "utils.hpp"

const SPI_InitTypeDef DRV8301::spi_config_ = {
    .Mode = SPI_MODE_MASTER,
    .Direction = SPI_DIRECTION_2LINES,
    .DataSize = SPI_DATASIZE_16BIT,
    .CLKPolarity = SPI_POLARITY_LOW,
    .CLKPhase = SPI_PHASE_2EDGE,
    .NSS = SPI_NSS_SOFT,
    .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
    .FirstBit = SPI_FIRSTBIT_MSB,
    .TIMode = SPI_TIMODE_DISABLE,
    .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
    .CRCPolynomial = 10,
};

bool DRV8301::config(float requested_gain, float *actual_gain)
{
    // for reference:
    // 20V/V on 500uOhm gives a range of +/- 150A
    // 40V/V on 500uOhm gives a range of +/- 75A
    // 20V/V on 666uOhm gives a range of +/- 110A
    // 40V/V on 666uOhm gives a range of +/- 55A

    uint16_t gain_setting = 3;
    float gain_choices[] = {10.0f, 20.0f, 40.0f, 80.0f};

    while (gain_setting && (gain_choices[gain_setting] > requested_gain))
    {
        gain_setting--;
    }

    if(actual_gain)
    {
        *actual_gain = gain_choices[gain_setting];
    }

    RegisterFile new_config;

    new_config.control_register_1 =
          (21 << 6) // Overcurrent set to approximately 150A at 100degC. This may need tweaking.
        | (0b01 << 4) // OCP_MODE: latch shut down
        | (0b0 << 3) // 6x PWM mode
        | (0b0 << 2) // don't reset latched faults
        | (0b00 << 0); // gate-drive peak current: 1.7A

    new_config.control_register_2 =
          (0b0 << 6) // OC_TOFF: cycle by cycle
        | (0b00 << 4) // calibration off (normal operation)
        | (gain_setting << 2) // select gain
        | (0b00 << 0); // report both over temperature and over current on nOCTW pin

    bool regs_equal = (regs_.control_register_1 == new_config.control_register_1)
                   && (regs_.control_register_2 == new_config.control_register_2);

    if(!regs_equal)
    {
        regs_ = new_config;
        state_ = kStateUninitialized;
        enable_gpio_.write(false);
    }

    return true;
}

bool DRV8301::init()
{
    uint16_t val;
    if(state_ == kStateReady)
    {
        return true;
    }
    enable_gpio_.write(true);
    delay_us(40);
    state_ = kStateUninitialized;
    enable_gpio_.write(true);
    osDelay(20);

    // Write current configuration
    bool wrote_regs    =  write_reg(kRegNameControl1, regs_.control_register_1)
                       && write_reg(kRegNameControl1, regs_.control_register_1)
                       && write_reg(kRegNameControl1, regs_.control_register_1)
                       && write_reg(kRegNameControl1, regs_.control_register_1)
                       && write_reg(kRegNameControl1, regs_.control_register_1) // the write operation tends to be ignored if only done once (not sure why)
                       && write_reg(kRegNameControl2, regs_.control_register_2);
    if (!wrote_regs)
    {
        return false;
    }

    delay_us(100);
    state_ = kStateStartupChecks;

    bool is_read_regs = read_reg(kRegNameControl1, &val) && (val == regs_.control_register_1)
                  && read_reg(kRegNameControl2, &val) && (val == regs_.control_register_2);
    if (!is_read_regs)
    {
        return false;
    }

    if (get_error() != FaultType_NoFault)
    {
        return false;
    }

    if (state_ == kStateStartupChecks)
    {
    state_ = kStateReady;
    }

    return state_ == kStateReady;
}

void DRV8301::do_check()
{
    if (state_ != kStateUninitialized && !nfault_gpio_.read())
    {
        state_ = kStateUninitialized;
    }
}

bool DRV8301::is_ready()
{
    return state_ == kStateReady;
}

DRV8301::FaultType_e DRV8301::get_error()
{
    uint16_t fault1, fault2;

    if (!read_reg(kRegNameStatus1, &fault1) ||
        !read_reg(kRegNameStatus2, &fault2))
        {
        return (FaultType_e)0xffffffff;
        }

    return (FaultType_e)((uint32_t)fault1 | ((uint32_t)(fault2 & 0x0080) << 16));
}

bool DRV8301::read_reg(const RegName_e regName, uint16_t *data)
{
    tx_buf_ = build_ctrl_word(DRV8301_CtrlMode_Read, regName, 0);
    HAL_SPI_TransmitReceive(&hspi3,(uint8_t*)&tx_buf_,(uint8_t*)&rx_buf_,1,1000);
    // if (!spi_arbiter_->transfer(spi_config_, cs_gpio_, (uint8_t *)(&tx_buf_), nullptr, 1, 1000))
    // {
    //     return false;
    // }

    delay_us(1);

    tx_buf_ = build_ctrl_word(DRV8301_CtrlMode_Read, regName, 0);
    rx_buf_ = 0xffff;
    HAL_SPI_TransmitReceive(&hspi3,(uint8_t*)&tx_buf_,(uint8_t*)&rx_buf_,1,1000);
    // if (!spi_arbiter_->transfer(spi_config_, cs_gpio_, (uint8_t *)(&tx_buf_), (uint8_t *)(&rx_buf_), 1, 1000))
    // {
    //     return false;
    // }

    delay_us(1);

    if (rx_buf_ == 0xbeef)
    {
        return false;
    }

    if (data)
    {
        *data = rx_buf_ & 0x07FF;
    }

    return true;
}

bool DRV8301::write_reg(const RegName_e regName, const uint16_t data)
{
    // Do blocking write
    tx_buf_ = build_ctrl_word(DRV8301_CtrlMode_Write, regName, data);
    HAL_SPI_TransmitReceive(&hspi3,(uint8_t*)&tx_buf_,(uint8_t*)&rx_buf_,1,1000);
    // uint8_t _tx_buf_ = 0x70;
    // uint8_t _rx_buf_ ;
     // spi_arbiter_->transfer(spi_config_,cs_gpio_,(uint8_t*)&tx_buf_,(uint8_t*)&rx_buf_,1,100);
    // if (!spi_arbiter_->transfer(spi_config_, cs_gpio_, (uint8_t *)(&tx_buf_), (uint8_t *)&rx_buf_, 1, 1000))
    // {
    //     return false;
    // }
    delay_us(1);

    return true;
}





