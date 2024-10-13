#ifndef __DRV8301_H__
#define __DRV8301_H__


#include "stm32_gpio.h"
#include "stm32_spi.h"


class DRV8301
{
public:
    typedef enum : uint32_t {
        FaultType_NoFault  = (0 << 0),  //!< No fault

        // Status Register 1
        FaultType_FETLC_OC = (1 << 0),  //!< FET Low side, Phase C Over Current fault
        FaultType_FETHC_OC = (1 << 1),  //!< FET High side, Phase C Over Current fault
        FaultType_FETLB_OC = (1 << 2),  //!< FET Low side, Phase B Over Current fault
        FaultType_FETHB_OC = (1 << 3),  //!< FET High side, Phase B Over Current fault
        FaultType_FETLA_OC = (1 << 4),  //!< FET Low side, Phase A Over Current fault
        FaultType_FETHA_OC = (1 << 5),  //!< FET High side, Phase A Over Current fault
        FaultType_OTW      = (1 << 6),  //!< Over Temperature Warning fault
        FaultType_OTSD     = (1 << 7),  //!< Over Temperature Shut Down fault
        FaultType_PVDD_UV  = (1 << 8),  //!< Power supply Vdd Under Voltage fault
        FaultType_GVDD_UV  = (1 << 9),  //!< DRV8301 Vdd Under Voltage fault
        FaultType_FAULT    = (1 << 10),

        // Status Register 2
        FaultType_GVDD_OV  = (1 << 23)  //!< DRV8301 Vdd Over Voltage fault
    } FaultType_e;

    DRV8301(STM32_SPI *spi,STM32_GPIO cs_gpio, STM32_GPIO enable_gpio, STM32_GPIO nfault_gpio);

    bool config(float requested_gain, float *actual_gain);

    bool init(void);

    void do_check(void);

    bool is_ready();

    FaultType_e get_error();

private:
    enum CtrlMode_e
    {
        DRV8301_CtrlMode_Read = 1 << 15,   //!< Read Mode
        DRV8301_CtrlMode_Write = 0 << 15   //!< Write Mode
    };

    enum RegName_e
    {
        kRegNameStatus1  = 0 << 11,  //!< Status Register 1
        kRegNameStatus2  = 1 << 11,  //!< Status Register 2
        kRegNameControl1 = 2 << 11,  //!< Control Register 1
        kRegNameControl2 = 3 << 11   //!< Control Register 2
    };

    struct RegisterFile
    {
        uint16_t control_register_1;
        uint16_t control_register_2;
    };

    static inline uint16_t build_ctrl_word(const CtrlMode_e ctrlMode,
                                           const RegName_e regName,
                                           const uint16_t data)
    {
        return ctrlMode | regName | (data & 0x07FF);
    }

    bool read_reg(const RegName_e regName, uint16_t* data);

    bool write_reg(const RegName_e regName, const uint16_t data);

    static const SPI_InitTypeDef spi_config_;

    STM32_SPI *spi_;
    STM32_GPIO cs_gpio_;
    STM32_GPIO enable_gpio_;
    STM32_GPIO nfault_gpio_;

    RegisterFile regs_;

    uint16_t tx_buf_;
    uint16_t rx_buf_;

    enum {
        kStateUninitialized,
        kStateStartupChecks,
        kStateReady,
    } state_ = kStateUninitialized;



};


#endif