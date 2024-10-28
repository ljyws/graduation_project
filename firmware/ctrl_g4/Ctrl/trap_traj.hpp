#ifndef __TRAP_TRAJ_HPP__
#define __TRAP_TRAJ_HPP__


class TrapezoidalTrajectory
{
public:
    typedef struct
    {
        float vel_limit = 2.0f;   // [turn/s]
        float accel_limit = 0.5f; // [turn/s^2]
        float decel_limit = 0.5f; // [turn/s^2]
    }Config_t;

    typedef struct
    {
        float Y;
        float Yd;
        float Ydd;
    }Step_t;

    bool planTrapezoidal(float Xf, float Xi, float Vi,
                         float Vmax, float Amax, float Dmax);
    Step_t eval(float t);

    Axis* axis_ = nullptr;  // set by Axis constructor
    Config_t config_;

    float Xi_;
    float Xf_;
    float Vi_;

    float Ar_;
    float Vr_;
    float Dr_;

    float Ta_;
    float Tv_;
    float Td_;
    float Tf_;

    float yAccel_;

    float t_;
};

#endif

