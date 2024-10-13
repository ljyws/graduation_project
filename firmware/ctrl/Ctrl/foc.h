#ifndef __FOC_H__
#define __FOC_H__
#include "phase_control_law.h"


class FOC : public AlphaBetaFrameController
{
public:

    void update();

    void reset() final;

    bool on_measuerment(
        std::optional<float> vbus_voltage,
        std::optional<float2D> Ialpha_beta,
        uint32_t input_timestamp) final;

    bool get_alpha_beta_output(
        uint32_t output_timestamp,
        std::optional<float2D> *mod_alpha_beta,
        std::optional<float> *ibus) final;

    std::optional<float2D> pi_gain_;
    float I_measured_report_filter_k_ = 1.0f;



};



#endif
