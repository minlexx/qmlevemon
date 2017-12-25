#include <cmath>
#include <QtGlobal>
#include "formulas.h"

/*
 * Reference: http://eveonline-third-party-documentation.readthedocs.io/en/latest/formulas/skillpoints.html
 */


float EM::skill_points_per_minute(int attr_primary, int attr_secondary)
{
    float fap = static_cast<float>(attr_primary);
    float fas = static_cast<float>(attr_secondary);
    return fap + 0.5*fas;
}


int EM::skill_points_needed_for_skill(int skill_rank, int skill_level)
{
    // skillpoints = 2^(2.5(skilllevel−1))  * 250 * skillrank
    Q_ASSERT(skill_rank >= 1);
    Q_ASSERT(skill_level >= 1);
    double stepen = 2.5 * (static_cast<double>(skill_level) - 1.0);
    double res = pow(2, stepen);
    res *= (250 * static_cast<double>(skill_rank));
    return static_cast<int>(res);
}
