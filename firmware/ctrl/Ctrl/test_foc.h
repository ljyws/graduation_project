// #ifndef __FOC_CALC_H__
// #define __FOC_CALC_H__
//
// #include "main.h"
//
// #define M_2PI (6.28318530716f)        // 2±¶Բ֜
//
// #define wrap_0_2pi(theta) theta = (theta > M_2PI) ? theta - M_2PI : theta; theta = (theta < 0.0f ) ? theta + M_2PI : theta;
//
//
// typedef struct
// {
//     float vbus;
//     float ibus;
//     float inv_vbus; // ĸϟµ瑹µ¹ʽ
//
//     float duty_now;
//
//     float i_abs;
//
//     float theta;   // ½Ƕȍ
//     float sin_val; // ´˽Ƕȶԓ¦µĕýϒֵ
//     float cos_val; // ´˽Ƕȶԓ¦µēϒֵ
//
//     float i_a; // A Ϡµ灷
//     float i_b; // B Ϡµ灷
//     float i_c; // C Ϡµ灷
//
//     float v_a; // A Ϡµ瑹
//     float v_b; // B Ϡµ瑹
//     float v_c; // C Ϡµ瑹
//
//     float i_d; // D ׸±ꏵµ灷
//     float i_q; // Q ׸±ꏵµ灷
//
//
//     float v_d; // D ׸±ꏵµ瑹
//     float v_q; // Q ׸±ꏵµ瑹
//
//     float i_alpha; // Alpha ׸±ꏵµ灷
//     float i_beta;  // Beta ׸±ꏵµ灷
//
//
//     float v_alpha; // Alpha ׸±ꏵµ瑹
//     float v_beta;  // Beta ׸±ꏵµ瑹
//
//     float dtc_a; // A Ϡ PWM ռ¿ձȍ
//     float dtc_b; // B Ϡ PWM ռ¿ձȍ
//     float dtc_c; // C Ϡ PWM ռ¿ձȍ
//
// } foc_para_t;
//
//
// void foc_calc(foc_para_t *foc);
// void sin_cos_val(foc_para_t *foc);
// void clarke_transform(foc_para_t *foc);
// void inverse_clarke(foc_para_t *foc);
// void park_transform(foc_para_t *foc);
// void inverse_park(foc_para_t *foc);
// void svpwm_midpoint(foc_para_t *foc);
// void svpwm_sector(foc_para_t *foc);
// int svpwm(foc_para_t *foc);
// int svm(float alpha, float beta, float* tA, float* tB, float* tC);
//
// void foc_volt(float vd_ref, float vq_ref, float pos);
// void foc_curr(float id_set, float iq_set, float pos);
// void foc_vel(float vel_set, float iq_set, float pos);
// void foc_pos(float pos_set, float vel_set, float iq_set, float pos);
//
// void foc_if_spd_ctrl(uint8_t N);
// void foc_spd_ctrl(uint8_t N);
// void foc_ctrl_mode(void);
// void motor_ctrl(void);
// void foc_update_current_ctrl_gain(uint16_t bandwidth);
// #endif
//
