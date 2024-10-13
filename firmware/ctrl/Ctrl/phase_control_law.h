#ifndef __PHASE_CONTROL_LAW_H__
#define __PHASE_CONTROL_LAW_H__
#include <array>
#include <optional>

using float2D = std::pair<float, float>;
struct Iph_ABC_t { float phA; float phB; float phC; };


template<size_t N_PHASE>
class PhaseControlLaw
{
public:
    virtual void reset() = 0;

    virtual bool on_measurement(
        std::optional<float> vbus_voltage,
        std::optional<std::array<float,N_PHASE>> currents,
        uint32_t input_timestamp) = 0;

    virtual bool get_output(
        uint32_t output_timestamp,
        float (&pwm_timings)[N_PHASE],
        std::optional<float> *ibus
    ) = 0;

};


class AlphaBetaFrameController : public PhaseControlLaw<3>
{
private:
    bool on_measurement(
        std::optional<float> vbus_voltage,
        std::optional<std::array<float, 3>> currents,
        uint32_t input_timestamp) final;

    bool get_output(
        uint32_t output_timestamp,
        float(&pwm_timings)[3],
        std::optional<float> *ibus) final;

protected:

    virtual bool on_measuerment(
        std::optional<float> vbus_voltage,
        std::optional<float2D> Ialpha_beta,
        uint32_t input_timestamp
    ) = 0;

    virtual bool get_alpha_beta_output(
        uint32_t output_timestamp,
        std::optional<float2D> *mod_alpha_beta,
        std::optional<float> *ibus
    ) = 0;
};




#endif
