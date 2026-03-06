#include "motor.h"

struct pos
{
    float x; 
    float y; 
    char type;
};

bool driveTo(const pos& bil, const pos& point);
