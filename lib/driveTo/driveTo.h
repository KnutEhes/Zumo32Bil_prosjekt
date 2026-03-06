#include "motor.h"

struct pos
{
    float x; 
    float y; 
};

bool driveTo(const pos& bil, const pos& point);
