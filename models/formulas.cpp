#include <cmath>
#include "formulas.h"

/*
 * Reference: http://eveonline-third-party-documentation.readthedocs.io/en/latest/formulas/skillpoints.html
 */

namespace EM {


float skill_points_per_minute(int attr_primary, int attr_secondary)
{
    float fap = static_cast<float>(attr_primary);
    float fas = static_cast<float>(attr_secondary);
    return fap + 0.5f * fas;
}


uint64_t skill_points_needed_for_skill_level(int skill_rank, int skill_level)
{
    // only return valid result for valid params... no asserts in release code
    if ((skill_rank <= 0) || (skill_level <= 0)) {
        return 0;
    }

    // Formula is:
    // skillpoints = 2^(2.5*(skilllevel−1))  * 250 * skillrank
    double stepen = 2.5 * (static_cast<double>(skill_level) - 1.0);
    double res = pow(2.0, stepen);
    res *= (250.0 * static_cast<double>(skill_rank));
    return static_cast<uint64_t>(ceil(res));
}

}
