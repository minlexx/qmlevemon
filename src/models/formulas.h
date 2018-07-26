#ifndef H_FORMULAS
#define H_FORMULAS

#include <stdint.h>

namespace EM {

float skill_points_per_minute(int attr_primary, int attr_secondary);
uint64_t skill_points_needed_for_skill_level(int skill_rank, int skill_level);

}

#endif
