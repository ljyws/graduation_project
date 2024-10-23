// #include "test_foc.h"
// #include "math.h"
// #include "main.h"
// #include "tim.h"
// #define SQRT3 (1.73205080757f)        // 3µĆ½·½¸ù
// #define SQRT3_BY_2 (0.86602540378f)   // 3µĆ½·½¸ùµĒ»°덊
// #define ONE_BY_SQRT3 (0.57735026919f) // 1³ýҔ3µĆ½·½¸ù
// #define TWO_BY_SQRT3 (1.15470053838f) // 2³ýҔ3µĆ½·½¸ù
//
// #define SIN_TABLE_SIZE  512
// foc_para_t foc;
// static const float sinTable_f32[SIN_TABLE_SIZE + 1] = {
//    0.00000000f, 0.01227154f, 0.02454123f, 0.03680722f, 0.04906767f, 0.06132074f,
//    0.07356456f, 0.08579731f, 0.09801714f, 0.11022221f, 0.12241068f, 0.13458071f,
//    0.14673047f, 0.15885814f, 0.17096189f, 0.18303989f, 0.19509032f, 0.20711138f,
//    0.21910124f, 0.23105811f, 0.24298018f, 0.25486566f, 0.26671276f, 0.27851969f,
//    0.29028468f, 0.30200595f, 0.31368174f, 0.32531029f, 0.33688985f, 0.34841868f,
//    0.35989504f, 0.37131719f, 0.38268343f, 0.39399204f, 0.40524131f, 0.41642956f,
//    0.42755509f, 0.43861624f, 0.44961133f, 0.46053871f, 0.47139674f, 0.48218377f,
//    0.49289819f, 0.50353838f, 0.51410274f, 0.52458968f, 0.53499762f, 0.54532499f,
//    0.55557023f, 0.56573181f, 0.57580819f, 0.58579786f, 0.59569930f, 0.60551104f,
//    0.61523159f, 0.62485949f, 0.63439328f, 0.64383154f, 0.65317284f, 0.66241578f,
//    0.67155895f, 0.68060100f, 0.68954054f, 0.69837625f, 0.70710678f, 0.71573083f,
//    0.72424708f, 0.73265427f, 0.74095113f, 0.74913639f, 0.75720885f, 0.76516727f,
//    0.77301045f, 0.78073723f, 0.78834643f, 0.79583690f, 0.80320753f, 0.81045720f,
//    0.81758481f, 0.82458930f, 0.83146961f, 0.83822471f, 0.84485357f, 0.85135519f,
//    0.85772861f, 0.86397286f, 0.87008699f, 0.87607009f, 0.88192126f, 0.88763962f,
//    0.89322430f, 0.89867447f, 0.90398929f, 0.90916798f, 0.91420976f, 0.91911385f,
//    0.92387953f, 0.92850608f, 0.93299280f, 0.93733901f, 0.94154407f, 0.94560733f,
//    0.94952818f, 0.95330604f, 0.95694034f, 0.96043052f, 0.96377607f, 0.96697647f,
//    0.97003125f, 0.97293995f, 0.97570213f, 0.97831737f, 0.98078528f, 0.98310549f,
//    0.98527764f, 0.98730142f, 0.98917651f, 0.99090264f, 0.99247953f, 0.99390697f,
//    0.99518473f, 0.99631261f, 0.99729046f, 0.99811811f, 0.99879546f, 0.99932238f,
//    0.99969882f, 0.99992470f, 1.00000000f, 0.99992470f, 0.99969882f, 0.99932238f,
//    0.99879546f, 0.99811811f, 0.99729046f, 0.99631261f, 0.99518473f, 0.99390697f,
//    0.99247953f, 0.99090264f, 0.98917651f, 0.98730142f, 0.98527764f, 0.98310549f,
//    0.98078528f, 0.97831737f, 0.97570213f, 0.97293995f, 0.97003125f, 0.96697647f,
//    0.96377607f, 0.96043052f, 0.95694034f, 0.95330604f, 0.94952818f, 0.94560733f,
//    0.94154407f, 0.93733901f, 0.93299280f, 0.92850608f, 0.92387953f, 0.91911385f,
//    0.91420976f, 0.90916798f, 0.90398929f, 0.89867447f, 0.89322430f, 0.88763962f,
//    0.88192126f, 0.87607009f, 0.87008699f, 0.86397286f, 0.85772861f, 0.85135519f,
//    0.84485357f, 0.83822471f, 0.83146961f, 0.82458930f, 0.81758481f, 0.81045720f,
//    0.80320753f, 0.79583690f, 0.78834643f, 0.78073723f, 0.77301045f, 0.76516727f,
//    0.75720885f, 0.74913639f, 0.74095113f, 0.73265427f, 0.72424708f, 0.71573083f,
//    0.70710678f, 0.69837625f, 0.68954054f, 0.68060100f, 0.67155895f, 0.66241578f,
//    0.65317284f, 0.64383154f, 0.63439328f, 0.62485949f, 0.61523159f, 0.60551104f,
//    0.59569930f, 0.58579786f, 0.57580819f, 0.56573181f, 0.55557023f, 0.54532499f,
//    0.53499762f, 0.52458968f, 0.51410274f, 0.50353838f, 0.49289819f, 0.48218377f,
//    0.47139674f, 0.46053871f, 0.44961133f, 0.43861624f, 0.42755509f, 0.41642956f,
//    0.40524131f, 0.39399204f, 0.38268343f, 0.37131719f, 0.35989504f, 0.34841868f,
//    0.33688985f, 0.32531029f, 0.31368174f, 0.30200595f, 0.29028468f, 0.27851969f,
//    0.26671276f, 0.25486566f, 0.24298018f, 0.23105811f, 0.21910124f, 0.20711138f,
//    0.19509032f, 0.18303989f, 0.17096189f, 0.15885814f, 0.14673047f, 0.13458071f,
//    0.12241068f, 0.11022221f, 0.09801714f, 0.08579731f, 0.07356456f, 0.06132074f,
//    0.04906767f, 0.03680722f, 0.02454123f, 0.01227154f, 0.00000000f, -0.01227154f,
//    -0.02454123f, -0.03680722f, -0.04906767f, -0.06132074f, -0.07356456f,
//    -0.08579731f, -0.09801714f, -0.11022221f, -0.12241068f, -0.13458071f,
//    -0.14673047f, -0.15885814f, -0.17096189f, -0.18303989f, -0.19509032f,
//    -0.20711138f, -0.21910124f, -0.23105811f, -0.24298018f, -0.25486566f,
//    -0.26671276f, -0.27851969f, -0.29028468f, -0.30200595f, -0.31368174f,
//    -0.32531029f, -0.33688985f, -0.34841868f, -0.35989504f, -0.37131719f,
//    -0.38268343f, -0.39399204f, -0.40524131f, -0.41642956f, -0.42755509f,
//    -0.43861624f, -0.44961133f, -0.46053871f, -0.47139674f, -0.48218377f,
//    -0.49289819f, -0.50353838f, -0.51410274f, -0.52458968f, -0.53499762f,
//    -0.54532499f, -0.55557023f, -0.56573181f, -0.57580819f, -0.58579786f,
//    -0.59569930f, -0.60551104f, -0.61523159f, -0.62485949f, -0.63439328f,
//    -0.64383154f, -0.65317284f, -0.66241578f, -0.67155895f, -0.68060100f,
//    -0.68954054f, -0.69837625f, -0.70710678f, -0.71573083f, -0.72424708f,
//    -0.73265427f, -0.74095113f, -0.74913639f, -0.75720885f, -0.76516727f,
//    -0.77301045f, -0.78073723f, -0.78834643f, -0.79583690f, -0.80320753f,
//    -0.81045720f, -0.81758481f, -0.82458930f, -0.83146961f, -0.83822471f,
//    -0.84485357f, -0.85135519f, -0.85772861f, -0.86397286f, -0.87008699f,
//    -0.87607009f, -0.88192126f, -0.88763962f, -0.89322430f, -0.89867447f,
//    -0.90398929f, -0.90916798f, -0.91420976f, -0.91911385f, -0.92387953f,
//    -0.92850608f, -0.93299280f, -0.93733901f, -0.94154407f, -0.94560733f,
//    -0.94952818f, -0.95330604f, -0.95694034f, -0.96043052f, -0.96377607f,
//    -0.96697647f, -0.97003125f, -0.97293995f, -0.97570213f, -0.97831737f,
//    -0.98078528f, -0.98310549f, -0.98527764f, -0.98730142f, -0.98917651f,
//    -0.99090264f, -0.99247953f, -0.99390697f, -0.99518473f, -0.99631261f,
//    -0.99729046f, -0.99811811f, -0.99879546f, -0.99932238f, -0.99969882f,
//    -0.99992470f, -1.00000000f, -0.99992470f, -0.99969882f, -0.99932238f,
//    -0.99879546f, -0.99811811f, -0.99729046f, -0.99631261f, -0.99518473f,
//    -0.99390697f, -0.99247953f, -0.99090264f, -0.98917651f, -0.98730142f,
//    -0.98527764f, -0.98310549f, -0.98078528f, -0.97831737f, -0.97570213f,
//    -0.97293995f, -0.97003125f, -0.96697647f, -0.96377607f, -0.96043052f,
//    -0.95694034f, -0.95330604f, -0.94952818f, -0.94560733f, -0.94154407f,
//    -0.93733901f, -0.93299280f, -0.92850608f, -0.92387953f, -0.91911385f,
//    -0.91420976f, -0.90916798f, -0.90398929f, -0.89867447f, -0.89322430f,
//    -0.88763962f, -0.88192126f, -0.87607009f, -0.87008699f, -0.86397286f,
//    -0.85772861f, -0.85135519f, -0.84485357f, -0.83822471f, -0.83146961f,
//    -0.82458930f, -0.81758481f, -0.81045720f, -0.80320753f, -0.79583690f,
//    -0.78834643f, -0.78073723f, -0.77301045f, -0.76516727f, -0.75720885f,
//    -0.74913639f, -0.74095113f, -0.73265427f, -0.72424708f, -0.71573083f,
//    -0.70710678f, -0.69837625f, -0.68954054f, -0.68060100f, -0.67155895f,
//    -0.66241578f, -0.65317284f, -0.64383154f, -0.63439328f, -0.62485949f,
//    -0.61523159f, -0.60551104f, -0.59569930f, -0.58579786f, -0.57580819f,
//    -0.56573181f, -0.55557023f, -0.54532499f, -0.53499762f, -0.52458968f,
//    -0.51410274f, -0.50353838f, -0.49289819f, -0.48218377f, -0.47139674f,
//    -0.46053871f, -0.44961133f, -0.43861624f, -0.42755509f, -0.41642956f,
//    -0.40524131f, -0.39399204f, -0.38268343f, -0.37131719f, -0.35989504f,
//    -0.34841868f, -0.33688985f, -0.32531029f, -0.31368174f, -0.30200595f,
//    -0.29028468f, -0.27851969f, -0.26671276f, -0.25486566f, -0.24298018f,
//    -0.23105811f, -0.21910124f, -0.20711138f, -0.19509032f, -0.18303989f,
//    -0.17096189f, -0.15885814f, -0.14673047f, -0.13458071f, -0.12241068f,
//    -0.11022221f, -0.09801714f, -0.08579731f, -0.07356456f, -0.06132074f,
//    -0.04906767f, -0.03680722f, -0.02454123f, -0.01227154f, -0.00000000f
// };
//
// float sin_f32(float x)
// {
//     float sinVal, fract, in;                           /* Temporary variables for input, output */
//     uint16_t index;                                    /* Index variable */
//     float a, b;                                        /* Two nearest output values */
//     int32_t n;
//     float findex;
//
//     /* input x is in radians */
//     /* Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi */
//     in = x * 0.159154943092f;
//
//     /* Calculation of floor value of input */
//     n = (int32_t) in;
//
//     /* Make negative values towards -infinity */
//     if (x < 0.0f){
//         n--;
//     }
//
//     /* Map input value to [0 1] */
//     in = in - (float) n;
//
//     /* Calculation of index of the table */
//     findex = (float)SIN_TABLE_SIZE * in;
//     index = (uint16_t)findex;
//
//     /* when "in" is exactly 1, we need to rotate the index down to 0 */
//     if (index >= SIN_TABLE_SIZE) {
//         index = 0;
//         findex -= (float)SIN_TABLE_SIZE;
//     }
//
//     /* fractional value calculation */
//     fract = findex - (float) index;
//
//     /* Read two nearest values of input value from the sin table */
//     a = sinTable_f32[index];
//     b = sinTable_f32[index+1];
//
//     /* Linear interpolation process */
//     sinVal = (1.0f-fract)*a + fract*b;
//
//     /* Return the output value */
//     return (sinVal);
// }
//
// float cos_f32(float x)
// {
//     float cosVal, fract, in;                   /* Temporary variables for input, output */
//     uint16_t index;                                /* Index variable */
//     float a, b;                                /* Two nearest output values */
//     int32_t n;
//     float findex;
//
//     /* input x is in radians */
//     /* Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi, add 0.25 (pi/2) to read sine table */
//     in = x * 0.159154943092f + 0.25f;
//
//     /* Calculation of floor value of input */
//     n = (int32_t) in;
//
//     /* Make negative values towards -infinity */
//     if (in < 0.0f){
//         n--;
//     }
//
//     /* Map input value to [0 1] */
//     in = in - (float) n;
//
//     /* Calculation of index of the table */
//     findex = (float)SIN_TABLE_SIZE * in;
//     index = (uint16_t)findex;
//
//     /* when "in" is exactly 1, we need to rotate the index down to 0 */
//     if (index >= SIN_TABLE_SIZE) {
//         index = 0;
//         findex -= (float)SIN_TABLE_SIZE;
//     }
//
//     /* fractional value calculation */
//     fract = findex - (float) index;
//
//     /* Read two nearest values of input value from the cos table */
//     a = sinTable_f32[index];
//     b = sinTable_f32[index+1];
//
//     /* Linear interpolation process */
//     cosVal = (1.0f-fract)*a + fract*b;
//
//     /* Return the output value */
//     return (cosVal);
// }
//
// #define min(x, y) (((x) < (y)) ? (x) : (y)) // ȡ½ϐ¡ֵ
// #define max(x, y) (((x) > (y)) ? (x) : (y)) // ȡ½ϴ�
//
//
// /**
// ***********************************************************************
// * @brief:      foc_calc(foc_para_t *foc)
// * @param[in]:  foc  FOC�����ṹ��ָ��
// * @retval:     void
// * @details:    FOC�㷨����Ҫ���������ڼ������ĵ����͵�ѹ����
// ***********************************************************************
// **/
// void foc_calc(foc_para_t *foc)
// {
//     //    sin_cos_val(foc);
//     //    clarke_transform(foc);
//     //    park_transform(foc);
//     //    inverse_park(foc);
//     //    svpwm_midpoint(foc);
//     foc->sin_val = sin_f32(foc->theta);
//     foc->cos_val = cos_f32(foc->theta);
//     foc->i_alpha = foc->i_a;
//     foc->i_beta = (foc->i_b - foc->i_c) * ONE_BY_SQRT3;
//     foc->i_d = foc->i_alpha *foc->cos_val + foc->i_beta   *foc->sin_val;
//     foc->i_q = foc->i_beta   *foc->cos_val - foc->i_alpha *foc->sin_val;
//     foc->v_alpha = (foc->v_d *foc->cos_val - foc->v_q *foc->sin_val);
//     foc->v_beta  = (foc->v_d *foc->sin_val + foc->v_q *foc->cos_val);
//     svm(foc->v_alpha*(foc->inv_vbus), foc->v_beta*(foc->inv_vbus), &foc->dtc_a, &foc->dtc_b, &foc->dtc_c);
//     //		svpwm_sector(foc);
// }
//
// /**
// ***********************************************************************
// * @brief:      sin_cos_val(foc_para_t *foc)
// * @param[in]:  foc  FOC�����ṹ��ָ��
// * @retval:     void
// * @details:    �����Ƕ� theta ��Ӧ�� sin �� cos ֵ
// ***********************************************************************
// **/
// void sin_cos_val(foc_para_t *foc)
// {
//     foc->sin_val = sin_f32(foc->theta);
//     foc->cos_val = cos_f32(foc->theta);
// }
//
// /**
// ***********************************************************************
// * @brief:      clarke_transform(foc_para_t *foc)
// * @param[in]:  foc  FOC�����ṹ��ָ��
// * @retval:     void
// * @details:    Clarke�任���������ת��Ϊ Alpha-Beta ����ϵ�µĵ���
// ***********************************************************************
// **/
// void clarke_transform(foc_para_t *foc)
// {
//     foc->i_alpha = foc->i_a;
//     foc->i_beta = (foc->i_b - foc->i_c) * ONE_BY_SQRT3;
// }
//
// /**
// ***********************************************************************
// * @brief:      inverse_clarke(foc_para_t *foc)
// * @param[in]:  foc  FOC�����ṹ��ָ��
// * @retval:     void
// * @details:    ��Clarke�任�� Alpha-Beta ����ϵ�µĵ�ѹת��Ϊ�����ѹ
// ***********************************************************************
// **/
// void inverse_clarke(foc_para_t *foc)
// {
//     foc->v_a = foc->v_alpha;
//     foc->v_b = -0.5f * foc->v_alpha + SQRT3_BY_2 *foc->v_beta;
//     foc->v_c = -0.5f * foc->v_alpha - SQRT3_BY_2 *foc->v_beta;
// }
//
// /**
// ***********************************************************************
// * @brief:      void park_transform(foc_para_t *foc)
// * @param[in]:  foc  FOC�����ṹ��ָ��
// * @retval:     void
// * @details:    Park�任�� Alpha-Beta ����ϵ�µĵ���ת��Ϊ dq ����ϵ�µĵ���
// ***********************************************************************
// **/
// void park_transform(foc_para_t *foc)
// {
//     foc->i_d = foc->i_alpha *foc->cos_val + foc->i_beta   *foc->sin_val;
//     foc->i_q = foc->i_beta   *foc->cos_val - foc->i_alpha *foc->sin_val;
// }
//
// /**
// ***********************************************************************
// * @brief:      inverse_park(foc_para_t *foc)
// * @param[in]:  foc  FOC�����ṹ��ָ��
// * @retval:     void
// * @details:    ��Park�任�� dq ����ϵ�µĵ�ѹת��Ϊ Alpha-Beta ����ϵ�µĵ�ѹ
// ***********************************************************************
// **/
// void inverse_park(foc_para_t *foc)
// {
//     foc->v_alpha = (foc->v_d *foc->cos_val - foc->v_q *foc->sin_val);
//     foc->v_beta  = (foc->v_d *foc->sin_val + foc->v_q *foc->cos_val);
// }
//
//
// void svpwm_midpoint(foc_para_t *foc)
// {
//     foc->v_alpha = foc->inv_vbus *foc->v_alpha;
//     foc->v_beta = foc->inv_vbus *foc->v_beta;
//     float va = foc->v_alpha;
//     float vb = -0.5f * foc->v_alpha + SQRT3_BY_2 *foc->v_beta;
//     float vc = -0.5f * foc->v_alpha - SQRT3_BY_2 *foc->v_beta;
//     float vmax = max(max(va, vb), vc);
//     float vmin = min(min(va, vb), vc);
//     float vcom = (vmax + vmin) * 0.5f;
//     foc->dtc_a = (vcom - va) + 0.5f;
//     foc->dtc_b = (vcom - vb) + 0.5f;
//     foc->dtc_c = (vcom - vc) + 0.5f;
// }
//
// void svpwm_sector(foc_para_t *foc)
// {
//     float TS = 1.0f;
//     float ta = 0.0f, tb = 0.0f, tc = 0.0f;
//     float k = (TS *SQRT3) * foc->inv_vbus;
//     float va = foc->v_beta;
//     float vb = (SQRT3 *foc->v_alpha - foc->v_beta) * 0.5f;
//     float vc = (-SQRT3 *foc->v_alpha - foc->v_beta) * 0.5f;
//     int a = (va > 0.0f) ? 1 : 0;
//     int b = (vb > 0.0f) ? 1 : 0;
//     int c = (vc > 0.0f) ? 1 : 0;
//     int sextant = (c << 2) + (b << 1) + a;
//
//     switch (sextant)
//     {
//     case 3:
//     {
//         float t4 = k *vb;
//         float t6 = k *va;
//         float t0 = (TS - t4 - t6) * 0.5f;
//         ta = t4 + t6 + t0;
//         tb = t6 + t0;
//         tc = t0;
//     }
//     break;
//
//     case 1:
//     {
//         float t6 = -k *vc;
//         float t2 = -k *vb;
//         float t0 = (TS - t2 - t6) * 0.5f;
//         ta = t6 + t0;
//         tb = t2 + t6 + t0;
//         tc = t0;
//     }
//     break;
//
//     case 5:
//     {
//         float t2 = k *va;
//         float t3 = k *vc;
//         float t0 = (TS - t2 - t3) * 0.5f;
//         ta = t0;
//         tb = t2 + t3 + t0;
//         tc = t3 + t0;
//     }
//     break;
//
//     case 4:
//     {
//         float t1 = -k *va;
//         float t3 = -k *vb;
//         float t0 = (TS - t1 - t3) * 0.5f;
//         ta = t0;
//         tb = t3 + t0;
//         tc = t1 + t3 + t0;
//     }
//     break;
//
//     case 6:
//     {
//         float t1 = k *vc;
//         float t5 = k *vb;
//         float t0 = (TS - t1 - t5) * 0.5f;
//         ta = t5 + t0;
//         tb = t0;
//         tc = t1 + t5 + t0;
//     }
//     break;
//
//     case 2:
//     {
//         float t4 = -k *vc;
//         float t5 = -k *va;
//         float t0 = (TS - t4 - t5) * 0.5f;
//         ta = t4 + t5 + t0;
//         tb = t0;
//         tc = t5 + t0;
//     }
//     break;
//
//     default:
//         break;
//     }
//
//     foc->dtc_a = 1.0f - ta;
//     foc->dtc_b = 1.0f - tb;
//     foc->dtc_c = 1.0f - tc;
// }
//
// int svpwm(foc_para_t *foc)
// {
//     int Sextant;
//     foc->v_alpha = foc->v_alpha*(foc->inv_vbus);
//     foc->v_beta  = foc->v_beta *(foc->inv_vbus);
//
//     if (foc->v_beta >= 0.0f)
//     {
//         if (foc->v_alpha >= 0.0f)
//         {
//             //quadrant I
//             if (ONE_BY_SQRT3 *foc->v_beta > foc->v_alpha)
//             {
//                 Sextant = 2;    //sextant v2-v3
//             }
//             else
//             {
//                 Sextant = 1;    //sextant v1-v2
//             }
//         }
//         else
//         {
//             //quadrant II
//             if (-ONE_BY_SQRT3 *foc-> v_beta > foc->v_alpha)
//             {
//                 Sextant = 3;    //sextant v3-v4
//             }
//             else
//             {
//                 Sextant = 2;    //sextant v2-v3
//             }
//         }
//     }
//     else
//     {
//         if (foc->v_alpha >= 0.0f)
//         {
//             //quadrant IV
//             if (-ONE_BY_SQRT3 *foc->v_beta > foc->v_alpha)
//             {
//                 Sextant = 5;    //sextant v5-v6
//             }
//             else
//             {
//                 Sextant = 6;    //sextant v6-v1
//             }
//         }
//         else
//         {
//             //quadrant III
//             if (ONE_BY_SQRT3 *foc->v_beta > foc->v_alpha)
//             {
//                 Sextant = 4;    //sextant v4-v5
//             }
//             else
//             {
//                 Sextant = 5;    //sextant v5-v6
//             }
//         }
//     }
//
//     switch (Sextant)
//     {
//     // sextant v1-v2
//     case 1:
//     {
//         // Vector on-times
//         float t1 = foc->v_alpha - ONE_BY_SQRT3 *foc->v_beta;
//         float t2 = TWO_BY_SQRT3 *foc->v_beta;
//         // PWM timings
//         foc->dtc_a = (1.0f - t1 - t2) * 0.5f;
//         foc->dtc_b = foc->dtc_a + t1;
//         foc->dtc_c = foc->dtc_b + t2;
//     }
//     break;
//
//     // sextant v2-v3
//     case 2:
//     {
//         // Vector on-times
//         float t2 = foc->v_alpha + ONE_BY_SQRT3 *foc->v_beta;
//         float t3 = -foc->v_alpha + ONE_BY_SQRT3 *foc->v_beta;
//         // PWM timings
//         foc->dtc_b = (1.0f - t2 - t3) * 0.5f;
//         foc->dtc_a = foc->dtc_b + t3;
//         foc->dtc_c = foc->dtc_a + t2;
//     }
//     break;
//
//     // sextant v3-v4
//     case 3:
//     {
//         // Vector on-times
//         float t3 = TWO_BY_SQRT3 *foc->v_beta;
//         float t4 = -foc->v_alpha - ONE_BY_SQRT3 *foc->v_beta;
//         // PWM timings
//         foc->dtc_b = (1.0f - t3 - t4) * 0.5f;
//         foc->dtc_c = foc->dtc_b + t3;
//         foc->dtc_a = foc->dtc_c + t4;
//     }
//     break;
//
//     // sextant v4-v5
//     case 4:
//     {
//         // Vector on-times
//         float t4 = -foc->v_alpha + ONE_BY_SQRT3 *foc->v_beta;
//         float t5 = -TWO_BY_SQRT3 *foc->v_beta;
//         // PWM timings
//         foc->dtc_c = (1.0f - t4 - t5) * 0.5f;
//         foc->dtc_b = foc->dtc_c + t5;
//         foc->dtc_a = foc->dtc_b + t4;
//     }
//     break;
//
//     // sextant v5-v6
//     case 5:
//     {
//         // Vector on-times
//         float t5 = -foc->v_alpha - ONE_BY_SQRT3 *foc->v_beta;
//         float t6 = foc->v_alpha - ONE_BY_SQRT3 *foc->v_beta;
//         // PWM timings
//         foc->dtc_c = (1.0f - t5 - t6) * 0.5f;
//         foc->dtc_a = foc->dtc_c + t5;
//         foc->dtc_b = foc->dtc_a + t6;
//     }
//     break;
//
//     // sextant v6-v1
//     case 6:
//     {
//         // Vector on-times
//         float t6 = -TWO_BY_SQRT3 *foc->v_beta;
//         float t1 = foc->v_alpha + ONE_BY_SQRT3 *foc->v_beta;
//         // PWM timings
//         foc->dtc_a = (1.0f - t6 - t1) * 0.5f;
//         foc->dtc_c = foc->dtc_a + t1;
//         foc->dtc_b = foc->dtc_c + t6;
//     }
//     break;
//     }
//
//     // if any of the results becomes NaN, result_valid will evaluate to false
//     int result_valid = foc->dtc_a >= 0.0f && foc->dtc_a <= 1.0f &&
//                        foc->dtc_b >= 0.0f && foc->dtc_b <= 1.0f &&
//                        foc->dtc_c >= 0.0f && foc->dtc_c <= 1.0f;
//     return result_valid ? 0 : -1;
// }
//
//
//
// int svm(float alpha, float beta, float *tA, float *tB, float *tC)
// {
//     int Sextant;
//
//     if (beta >= 0.0f)
//     {
//         if (alpha >= 0.0f)
//         {
//             //quadrant I
//             if (ONE_BY_SQRT3 *beta > alpha)
//             {
//                 Sextant = 2;    //sextant v2-v3
//             }
//             else
//             {
//                 Sextant = 1;    //sextant v1-v2
//             }
//         }
//         else
//         {
//             //quadrant II
//             if (-ONE_BY_SQRT3 *beta > alpha)
//             {
//                 Sextant = 3;    //sextant v3-v4
//             }
//             else
//             {
//                 Sextant = 2;    //sextant v2-v3
//             }
//         }
//     }
//     else
//     {
//         if (alpha >= 0.0f)
//         {
//             //quadrant IV
//             if (-ONE_BY_SQRT3 *beta > alpha)
//             {
//                 Sextant = 5;    //sextant v5-v6
//             }
//             else
//             {
//                 Sextant = 6;    //sextant v6-v1
//             }
//         }
//         else
//         {
//             //quadrant III
//             if (ONE_BY_SQRT3 *beta > alpha)
//             {
//                 Sextant = 4;    //sextant v4-v5
//             }
//             else
//             {
//                 Sextant = 5;    //sextant v5-v6
//             }
//         }
//     }
//
//     switch (Sextant)
//     {
//     // sextant v1-v2
//     case 1:
//     {
//         // Vector on-times
//         float t1 = alpha - ONE_BY_SQRT3 *beta;
//         float t2 = TWO_BY_SQRT3 *beta;
//         // PWM timings
//         *tA = (1.0f - t1 - t2) * 0.5f;
//         *tB = *tA + t1;
//         *tC = *tB + t2;
//     }
//     break;
//
//     // sextant v2-v3
//     case 2:
//     {
//         // Vector on-times
//         float t2 = alpha + ONE_BY_SQRT3 *beta;
//         float t3 = -alpha + ONE_BY_SQRT3 *beta;
//         // PWM timings
//         *tB = (1.0f - t2 - t3) * 0.5f;
//         *tA = *tB + t3;
//         *tC = *tA + t2;
//     }
//     break;
//
//     // sextant v3-v4
//     case 3:
//     {
//         // Vector on-times
//         float t3 = TWO_BY_SQRT3 *beta;
//         float t4 = -alpha - ONE_BY_SQRT3 *beta;
//         // PWM timings
//         *tB = (1.0f - t3 - t4) * 0.5f;
//         *tC = *tB + t3;
//         *tA = *tC + t4;
//     }
//     break;
//
//     // sextant v4-v5
//     case 4:
//     {
//         // Vector on-times
//         float t4 = -alpha + ONE_BY_SQRT3 *beta;
//         float t5 = -TWO_BY_SQRT3 *beta;
//         // PWM timings
//         *tC = (1.0f - t4 - t5) * 0.5f;
//         *tB = *tC + t5;
//         *tA = *tB + t4;
//     }
//     break;
//
//     // sextant v5-v6
//     case 5:
//     {
//         // Vector on-times
//         float t5 = -alpha - ONE_BY_SQRT3 *beta;
//         float t6 = alpha - ONE_BY_SQRT3 *beta;
//         // PWM timings
//         *tC = (1.0f - t5 - t6) * 0.5f;
//         *tA = *tC + t5;
//         *tB = *tA + t6;
//     }
//     break;
//
//     // sextant v6-v1
//     case 6:
//     {
//         // Vector on-times
//         float t6 = -TWO_BY_SQRT3 *beta;
//         float t1 = alpha + ONE_BY_SQRT3 *beta;
//         // PWM timings
//         *tA = (1.0f - t6 - t1) * 0.5f;
//         *tC = *tA + t1;
//         *tB = *tC + t6;
//     }
//     break;
//     }
//
//     // if any of the results becomes NaN, result_valid will evaluate to false
//     int result_valid = *tA >= 0.0f && *tA <= 1.0f && *tB >= 0.0f && *tB <= 1.0f && *tC >= 0.0f && *tC <= 1.0f;
//     return result_valid ? 0 : -1;
// }
//
//
// #define set_dtc_a(value)     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, value)
// #define set_dtc_b(value)     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, value)
// #define set_dtc_c(value)     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, value)
//
// void foc_volt(float vd_ref, float vq_ref, float pos)
// {
//     foc.theta = pos;
//     wrap_0_2pi(foc.theta);
//     sin_cos_val(&foc);
//     foc.vbus = 24.0f;
//     foc.inv_vbus = 1.5f/(foc.vbus);
//     clarke_transform(&foc);
//     park_transform(&foc);
//
//     foc.v_d = vd_ref;
//     foc.v_q = vq_ref;
//
//     inverse_park(&foc);
//
//     svpwm(&foc);
//
//     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, foc.dtc_a * 4200.0f);
//     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, foc.dtc_b * 4200.0f);
//     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, foc.dtc_c * 4200.0f);
//
// }
//
